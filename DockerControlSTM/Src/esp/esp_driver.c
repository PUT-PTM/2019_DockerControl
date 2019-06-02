#include <esp/esp_commands.h>
#include <usbd_cdc_if.h>
#include "esp/esp_driver.h"
#include "main.h"
#include "utils.h"
#include <stdlib.h>
#include "usbd_cdc_if.h"

char server_ip[15] = DEFAULT_SERVER_IP;
char server_port[5] = DEFAULT_SERVER_PORT;

char wifi_name[100] = DEFAULT_WIFI_NAME;
char wifi_password[100] = DEFAULT_WIFI_PASSWORD;

uint8_t esp_received_packet_header[10];
uint8_t esp_received_packet_body[4096];

enum esp_connection_state esp_connection_state = IDLE;

uint8_t esp_packet_received = 0;

uint8_t esp_uart_receive[5];
const uint16_t esp_uart_size = 5;

void esp_test(UART_HandleTypeDef * const huart) {
    esp_send_def_command(huart, ESP_TEST);
    util_log(ESP_TEST);
    HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_15);
    HAL_Delay(200);
}

void esp_wait(UART_HandleTypeDef * const huart) {
    util_log("waiting for esp...");
    for (int i = 0; i < 5; i++)
    {
        HAL_Delay(1200);
        esp_test(huart);
    }
}

void esp_trigg() {
    util_log("esp trigg...");
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_11, GPIO_PIN_SET);
    HAL_Delay(50);
}

void esp_init(UART_HandleTypeDef * const huart) {
    esp_wait(huart);

    util_log("configuring esp...");

    // set no echo
    const char * command = ESP_NO_ECHO;
    esp_send_def_command(huart, command);
    util_log(command);
    free((void *) command);
    HAL_Delay(100);

    // set esp as client
    command = ESP_WIFI_CLIENT;
    esp_send_def_command(huart, command);
    util_log(command);
    free((void *) command);
    HAL_Delay(1000);

    // connect esp to wifi
    char wifi[50];
    wifi[0] = '\0';

    strcat(wifi, ESP_WIFI_CONNECT_BEGIN);
    strcat(wifi, wifi_name);
    strcat(wifi, ESP_WIFI_CONNECT_DELIMITER);
    strcat(wifi, wifi_password);
    strcat(wifi, ESP_WIFI_CONNECT_END);
    strcat(wifi, "\0");

    esp_send_def_command(huart, wifi);
    util_log(wifi);

    HAL_Delay(8000);

    // get esp ip
    command = ESP_GET_IP;
    esp_send_def_command(huart, command);
    util_log(command);
    free((void *) command);
    HAL_Delay(1000);

    util_log("esp ready");
}

void esp_passthrough(UART_HandleTypeDef * const huart) {
    util_log("enter esp passthrough");

    char server[150];
    server[0] = '\0';

    strcat(server, ESP_TCP_CONNECT_TO_SERVER_BEGIN);
    strcat(server, server_ip);
    strcat(server, ESP_TCP_CONNECT_TO_SERVER_DELIMITER);
    strcat(server, server_port);
    strcat(server, ESP_TCP_CONNECT_TO_SERVER_END);
    strcat(server, "\0");

    esp_send_def_command(huart, server);
    util_log(server);
    HAL_Delay(1000);

    esp_send_def_command(huart, ESP_PASSTHROUGH_MODE);
    esp_send_def_command(huart, ESP_PASSTHROUGH_START);
    HAL_Delay(1500);

    util_log("esp in passthrough");
    esp_connection_state = WAIT_HEADER;
}

const uint16_t esp_process_header() {
    char body_size_string[4];
    memcpy(body_size_string, &esp_received_packet_header[5], 4);
    const uint16_t body_size = (uint16_t) atoi(body_size_string);
    esp_connection_state = WAIT_BODY;
    return body_size;
}

void esp_process_body() {
    esp_packet_received = 1;
    esp_connection_state = WAIT_HEADER;
}

void esp_clear_uart_buff(UART_HandleTypeDef * const huart) {
    util_log("clearing buffer...");
    uint8_t mock[1];
    while (HAL_UART_Receive(huart, mock, 1, 10) == HAL_OK);
    util_log("buffer cleared");
}

inline void esp_data_callback(UART_HandleTypeDef * const huart) {
    HAL_UART_AbortReceive_IT(huart);
    HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_13);
    if(huart->Instance == USART3) {
        switch (esp_connection_state) {
            case WAIT_HEADER:
            {
                const uint16_t body_size = esp_process_header();
                HAL_UART_Receive_IT(huart, esp_received_packet_body, body_size);
            }
                break;
            case WAIT_BODY:
                esp_process_body();
                HAL_UART_Receive_IT(huart, esp_received_packet_header, sizeof(esp_received_packet_header));
                break;
            case IDLE:
                CDC_Transmit_FS(esp_uart_receive, esp_uart_size);
                HAL_UART_Receive_IT(huart, esp_uart_receive, esp_uart_size);
                break;
            default:break;
        }
    }
}

void esp_start(UART_HandleTypeDef * const huart) {
    HAL_UART_Receive_IT(huart, esp_uart_receive, esp_uart_size);

    esp_trigg();
    esp_init(huart);
    esp_passthrough(huart);

    HAL_UART_AbortReceive_IT(huart);
    HAL_UART_Receive_IT(huart, esp_received_packet_header, sizeof(esp_received_packet_header));
}

void esp_send_command(UART_HandleTypeDef * huart, uint8_t * const command, const uint16_t size) {
    HAL_UART_Transmit_IT(huart, command, size);
    HAL_Delay(200);
}

void esp_send_def_command(UART_HandleTypeDef * huart, const char * const command) {
    esp_send_command(huart, (uint8_t *) command, (const uint16_t) strlen(command));
}
