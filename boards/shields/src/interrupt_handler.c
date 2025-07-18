
/* GPIO interrupt */


#include <zephyr/drivers/gpio.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(motion_int, LOG_LEVEL_DBG);

/* match node name in config? */
static const struct gpio_dt_spec motion_pin = GPIO_DT_SPEC_GET(DT_NODELABEL(my_pin), gpios);

static void motion_isr(const struct device *dev, struct gpio_callback *cb, uint32_t pins) {
    LOG_INF("Motion detected!");
}

static struct gpio_callback motion_cb;

static int init_motion_interrupt(const struct device *dev) {
    ARG_UNUSED(dev);
    
    if (!device_is_ready(motion_pin.port)) {
        LOG_ERR("GPIO device not ready");
        return -ENODEV;
    }

    // Configure pin and interrupt
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