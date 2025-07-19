
/* GPIO interrupt & soft interrupt */
#include <zephyr/init.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/logging/log.h>
#include <zephyr/shell/shell.h>

LOG_MODULE_REGISTER(motion_int, LOG_LEVEL_DBG);

static const struct gpio_dt_spec motion_pin = GPIO_DT_SPEC_GET(DT_NODELABEL(my_pin), gpios);
static struct gpio_callback motion_cb;

// Forward declaration
void trigger_motion_isr_soft(void);

static void motion_isr(const struct device *dev, struct gpio_callback *cb, uint32_t pins) {
    if (pins & BIT(motion_pin.pin)) {
        LOG_INF("Motion detected! (source: %s)", 
                k_is_in_isr() ? "HARDWARE" : "SOFTWARE");
    }
}

void trigger_motion_isr_soft(void) {
    const struct device *fake_dev = motion_pin.port;
    uint32_t fake_pins = BIT(motion_pin.pin);
    motion_isr(fake_dev, &motion_cb, fake_pins);
}

static int init_motion_interrupt(const struct device *dev) {
    ARG_UNUSED(dev);
    
    if (!device_is_ready(motion_pin.port)) {
        LOG_ERR("GPIO device not ready");
        return -ENODEV;
    }

    int ret = gpio_pin_configure_dt(&motion_pin, GPIO_INPUT);
    if (ret != 0) return ret;

    ret = gpio_pin_interrupt_configure_dt(&motion_pin, GPIO_INT_EDGE_RISING);
    if (ret != 0) return ret;

    gpio_init_callback(&motion_cb, motion_isr, BIT(motion_pin.pin));
    gpio_add_callback(motion_pin.port, &motion_cb);
    
    LOG_INF("Motion interrupt initialized");
    return 0;
}

SYS_INIT(init_motion_interrupt, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);

/* Shell Command Implementation */
static int cmd_trigger_motion(const struct shell *sh, size_t argc, char **argv) {
    ARG_UNUSED(argc);
    ARG_UNUSED(argv);
    
    trigger_motion_isr_soft();
    shell_print(sh, "Software motion interrupt triggered");
    return 0;
}

/* Create shell command structure */
SHELL_CMD_ARG_REGISTER(trigger_motion, NULL, "Trigger motion ISR", cmd_trigger_motion, 1, 0);

/* Explicit shell registration */
static int register_shell_commands(const struct device *dev) {
    ARG_UNUSED(dev);
    LOG_DBG("Registering shell commands");
    return 0;
}

/* Ensure commands are registered after shell is ready */
SYS_INIT(register_shell_commands, POST_KERNEL, 99);