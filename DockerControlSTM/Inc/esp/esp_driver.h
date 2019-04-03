#ifndef HELLO_WIFI_ESP_DRIVER_H
#define HELLO_WIFI_ESP_DRIVER_H

#include "esp_connection_data.h"

enum esp_connection_state { IDLE, WAIT_HEADER, WAIT_BODY };

enum esp_cmd { ACK, CALL, CACT };

void esp_wait(UART_HandleTypeDef * huart);

void esp_init(UART_HandleTypeDef * huart);

void esp_send_command(UART_HandleTypeDef * huart, const char * command);

void esp_test(UART_HandleTypeDef * huart);

void esp_passthrough(UART_HandleTypeDef * huart);

const uint16_t esp_process_header(UART_HandleTypeDef * huart);

void esp_process_body(UART_HandleTypeDef * huart);

void esp_clear_uart_buff(UART_HandleTypeDef * huart);



#endif //HELLO_WIFI_ESP_DRIVER_H
