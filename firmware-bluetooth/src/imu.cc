#include "imu.h"
#include "imu_descriptor.h"
#include "config.h"
#include "descriptor_parser.h"
#include "globals.h"
#include "platform.h"
#include "remapper.h"

#include <zephyr/drivers/gpio.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <math.h>

#if DT_NODE_EXISTS(DT_NODELABEL(lsm6ds3tr_c))

#define IMU_VIRTUAL_INTERFACE 0x1000
#define CALIBRATION_SAMPLES 200

#define IMU_SAMPLE_RATE_MS 15  
#define MAX_ERROR_COUNT_BEFORE_BACKOFF 5
#define ERROR_BACKOFF_MULTIPLIER 4
#define CALIBRATION_RETRY_DELAY_MS 500

#define PI 3.14159265359f
#define DEG_TO_RAD (PI / 180.0f)
#define RAD_TO_DEG (180.0f / PI)
#define GRAVITY 9.81f

#define LED_ACTIVITY_DURATION_MS 50

#define MIN_DT_SECONDS 0.005f
#define MAX_DT_SECONDS 0.050f
#define EXPECTED_DT_SECONDS 0.015f
#define CALIBRATION_SAMPLE_DELAY_MS 5

#define IMU_ODR_FREQUENCY 52
#define ACCEL_SCALE_RANGE 2
#define GYRO_SCALE_RANGE 125

#define MAX_FILTER_BUFFER_SIZE 16

typedef struct {
    float beta_base;
    float beta_min;
    float beta_max;
    float stationary_threshold;
    float accel_trust_threshold_high;
    float accel_trust_threshold_low;
    float bias_update_rate;
    float gyro_deadzone;
    float angle_clamp_limit;
    float magnitude_filter_alpha;
} imu_config_t;

static imu_config_t imu_config = {
    .beta_base = 0.1f,
    .beta_min = 0.01f,
    .beta_max = 0.3f,
    .stationary_threshold = 0.01f,
    .accel_trust_threshold_high = 2.0f,
    .accel_trust_threshold_low = 0.5f,
    .bias_update_rate = 0.001f,
    .gyro_deadzone = 0.001f,
    .angle_clamp_limit = 45.0f,
    .magnitude_filter_alpha = 0.9f
};

typedef struct {
    float y, alpha;
} iir_t;

typedef struct {
    float buffer[MAX_FILTER_BUFFER_SIZE];
    int index;
    int count;
    int size;
    bool initialized;
} moving_avg_filter_t;

static volatile float madgwick_q0 = 1.0f;
static volatile float madgwick_q1 = 0.0f;
static volatile float madgwick_q2 = 0.0f;
static volatile float madgwick_q3 = 0.0f;

static const struct device* imu_dev;
static void imu_work_fn(struct k_work* work);
static K_WORK_DELAYABLE_DEFINE(imu_work, imu_work_fn);

static volatile float pitch_offset = 0.0f;
static volatile float roll_offset = 0.0f;
static int64_t last_timestamp = 0;

static volatile float gyro_bias_x = 0.0f;
static volatile float gyro_bias_y = 0.0f;
static volatile float gyro_bias_z = 0.0f;
static float accel_bias_x = 0.0f;
static float accel_bias_y = 0.0f;
static float accel_bias_z = 0.0f;
static bool is_calibrated = false;

static iir_t magnitude_filter = {.y = 9.81f, .alpha = 0.9f};
static uint32_t error_count = 0;

static uint8_t last_known_angle_clamp_limit = 90;

static moving_avg_filter_t pitch_filter = {
    .index = 0,
    .count = 0,
    .initialized = false
};

static moving_avg_filter_t roll_filter = {
    .index = 0,
    .count = 0,
    .initialized = false
};

extern const struct gpio_dt_spec led0;
extern struct k_work_delayable activity_led_off_work;

static float inv_sqrt(float x) {
    float halfx = 0.5f * x;
    float y = x;
    long i = *(long*)&y;
    i = 0x5f3759df - (i >> 1);
    y = *(float*)&i;
    y = y * (1.5f - (halfx * y * y));
    return y;
}

