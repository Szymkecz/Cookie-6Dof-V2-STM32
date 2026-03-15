#include "app.hpp"
#include "main.h"
#include "usart.h" 
#include <cstdio>
#include "servo_manager.hpp"

//TEMP
#include "tim.h"

extern "C" int _write(int file, char *ptr, int len) {
    // Wysyła ciąg znaków przez UART2
    HAL_UART_Transmit(&huart2, (uint8_t*)ptr, (uint16_t)len, HAL_MAX_DELAY);
    return len;
}


int app() {
    
    HAL_GPIO_WritePin(RELAY_GPIO_Port,RELAY_Pin,GPIO_PIN_SET);
    
    printf("Zaczynamy! STM32 gotowe do pracy w C++\r\n");
    printf("%.2f\r\n", 3.14);

     ServoManager::init();

    // 1. Uruchomienie generowania sygnału PWM na kanale 1 Timera 1
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);

    // 2. Ustawienie szerokości impulsu na 1500us (pozycja środkowa, 90 stopni)
    // __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, 1500);

     std::array<double, 7> angles = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
    std::array<uint16_t, 7> pwm_values = {1500, 1500, 1500, 1500, 1500, 1500, 1500};
    ServoManager::set_pwm(pwm_values);
     
     
     while (true) {
         

        // HAL_GPIO_WritePin(LD2_GPIO_Port,LD2_Pin, GPIO_PIN_SET);
        // HAL_Delay(500);
        // HAL_GPIO_WritePin(LD2_GPIO_Port,LD2_Pin, GPIO_PIN_RESET);
        // HAL_Delay(500);
        HAL_Delay(20);
    
    }
}