
#ifndef SERVO_HPP
#define SERVO_HPP

#include "stdio.h"
#include "stm32f4xx_hal.h"
#include <cmath>
#include <algorithm>

constexpr uint16_t MIN_PULSE_WIDTH = 350;
constexpr uint16_t MAX_PULSE_WIDTH = 2600;
constexpr uint8_t PWM_DEADBAND = 5;

constexpr int16_t DEFAULT_MIN_ANGLE = 0;
constexpr int16_t DEFAULT_MAX_ANGLE = 180;
float remapf(float x, float in_min, float in_max, float out_min, float out_max);

class Servo {
public:
    Servo() noexcept = default;

    Servo(TIM_HandleTypeDef* timer,
          uint32_t channel,
          uint16_t min_pwm,
          uint16_t max_pwm);

    void init(TIM_HandleTypeDef* timer,
              uint32_t channel,
              uint16_t min_pwm,
              uint16_t max_pwm);

    // calibration
    void set_new_map(int16_t min_map, int16_t max_map);
    void set_angle_offset(int16_t offset);
    void set_adc_map(uint16_t min_adc, uint16_t max_adc) {}; // not implemented yet

    void turn_off();
    void turn_on();

    // movement
    void set_pwm(uint16_t pwm);
    void set_angle(float angle);

    // feedback info
    uint16_t get_curr_pwm() const noexcept
    {
        return curr_pwm_;
    }
    float get_curr_angle() const noexcept
    {
        return curr_angle_;
    }

    // returns reach of servo in degrees
    int16_t get_offset_angle() const noexcept
    {
        return offset_angle_map_;
    }
    int16_t get_min_angle_map() const noexcept
    {
        return std::min(min_angle_map_, max_angle_map_) - offset_angle_map_;
    }
    int16_t get_max_angle_map() const noexcept
    {
        return std::max(min_angle_map_, max_angle_map_) - offset_angle_map_;
    }

    void print_config() const noexcept;
    char uart_config() const noexcept
    {
        char buffer[100];
        /*
        snprintf(buffer, sizeof(buffer), "CH:%lu, PWM:[%u-%u], CURR_PWM:%u, CURR_ANGLE:%.2f, ANGLE_MAP:[%d-%d], OFFSET:%d\r\n",
                 channel_, min_pwm_, max_pwm_, curr_pwm_, curr_angle_, min_angle_map_, max_angle_map_, offset_angle_map_);
        */
        return *buffer; 
    }
private:
    TIM_HandleTypeDef* timer_ = nullptr;
    uint32_t channel_ = 0;
    bool is_initialized_ = false;

    uint16_t curr_pwm_ = 0;
    uint16_t prev_pwm_ = 0;
    uint16_t min_pwm_ = MIN_PULSE_WIDTH;
    uint16_t max_pwm_ = MAX_PULSE_WIDTH;

    float curr_angle_ = 0.0;
    int16_t offset_angle_map_ = 0;
    int16_t min_angle_map_ = DEFAULT_MIN_ANGLE;
    int16_t max_angle_map_ = DEFAULT_MAX_ANGLE;
};

#endif /* SERVO_HPP*/