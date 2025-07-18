#include <zephyr/init.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/logging/log.h>


/* GPIO interrupt */
LOG_MODULE_REGISTER(my_interrupt, LOG_LEVEL_DBG);

/* match node name in config? */
static const struct gpio_dt_spec my_pin = GPIO_DT_SPEC_GET(DT_NODELABEL(my_pin), gpios);

static void my_isr(const struct device *dev, struct gpio_callback *cb, uint32_t pins) {
    LOG_INF("motion interrupt triggered!");
}

static struct gpio_callback gpio_cb;

static int init_my_interrupt(const struct device *dev) {
    ARG_UNUSED(dev);
    
    if (!device_is_ready(my_pin.port)) {
        LOG_ERR("GPIO device not ready");
        return -ENODEV;
    }

    int ret = gpio_pin_configure_dt(&my_pin, GPIO_INPUT);
    if (ret != 0) {
        LOG_ERR("Failed to configure pin %d: %d", my_pin.pin, ret);
        return ret;
    }

    ret = gpio_pin_interrupt_configure_dt(&my_pin, GPIO_INT_EDGE_TO_ACTIVE);
    if (ret != 0) {
        LOG_ERR("Failed to configure interrupt: %d", ret);
        return ret;
    }

    gpio_init_callback(&gpio_cb, my_isr, BIT(my_pin.pin));
    gpio_add_callback(my_pin.port, &gpio_cb);
    
    LOG_INF("Interrupt handler initialized");
    return 0;
}

SYS_INIT(init_my_interrupt, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);