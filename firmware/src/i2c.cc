#include <hardware/gpio.h>
#include <hardware/i2c.h>

#include "i2c.h"

void our_i2c_init() {
    i2c_init(I2C_BLOCK, 400000);

    gpio_set_function(PIN_SDA, GPIO_FUNC_I2C);
    gpio_set_function(PIN_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(PIN_SDA);
    gpio_pull_up(PIN_SCL);
}
