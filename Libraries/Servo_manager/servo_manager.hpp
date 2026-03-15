#ifndef SERVO_MANAGER_HPP
#define SERVO_MANAGER_HPP

#include "servo.hpp"
#include <cstdio>
#include <array>
#include <cinttypes>

namespace ServoManager {

    constexpr uint8_t SERVO_COUNT = 7;
    extern Servo SrvArray[SERVO_COUNT];

    void init();
    void set_pwm(std::array<uint16_t, SERVO_COUNT>& pwm_values);
    void set_angles(std::array<double, SERVO_COUNT>& angles);

    std::array<uint16_t, SERVO_COUNT> get_curr_pwm();
    std::array<double, SERVO_COUNT> get_curr_angles();

    // debugging
    void print_curr_pwm();
    void print_angles();
    void print_servo_config();

} // namespace ServoManager

#endif // SERVO_MANAGER_HPP