static float compute_dynamic_beta(float hp_magnitude) {
    if (hp_magnitude < imu_config.accel_trust_threshold_low) {
        return imu_config.beta_max;
    } else if (hp_magnitude > imu_config.accel_trust_threshold_high) {
        return imu_config.beta_min;
    } else {
        float ratio = (hp_magnitude - imu_config.accel_trust_threshold_low) / 
                     (imu_config.accel_trust_threshold_high - imu_config.accel_trust_threshold_low);
        return imu_config.beta_max - ratio * (imu_config.beta_max - imu_config.beta_min);
    }
}

static void madgwick_update_imu(float gx, float gy, float gz, float ax, float ay, float az, float dt, float beta) {
    float recipNorm;
    float s0, s1, s2, s3;
    float qDot1, qDot2, qDot3, qDot4;
    float _2q0, _2q1, _2q2, _2q3, _4q0, _4q1, _4q2, _8q1, _8q2, q0q0, q1q1, q2q2, q3q3;

    qDot1 = 0.5f * (-madgwick_q1 * gx - madgwick_q2 * gy - madgwick_q3 * gz);
    qDot2 = 0.5f * (madgwick_q0 * gx + madgwick_q2 * gz - madgwick_q3 * gy);
    qDot3 = 0.5f * (madgwick_q0 * gy - madgwick_q1 * gz + madgwick_q3 * gx);
    qDot4 = 0.5f * (madgwick_q0 * gz + madgwick_q1 * gy - madgwick_q2 * gx);

    if (!((ax == 0.0f) && (ay == 0.0f) && (az == 0.0f))) {

        recipNorm = inv_sqrt(ax * ax + ay * ay + az * az);
        ax *= recipNorm;
        ay *= recipNorm;
        az *= recipNorm;

        _2q0 = 2.0f * madgwick_q0;
        _2q1 = 2.0f * madgwick_q1;
        _2q2 = 2.0f * madgwick_q2;
        _2q3 = 2.0f * madgwick_q3;
        _4q0 = 4.0f * madgwick_q0;
        _4q1 = 4.0f * madgwick_q1;
        _4q2 = 4.0f * madgwick_q2;
        _8q1 = 8.0f * madgwick_q1;
        _8q2 = 8.0f * madgwick_q2;
        q0q0 = madgwick_q0 * madgwick_q0;
        q1q1 = madgwick_q1 * madgwick_q1;
        q2q2 = madgwick_q2 * madgwick_q2;
        q3q3 = madgwick_q3 * madgwick_q3;

        s0 = _4q0 * q2q2 + _2q2 * ax + _4q0 * q1q1 - _2q1 * ay;
        s1 = _4q1 * q3q3 - _2q3 * ax + 4.0f * q0q0 * madgwick_q1 - _2q0 * ay - _4q1 + _8q1 * q1q1 + _8q1 * q2q2 + _4q1 * az;
        s2 = 4.0f * q0q0 * madgwick_q2 + _2q0 * ax + _4q2 * q3q3 - _2q3 * ay - _4q2 + _8q2 * q1q1 + _8q2 * q2q2 + _4q2 * az;
        s3 = 4.0f * q1q1 * madgwick_q3 - _2q1 * ax + 4.0f * q2q2 * madgwick_q3 - _2q2 * ay;
        recipNorm = inv_sqrt(s0 * s0 + s1 * s1 + s2 * s2 + s3 * s3);
        s0 *= recipNorm;
        s1 *= recipNorm;
        s2 *= recipNorm;
        s3 *= recipNorm;

        qDot1 -= beta * s0;
        qDot2 -= beta * s1;
        qDot3 -= beta * s2;
        qDot4 -= beta * s3;
    }

    madgwick_q0 += qDot1 * dt;
    madgwick_q1 += qDot2 * dt;
    madgwick_q2 += qDot3 * dt;
    madgwick_q3 += qDot4 * dt;

    recipNorm = inv_sqrt(madgwick_q0 * madgwick_q0 + madgwick_q1 * madgwick_q1 + madgwick_q2 * madgwick_q2 + madgwick_q3 * madgwick_q3);
    madgwick_q0 *= recipNorm;
    madgwick_q1 *= recipNorm;
    madgwick_q2 *= recipNorm;
    madgwick_q3 *= recipNorm;
}

