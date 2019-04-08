#include "main.h"
#include <string.h>
#include "dc/dc.h"

enum DC_COMMAND_ENUM cmd = ACKN;

uint8_t dc_session_id = 0;

uint8_t dc_header[10] = {PACKET_BEGIN, '0', '0', '0', PACKET_DELIMITER, '0', '0', '0', '0', PACKET_DELIMITER }; //, '\r', '\n'};
uint8_t dc_body[4096];

void dc_resolve_cmd(const uint8_t * const body) {
    char command[5];
    strncpy(command, (const char *) body, 4);
    command[4] = '\0';

    enum DC_COMMAND_ENUM i;
    for (i = READ; i <= ERRR; i++)
        if (strcmp(DC_COMMAND_STRING[i], command) == 0) {
            cmd = i;
            return;
        }

    cmd = ERRR;
}

void dc_make_header() {
    uint8_t session_id_string[3];
    sprintf((char *) session_id_string, "%03d", dc_session_id);
    dc_header[1] = session_id_string[0];
    dc_header[2] = session_id_string[1];
    dc_header[3] = session_id_string[2];

    uint16_t body_size;
    for (body_size = 0; dc_body[body_size] != PACKET_END; body_size++);
    body_size++;

    uint8_t body_size_string[4];
    sprintf((char *) body_size_string, "%04d", body_size);
    dc_header[5] = body_size_string[0];
    dc_header[6] = body_size_string[1];
    dc_header[7] = body_size_string[2];
    dc_header[8] = body_size_string[3];
}

void dc_make_body() {
    dc_body[0] = (uint8_t) DC_COMMAND_STRING[cmd][0];
    dc_body[1] = (uint8_t) DC_COMMAND_STRING[cmd][1];
    dc_body[2] = (uint8_t) DC_COMMAND_STRING[cmd][2];
    dc_body[3] = (uint8_t) DC_COMMAND_STRING[cmd][3];
    dc_body[4] = PACKET_DELIMITER;

    // add data

    dc_body[5] = PACKET_END;
}

void dc_start_session(UART_HandleTypeDef * const huart) {
    esp_init(huart);
    esp_passthrough(huart);

    cmd = READ;
    dc_send(huart);
}

void dc_send(UART_HandleTypeDef * const huart) {
    dc_make_body();
    dc_make_header();
    esp_send_command(huart, dc_header, 10);
    esp_send_command(huart, dc_body, 6);
}