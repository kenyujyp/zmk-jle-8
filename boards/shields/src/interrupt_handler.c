
/* GPIO interrupt & soft interrupt */

#include <zephyr/init.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/logging/log.h>
#include <zephyr/shell/shell.h>  // Shell support
#include <zephyr/sys/util.h>

LOG_MODULE_REGISTER(motion_int, LOG_LEVEL_DBG);

static const struct gpio_dt_spec motion_pin = GPIO_DT_SPEC_GET(DT_NODELABEL(my_pin), gpios);
static struct gpio_callback motion_cb;

// Function to trigger the ISR manually
void trigger_motion_isr_soft(void) {
    LOG_DBG("Simulating motion interrupt");
    
    // Create fake parameters that match ISR signature
    const struct device *fake_dev = motion_pin.port;
    uint32_t fake_pins = BIT(motion_pin.pin);
    
    // Directly call the ISR with simulated parameters
    motion_isr(fake_dev, &motion_cb, fake_pins);
}

static void motion_isr(const struct device *dev, struct gpio_callback *cb, uint32_t pins) {
    if (pins & BIT(motion_pin.pin)) {
        LOG_INF("Motion detected! (source: %s)", 
                k_is_in_isr() ? "HARDWARE" : "SOFTWARE");
    }
}

static int init_motion_interrupt(const struct device *dev) {
    ARG_UNUSED(dev);
    
    if (!device_is_ready(motion_pin.port)) {
        LOG_ERR("GPIO device not ready");
        return -ENODEV;
    }

    int ret = gpio_pin_configure_dt(&motion_pin, GPIO_INPUT);
    if (ret != 0) {
        LOG_ERR("Pin config failed: %d", ret);
        return ret;
    }

    ret = gpio_pin_interrupt_configure_dt(&motion_pin, GPIO_INT_EDGE_RISING);
    if (ret != 0) {
        LOG_ERR("Interrupt config failed: %d", ret);
        return ret;
    }

    gpio_init_callback(&motion_cb, motion_isr, BIT(motion_pin.pin));
    gpio_add_callback(motion_pin.port, &motion_cb);
    
    LOG_INF("Motion interrupt initialized");
    return 0;
}

SYS_INIT(init_motion_interrupt, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);

// Shell command handler
static int cmd_trigger_motion(const struct shell *shell, size_t argc, char **argv)
{
    ARG_UNUSED(argc);
    ARG_UNUSED(argv);
    
    trigger_motion_isr_soft();
    shell_print(shell, "Motion ISR triggered via console");
    return 0;
}

/* register shell command */
SHELL_CMD_REGISTER(trigger_motion, NULL, "Trigger motion interrupt", cmd_trigger_motion);