static float madgwick_get_pitch(void) {
    return asinf(-2.0f * (madgwick_q1 * madgwick_q3 - madgwick_q0 * madgwick_q2)) * RAD_TO_DEG;
}

static float madgwick_get_roll(void) {
    return atan2f(2.0f * (madgwick_q0 * madgwick_q1 + madgwick_q2 * madgwick_q3), 
                  1.0f - 2.0f * (madgwick_q1 * madgwick_q1 + madgwick_q2 * madgwick_q2)) * RAD_TO_DEG;
}

static bool read_imu_raw(float *ax, float *ay, float *az, float *gx, float *gy, float *gz) {
    struct sensor_value accel[3], gyro[3];
    
    if (sensor_sample_fetch(imu_dev) < 0) return false;
    
    if (sensor_channel_get(imu_dev, SENSOR_CHAN_ACCEL_X, &accel[0]) < 0 ||
        sensor_channel_get(imu_dev, SENSOR_CHAN_ACCEL_Y, &accel[1]) < 0 ||
        sensor_channel_get(imu_dev, SENSOR_CHAN_ACCEL_Z, &accel[2]) < 0) {
        return false;
    }
    
    if (sensor_channel_get(imu_dev, SENSOR_CHAN_GYRO_X, &gyro[0]) < 0 ||
        sensor_channel_get(imu_dev, SENSOR_CHAN_GYRO_Y, &gyro[1]) < 0 ||
        sensor_channel_get(imu_dev, SENSOR_CHAN_GYRO_Z, &gyro[2]) < 0) {
        return false;
    }
    
    *ax = (float)sensor_value_to_double(&accel[0]);
    *ay = (float)sensor_value_to_double(&accel[1]);
    *az = (float)sensor_value_to_double(&accel[2]);
    *gx = (float)sensor_value_to_double(&gyro[0]);
    *gy = (float)sensor_value_to_double(&gyro[1]);
    *gz = (float)sensor_value_to_double(&gyro[2]);
    
    return true;
}

static int16_t scale_angle_to_int16(float angle, float min_angle, float max_angle) {
    angle = fmaxf(min_angle, fminf(max_angle, angle));
    float normalized = (angle - min_angle) / (max_angle - min_angle);

    int scaled = (int)((normalized - 0.5f) * 65535.0f);
    return (int16_t)fmaxf(-32768.0f, fminf(32767.0f, (float)scaled));
}

static uint16_t scale_magnitude_to_uint16(float magnitude, float max_magnitude) {
    magnitude = fmaxf(0.0f, fminf(max_magnitude, magnitude));
    float normalized = magnitude / max_magnitude;
    int scaled = (int)(normalized * 255.0f);
    return (uint16_t)fmaxf(0.0f, fminf(255.0f, (float)scaled));
}

static void clamp_angle_to_limit(float* angle) {
    float current_clamp_limit = (float)imu_angle_clamp_limit;
    *angle = fmaxf(-current_clamp_limit, fminf(current_clamp_limit, *angle));
}

static float apply_deadzone(float value, float deadzone) {
    if (fabsf(value) < deadzone) {
        return 0.0f;
    }
    return value > 0 ? value - deadzone : value + deadzone;
}

static void calibrate_orientation(float pitch, float roll) {
    pitch_offset = pitch;
    roll_offset = roll;
}

