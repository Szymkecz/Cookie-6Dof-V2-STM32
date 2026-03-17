#include "app.hpp"
#include "main.h"
#include "usart.h" 
#include <cstdio>
#include "servo_manager.hpp"
#include <cstring>

//TEMP
#include "tim.h"

extern DMA_HandleTypeDef hdma_usart2_rx;
uint8_t rx_buffer[128];
bool is_streaming = false;

extern "C" int _write(int file, char *ptr, int len) {
    // Wysyła ciąg znaków przez UART2
    HAL_UART_Transmit(&huart2, (uint8_t*)ptr, (uint16_t)len, HAL_MAX_DELAY);
    return len;
}

extern "C" void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size) {
    if (huart->Instance == USART2) {
        
        // Zabezpieczenie na końcu odebranego ciągu znaków
        rx_buffer[Size] = '\0'; 

        // 1. Sprawdzanie komend sterujących strumieniem
        if (strncmp((char*)rx_buffer, "START_STREAM;", 13) == 0) {
            is_streaming = true;
            printf("ACK_STREAM\r\n"); // Python na to czeka!
        } 
        else if (strncmp((char*)rx_buffer, "STOP_STREAM;", 12) == 0) {
            is_streaming = false;
            printf("Zatrzymano strumien.\r\n");
        }
        
        // 2. Parsowanie ramek z pozycjami (format: JN[ID, J1, J2, J3, J4, J5, J6];)
        else if (is_streaming && rx_buffer[0] == 'J' && rx_buffer[1] == 'N') {
            int frame_id = 0;
            float j[6] = {0.0f}; 
            
            // %d to ID ramki, %f to kolejne kąty
            int parsed = sscanf((char*)rx_buffer, "JN[%d,%f,%f,%f,%f,%f,%f]", 
                                &frame_id, &j[0], &j[1], &j[2], &j[3], &j[4], &j[5]);
            
            if (parsed == 7) {
                // SUKCES: Odsyłamy Pythonowi potwierdzenie dla tej konkretnej ramki
                printf("FB[%d,OK]\r\n", frame_id);
                
                // --- TUTAJ MOŻESZ PRZEKAZAĆ KĄTY DO SERWO MANAGERA ---
                // Twój ServoManager ma std::array<double, 7>, więc możemy to przepisać:
                /*
                std::array<double, 7> new_angles = {j[0], j[1], j[2], j[3], j[4], j[5], 0.0};
                
                */
            } else {
                // BŁĄD: Ramka była uszkodzona
                printf("FB[%d,ERR_PARSE]\r\n", frame_id);
            }
        }

        // 3. Krytyczne: Ponowne uzbrojenie DMA na kolejną paczkę danych
        HAL_UARTEx_ReceiveToIdle_DMA(&huart2, rx_buffer, sizeof(rx_buffer));
        __HAL_DMA_DISABLE_IT(&hdma_usart2_rx, DMA_IT_HT); // Wyłączenie przerwania w połowie
    }
}


int app() {
    
    // Włączenie zasilania serw (Przekaźnik)
    HAL_GPIO_WritePin(RELAY_GPIO_Port, RELAY_Pin, GPIO_PIN_SET);
    
    printf("Zaczynamy! STM32 gotowe do pracy w C++\r\n");
    printf("Test floatow: %.2f\r\n", 3.14);

    ServoManager::init();

    // 1. Uruchomienie generowania sygnału PWM na kanale 1 Timera 1
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);

    // Inicjalizacja serw na pozycje startowe
    std::array<double, 7> angles = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
    std::array<uint16_t, 7> pwm_values = {1500, 1500, 1500, 1500, 1500, 1500, 1500};
    ServoManager::set_pwm(pwm_values);
     
    // NOWE: Uruchomienie nasłuchiwania UART przez DMA przed wejściem w pętlę!
    HAL_UARTEx_ReceiveToIdle_DMA(&huart2, rx_buffer, sizeof(rx_buffer));
    __HAL_DMA_DISABLE_IT(&hdma_usart2_rx, DMA_IT_HT);
     
    // Główna pętla - procesor jest całkowicie wolny i odciążony
    while (true) {
        
        // Tutaj będą tylko rzeczy nieblokujące, np. sprawdzanie czujników, INA3221 itp.
        
        /* if (htim3_has_passed){
            ServoManager::set_angles(new_angles); 
        }*/
        // HAL_Delay(20);
    }
}