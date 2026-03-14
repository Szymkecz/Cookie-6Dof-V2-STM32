#include "app.hpp"
#include "main.h" 


// Główna funkcja aplikacji
void app() {
    // Inicjalizacja Twoich obiektów (np. Serwa, Kinematyki)
    // ...

    // Główna pętla programu (zastępuje while(1) z main.c)
    while (true) {
        // Tutaj czytasz UART, liczysz kąty, machasz serwem
        HAL_GPIO_WritePin(LD2_GPIO_Port,LD2_Pin, GPIO_PIN_SET);
        HAL_Delay(500);
        HAL_GPIO_WritePin(LD2_GPIO_Port,LD2_Pin, GPIO_PIN_RESET);
        HAL_Delay(500);
        // HAL_Delay(10); // Małe opóźnienie, żeby nie zablokować mikrokontrolera
    }
}