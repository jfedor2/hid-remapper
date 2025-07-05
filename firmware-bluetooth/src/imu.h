#pragma once

#include <zephyr/drivers/sensor.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/kernel.h>
#include "globals.h"

extern const struct gpio_dt_spec led0;
extern struct k_work_delayable activity_led_off_work;

bool imu_init(); 
void imu_recalibrate_orientation();
void imu_recalibrate_sensors(); 