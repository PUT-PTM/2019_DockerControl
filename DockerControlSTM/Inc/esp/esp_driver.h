#ifndef HELLO_WIFI_ESP_DRIVER_H
#define HELLO_WIFI_ESP_DRIVER_H

#include "esp_connection_data.h"

enum esp_connection_state { IDLE, WAIT_HEADER, WAIT_BODY };

void esp_start(UART_HandleTypeDef * huart);

void esp_send_command(UART_HandleTypeDef * huart, uint8_t * command, uint16_t size);

void esp_send_def_command(UART_HandleTypeDef * huart, const char * command);

#endif //HELLO_WIFI_ESP_DRIVER_H
