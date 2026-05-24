#include "app.hpp"
#include "main.h"
#include "usart.h"
#include <cstdio>
#include "servo_manager.hpp"
#include <cstring>

// TEMP
#include "tim.h"
#include "gpio.h"

extern DMA_HandleTypeDef hdma_usart2_rx;
uint8_t rx_buffer[128];
bool is_streaming = false;
std::array<double, 7> angles = {0.0, 90.0, 0.0, 0.0, 0.0, 0.0, 0.0};

constexpr char CMD_START_STREAM[] = "START_STREAM;";
constexpr char CMD_STOP_STREAM[] = "STOP_STREAM;";
constexpr char CMD_GET_CONFIG[] = "GET_CONFIG;";
constexpr char CMD_MOVE_JOINTS[] = "MOVE_JOINTS[";
constexpr char CMD_MOVE_TO[] = "MOVE_TO[";
constexpr char CMD_HOME[] = "HOME;";

extern "C" int _write(int file, char *ptr, int len)
{
    // Wysyła ciąg znaków przez UART2
    HAL_UART_Transmit(&huart2, (uint8_t *)ptr, (uint16_t)len, HAL_MAX_DELAY);
    return len;
}

extern "C" void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
    if (huart->Instance == USART2)
    {

        // Zabezpieczenie na końcu odebranego ciągu znaków
        rx_buffer[Size] = '\0';

        // 1. Sprawdzanie komend sterujących strumieniem
        if (strncmp((char *)rx_buffer, CMD_START_STREAM, sizeof(CMD_START_STREAM) - 1) == 0)
        {
            is_streaming = true;
            printf("ACK_STREAM\r\n"); // Python na to czeka!
        }
        else if (strncmp((char *)rx_buffer, CMD_STOP_STREAM, sizeof(CMD_STOP_STREAM) - 1) == 0)
        {
            is_streaming = false;
            printf("Zatrzymano strumien.\r\n");
        }
        else if (strncmp((char *)rx_buffer, CMD_GET_CONFIG, sizeof(CMD_GET_CONFIG) - 1) == 0)
        {
            ServoManager::print_servos_config();
        }
        else if (strncmp((char *)rx_buffer, CMD_MOVE_JOINTS, sizeof(CMD_MOVE_JOINTS) - 1) == 0)
        {

            // 1. Znajdujemy nawias zamykający ']' i podmieniamy go na znak końca stringa '\0'
            // Dzięki temu ucinamy "\n" i średnik, zostawiając same dane.
            char *end_bracket = strchr((char *)rx_buffer, ']');
            if (end_bracket != nullptr)
            {
                *end_bracket = '\0';

                // 2. data_start wskazuje teraz na pierwszy znak po "MOVE_JOINTS["
                // np. tekst: "2.5,+,0,-,0,0,0"
                char *data_start = (char *)rx_buffer + sizeof(CMD_MOVE_JOINTS) - 1;

                // 3. Rozbijamy tekst po przecinkach używając strtok
                char *token = strtok(data_start, ",");

                if (token != nullptr)
                {
                    // Pierwszy element to nasza prędkość (float)
                    float move_speed = atof(token);

                    // Tablica na 6 kierunków ('+', '-', '0')
                    std::array<char, 6> directions = {'0', '0', '0', '0', '0', '0'};

                    // Pobieramy kolejne 6 elementów
                    for (int i = 0; i < 6; i++)
                    {
                        token = strtok(nullptr, ",");
                        if (token != nullptr)
                        {
                            // Zapisujemy tylko pierwszy znak z tokena
                            directions[i] = token[0];
                        }
                    }

                    // 4. Mamy wszystko! Przekazujemy rozpakowane dane do Menedżera Serw
                    ServoManager::handle_manual_move(move_speed, directions);
                    // TODO return via uart actual position, add in webApp update of new position
                    ServoManager::print_current_angles();
                }
            }
        }
        else if (strncmp((char *)rx_buffer, CMD_MOVE_TO, sizeof(CMD_MOVE_TO) - 1) == 0)
        {
            float j[6] = {0.0f};
            char *data_start = (char *)rx_buffer + sizeof(CMD_MOVE_TO) - 1;

            // Parsowanie 6 floatów oddzielonych przecinkami
            int parsed = sscanf(data_start, "%f,%f,%f,%f,%f,%f",
                                &j[0], &j[1], &j[2], &j[3], &j[4], &j[5]);

            if (parsed == 6)
            {
                angles = {j[0], j[1], j[2], j[3], j[4], j[5], 0.0};
                ServoManager::set_angles(angles);
                ServoManager::print_current_angles();
            }
            else
            {
                printf("ERR_PARSE_MOVE_TO\r\n");
            }
        }
        else if (strncmp((char *)rx_buffer, CMD_HOME, sizeof(CMD_HOME) - 1) == 0)
        {
            angles = {0.0, 90.0, 0.0, 0.0, 0.0, 0.0, 0.0};
            ServoManager::set_angles(angles);
            ServoManager::print_current_angles();
        }

        // 2. Parsowanie ramek z pozycjami (format: JN[ID, J1, J2, J3, J4, J5, J6];)
        // else if (is_streaming && rx_buffer[0] == 'J' && rx_buffer[1] == 'N')
        // {
        //     int frame_id = 0;
        //     float j[6] = {0.0f};

        //     // %d to ID ramki, %f to kolejne kąty
        //     int parsed = sscanf((char *)rx_buffer, "JN[%d,%f,%f,%f,%f,%f,%f]",
        //                         &frame_id, &j[0], &j[1], &j[2], &j[3], &j[4], &j[5]);

        //     if (parsed == 7)
        //     {
        //         // SUKCES: Odsyłamy Pythonowi potwierdzenie dla tej konkretnej ramki
        //         printf("FB[%d,OK]\r\n", frame_id);

        //         // --- TUTAJ MOŻESZ PRZEKAZAĆ KĄTY DO SERWO MANAGERA ---
        //         // Twój ServoManager ma std::array<double, 7>, więc możemy to przepisać:

        //         angles = {j[0], j[1], j[2], j[3], j[4], j[5], 0.0};
        //         ServoManager::set_angles(angles);
        //     }
        //     else
        //     {
        //         // BŁĄD: Ramka była uszkodzona
        //         printf("FB[%d,ERR_PARSE]\r\n", frame_id);
        //     }
        // }

        // 3. Krytyczne: Ponowne uzbrojenie DMA na kolejną paczkę danych
        HAL_UARTEx_ReceiveToIdle_DMA(&huart2, rx_buffer, sizeof(rx_buffer));
        __HAL_DMA_DISABLE_IT(&hdma_usart2_rx, DMA_IT_HT); // Wyłączenie przerwania w połowie
    }
}

