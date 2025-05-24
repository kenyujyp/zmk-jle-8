// zmk-config/behaviors/motion_handler.c

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zmk/behavior.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/logging/log.h>

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

// Configuration structure for the behavior
struct motion_handler_config {
    const struct device *sensor; // GPIO device
    uint8_t pin; // GPIO pin number
};

// Work queue for deferred interrupt handling
static struct k_work motion_work;

// Work handler (runs after interrupt)
static void motion_handler_work(struct k_work *work) {
    LOG_DBG("Motion detected!");
    // Add custom logic (e.g., toggle layers, send keystrokes)
}

// Initialize the behavior
static int motion_handler_init(const struct device *dev) {
    const struct motion_handler_config *cfg = dev->config;

    // Configure GPIO pin
    gpio_pin_configure(cfg->sensor, cfg->pin, GPIO_INPUT | GPIO_PULL_UP);
    // Enable interrupt on falling edge (adjust as needed)
    gpio_pin_interrupt_configure(cfg->sensor, cfg->pin, GPIO_INT_EDGE_FALLING);

    // Initialize work queue
    k_work_init(&motion_work, motion_handler_work);

    return 0;
}

// Define the behavior
ZMK_BEHAVIOR_DEFINITION(motion_handler, MOTION_HANDLER, motion_handler_init, NULL);