static bool calibrate_sensors(void) {
    float sum_accel_x = 0.0f, sum_accel_y = 0.0f, sum_accel_z = 0.0f;
    float sum_gyro_x = 0.0f, sum_gyro_y = 0.0f, sum_gyro_z = 0.0f;
    
    for (int i = 0; i < CALIBRATION_SAMPLES; i++) {
        float ax, ay, az, gx, gy, gz;
        if (!read_imu_raw(&ax, &ay, &az, &gx, &gy, &gz)) {
            return false;
        }
        
        sum_accel_x += ax;
        sum_accel_y += ay;
        sum_accel_z += az;
        sum_gyro_x += gx;
        sum_gyro_y += gy;
        sum_gyro_z += gz;
        
        k_msleep(CALIBRATION_SAMPLE_DELAY_MS);
    }
    
    accel_bias_x = sum_accel_x / CALIBRATION_SAMPLES;
    accel_bias_y = sum_accel_y / CALIBRATION_SAMPLES;
    accel_bias_z = sum_accel_z / CALIBRATION_SAMPLES - GRAVITY;
    
    gyro_bias_x = sum_gyro_x / CALIBRATION_SAMPLES;
    gyro_bias_y = sum_gyro_y / CALIBRATION_SAMPLES;
    gyro_bias_z = sum_gyro_z / CALIBRATION_SAMPLES;
    
    return true;
}

static float iir_update_magnitude(iir_t *filter, float input) {
    filter->y = filter->alpha * filter->y + (1.0f - filter->alpha) * input;
    return filter->y;
}

static float moving_avg_filter_update(moving_avg_filter_t *filter, float input) {
    int bufsize = filter->size;
    if (bufsize < 1) bufsize = 1;
    if (bufsize > MAX_FILTER_BUFFER_SIZE) bufsize = MAX_FILTER_BUFFER_SIZE;
    filter->buffer[filter->index] = input;
    filter->index = (filter->index + 1) % bufsize;
    
    if (!filter->initialized) {
        filter->initialized = true;
        filter->count = 1;
        return input;
    }
    
    if (filter->count < bufsize) {
        filter->count++;
    }
    
    float sum = 0.0f;
    for (int i = 0; i < filter->count; i++) {
        sum += filter->buffer[i];
    }
    
    return sum / filter->count;
}

static void update_gyro_bias_if_stationary(float gx_raw, float gy_raw, float gz_raw, float hp_magnitude) {
    if (hp_magnitude < imu_config.stationary_threshold) {
        gyro_bias_x += imu_config.bias_update_rate * (gx_raw - gyro_bias_x);
        gyro_bias_y += imu_config.bias_update_rate * (gy_raw - gyro_bias_y);
        gyro_bias_z += imu_config.bias_update_rate * (gz_raw - gyro_bias_z);
    }
}