int app()
{

    // Włączenie zasilania serw (Przekaźnik)
    HAL_GPIO_WritePin(RELAY_GPIO_Port, RELAY_Pin, GPIO_PIN_SET);

    printf("Zaczynamy! STM32 gotowe do pracy w C++\r\n");
    printf("Test floatow: %.2f\r\n", 3.14);

    ServoManager::init();

    // Inicjalizacja serw na pozycje startowe

    // std::array<uint16_t, 7> pwm_values = {1500, 1500, 1500, 1500, 1500, 1500, 1500};
    // ServoManager::set_pwm(pwm_values);
    ServoManager::set_angles(angles);

    // NOWE: Uruchomienie nasłuchiwania UART przez DMA przed wejściem w pętlę!
    HAL_UARTEx_ReceiveToIdle_DMA(&huart2, rx_buffer, sizeof(rx_buffer));
    __HAL_DMA_DISABLE_IT(&hdma_usart2_rx, DMA_IT_HT);

    // Główna pętla - procesor jest całkowicie wolny i odciążony
    while (true)
    {

        // Tutaj będą tylko rzeczy nieblokujące, np. sprawdzanie czujników, INA3221 itp.

        /* if (htim3_has_passed){
            ServoManager::set_angles(new_angles);
        }*/
        // if(HAL_GPIO_ReadPin(B1_GPIO_Port, B1_Pin) == GPIO_PIN_RESET) {
        //     printf("Przycisk wcisniety!\r\n");
        //     Error_Handler();
        // }
    }
}