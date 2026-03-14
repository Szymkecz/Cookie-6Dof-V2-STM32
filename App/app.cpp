#include "app.hpp"
#include "main.h"
#include "usart.h" 
#include <cstdio>


extern "C" int _write(int file, char *ptr, int len) {
    // Wysyła ciąg znaków przez UART2
    HAL_UART_Transmit(&huart2, (uint8_t*)ptr, (uint16_t)len, HAL_MAX_DELAY);
    return len;
}

int app() {

    
    printf("Zaczynamy! STM32 gotowe do pracy w C++\r\n");
    
    while (true) {
    
        HAL_GPIO_WritePin(LD2_GPIO_Port,LD2_Pin, GPIO_PIN_SET);
        HAL_Delay(500);
        HAL_GPIO_WritePin(LD2_GPIO_Port,LD2_Pin, GPIO_PIN_RESET);
        HAL_Delay(500);
    
    }
}