static void imu_work_fn(struct k_work* work) {
    if (!imu_dev) {
        error_count++;
        if (error_count > MAX_ERROR_COUNT_BEFORE_BACKOFF) {
    
            k_work_reschedule(&imu_work, K_MSEC(IMU_SAMPLE_RATE_MS * ERROR_BACKOFF_MULTIPLIER));
            error_count = 0;
        } else {
            k_work_reschedule(&imu_work, K_MSEC(IMU_SAMPLE_RATE_MS));
        }
        return;
    }

    int64_t now = k_uptime_get();
    float dt = last_timestamp ? (now - last_timestamp) / 1000.0f : EXPECTED_DT_SECONDS;
    last_timestamp = now;


    if (dt < MIN_DT_SECONDS || dt > MAX_DT_SECONDS) {
        dt = EXPECTED_DT_SECONDS;
    }

    if (!is_calibrated) {
        if (calibrate_sensors()) {
            is_calibrated = true;
            magnitude_filter.alpha = imu_config.magnitude_filter_alpha;
            
            float pitch = madgwick_get_pitch();
            float roll = madgwick_get_roll();
            calibrate_orientation(pitch, roll);
        } else {
            error_count++;

            k_work_reschedule(&imu_work, K_MSEC(CALIBRATION_RETRY_DELAY_MS));
            return;
        }
    }

    float ax_raw, ay_raw, az_raw, gx_raw, gy_raw, gz_raw;
    if (!read_imu_raw(&ax_raw, &ay_raw, &az_raw, &gx_raw, &gy_raw, &gz_raw)) {
        error_count++;
        gpio_pin_set_dt(&led0, false);
        

        uint32_t delay_ms = (error_count > MAX_ERROR_COUNT_BEFORE_BACKOFF) ? 
                           IMU_SAMPLE_RATE_MS * ERROR_BACKOFF_MULTIPLIER : 
                           IMU_SAMPLE_RATE_MS;
        k_work_reschedule(&imu_work, K_MSEC(delay_ms));
        return;
    }
    
    error_count = 0;
    
    if (last_known_angle_clamp_limit != imu_angle_clamp_limit) {
        last_known_angle_clamp_limit = imu_angle_clamp_limit;
        imu_config.angle_clamp_limit = (float)imu_angle_clamp_limit;
        
        int adaptive_buffer_size = imu_filter_buffer_size;
        
        pitch_filter.size = adaptive_buffer_size;
        roll_filter.size = adaptive_buffer_size;
        pitch_filter.initialized = false;
        roll_filter.initialized = false;
        pitch_filter.count = 0;
        roll_filter.count = 0;
        pitch_filter.index = 0;
        roll_filter.index = 0;
    }
    
    float ax = ax_raw - accel_bias_x;
    float ay = ay_raw - accel_bias_y;
    float az = az_raw - accel_bias_z;
    float gx = gx_raw - gyro_bias_x;
    float gy = gy_raw - gyro_bias_y;
    float gz = gz_raw - gyro_bias_z;
    
    gx = apply_deadzone(gx, imu_config.gyro_deadzone);
    gy = apply_deadzone(gy, imu_config.gyro_deadzone);
    gz = apply_deadzone(gz, imu_config.gyro_deadzone);
    
    float accel_mag = sqrtf(ax * ax + ay * ay + az * az);
    float filtered_mag = iir_update_magnitude(&magnitude_filter, accel_mag);
    float hp_magnitude = accel_mag - filtered_mag;
    
    update_gyro_bias_if_stationary(gx_raw, gy_raw, gz_raw, fabsf(hp_magnitude));
    
    float beta = compute_dynamic_beta(fabsf(hp_magnitude));
    
    madgwick_update_imu(gx, gy, gz, ax, ay, az, dt, beta);
    
    float pitch = madgwick_get_pitch();
    float roll = madgwick_get_roll();
    
    float pitch_corrected = -(pitch - pitch_offset);
    float roll_corrected = roll - roll_offset;
    
    if (imu_pitch_inverted) {
        pitch_corrected = -pitch_corrected;
    }
    if (imu_roll_inverted) {
        roll_corrected = -roll_corrected;
    }
    
    pitch_corrected = moving_avg_filter_update(&pitch_filter, pitch_corrected);
    roll_corrected = moving_avg_filter_update(&roll_filter, roll_corrected);
    
    clamp_angle_to_limit(&pitch_corrected);
    clamp_angle_to_limit(&roll_corrected);
    
    float current_clamp_limit = (float)imu_angle_clamp_limit;
    int16_t pitch_scaled = scale_angle_to_int16(pitch_corrected, -current_clamp_limit, current_clamp_limit);
    int16_t roll_scaled = scale_angle_to_int16(roll_corrected, -current_clamp_limit, current_clamp_limit);
    uint16_t magnitude_scaled = scale_magnitude_to_uint16(hp_magnitude, 25.0f);
    
    imu_report_t imu_report = { 
        .pitch = pitch_scaled,
        .roll = roll_scaled,
        .magnitude = magnitude_scaled
    };
    
    handle_received_report((uint8_t*)&imu_report, (int)sizeof(imu_report), IMU_VIRTUAL_INTERFACE);

    k_work_cancel_delayable(&activity_led_off_work);
    gpio_pin_set_dt(&led0, true);
    k_work_reschedule(&activity_led_off_work, K_MSEC(LED_ACTIVITY_DURATION_MS));


    k_work_reschedule(&imu_work, K_MSEC(IMU_SAMPLE_RATE_MS));
}

bool imu_init() {
    imu_dev = DEVICE_DT_GET(DT_NODELABEL(lsm6ds3tr_c));
    
    if (!device_is_ready(imu_dev)) {
        return false;
    }
    
    imu_config.angle_clamp_limit = (float)imu_angle_clamp_limit;
    
    int adaptive_buffer_size = imu_filter_buffer_size;
    
    pitch_filter.size = adaptive_buffer_size;
    roll_filter.size = adaptive_buffer_size;
    pitch_filter.initialized = false;
    roll_filter.initialized = false;
    pitch_filter.count = 0;
    roll_filter.count = 0;
    pitch_filter.index = 0;
    roll_filter.index = 0;

    struct sensor_value odr_attr;
    odr_attr.val1 = IMU_ODR_FREQUENCY;
    odr_attr.val2 = 0;

    if (sensor_attr_set(imu_dev, SENSOR_CHAN_ACCEL_XYZ,
                        SENSOR_ATTR_SAMPLING_FREQUENCY, &odr_attr) < 0) {
        return false;
    }

    if (sensor_attr_set(imu_dev, SENSOR_CHAN_GYRO_XYZ,
                        SENSOR_ATTR_SAMPLING_FREQUENCY, &odr_attr) < 0) {
        return false;
    }

    struct sensor_value accel_scale_attr;
    accel_scale_attr.val1 = ACCEL_SCALE_RANGE;
    accel_scale_attr.val2 = 0;

    sensor_attr_set(imu_dev, SENSOR_CHAN_ACCEL_XYZ,
                     SENSOR_ATTR_FULL_SCALE, &accel_scale_attr);

    struct sensor_value angular_scale_attr;
    angular_scale_attr.val1 = GYRO_SCALE_RANGE;
    angular_scale_attr.val2 = 0;

    sensor_attr_set(imu_dev, SENSOR_CHAN_GYRO_XYZ,
                     SENSOR_ATTR_FULL_SCALE, &angular_scale_attr);
    

    float ax, ay, az, gx, gy, gz;
    if (!read_imu_raw(&ax, &ay, &az, &gx, &gy, &gz)) {
        return false;
    }

    parse_descriptor(0x0F0D, 0x00C1, imu_hid_report_desc, IMU_HID_REPORT_DESC_SIZE, IMU_VIRTUAL_INTERFACE, 0);
    device_connected_callback(IMU_VIRTUAL_INTERFACE, 0x0F0D, 0x00C1, 0);
    
    their_descriptor_updated = true;


    k_work_schedule(&imu_work, K_MSEC(IMU_SAMPLE_RATE_MS));

    return true;
}

void imu_recalibrate_orientation() {
    if (is_calibrated) {
        float pitch = madgwick_get_pitch();
        float roll = madgwick_get_roll();
        calibrate_orientation(pitch, roll);
        
        int adaptive_buffer_size = imu_filter_buffer_size;
        
        pitch_filter.size = adaptive_buffer_size;
        roll_filter.size = adaptive_buffer_size;
        pitch_filter.initialized = false;
        roll_filter.initialized = false;
        pitch_filter.count = 0;
        roll_filter.count = 0;
        pitch_filter.index = 0;
        roll_filter.index = 0;
        

    }
}

void imu_recalibrate_sensors() {
    if (is_calibrated) {
        is_calibrated = false;
        error_count = 0;
        
        madgwick_q0 = 1.0f;
        madgwick_q1 = 0.0f;
        madgwick_q2 = 0.0f;
        madgwick_q3 = 0.0f;
        
        magnitude_filter = (iir_t){.y = 9.81f, .alpha = imu_config.magnitude_filter_alpha};
        
        int adaptive_buffer_size = imu_filter_buffer_size;
        
        pitch_filter.size = adaptive_buffer_size;
        roll_filter.size = adaptive_buffer_size;
        pitch_filter.initialized = false;
        roll_filter.initialized = false;
        pitch_filter.count = 0;
        roll_filter.count = 0;
        pitch_filter.index = 0;
        roll_filter.index = 0;
        

    }
}

#else

bool imu_init() {
    return true;
}

#endif 