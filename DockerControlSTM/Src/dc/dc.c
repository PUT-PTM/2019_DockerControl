#include "main.h"
#include <string.h>
#include "dc/dc.h"

enum DC_COMMAND_ENUM cmd = ACKN;

char dc_session_id[4] = "000";

uint8_t dc_header[10] = {PACKET_BEGIN, '0', '0', '0', PACKET_DELIMITER, '0', '0', '0', '0', PACKET_DELIMITER }; //, '\r', '\n'};
uint8_t dc_body[4096];

uint8_t new_containers = 0;
uint8_t containers_size = 0;
struct container containers[20];

uint8_t new_images = 0;
uint8_t images_size = 0;
uint8_t images[20][50];

void dc_new_cmd(const uint8_t * const packet_header, const uint8_t * const packet_body) {
    dc_resolve_cmd(packet_body);
    dc_apply_cmd(packet_header, packet_body);
}

void dc_resolve_cmd(const uint8_t * const packet_body) {
    char command[5];
    strncpy(command, (const char *) packet_body, 4);
    command[4] = '\0';

    enum DC_COMMAND_ENUM i;
    for (i = READ; i <= ERRR; i++)
        if (strcmp(DC_COMMAND_STRING[i], command) == 0) {
            cmd = i;
            return;
        }

    cmd = ERRR;
}

void dc_apply_cmd(const uint8_t * const packet_header, const uint8_t * const packet_body) {
    switch (cmd) {
        case READ:
            dc_set_session_id(packet_header);
            break;
        case ACKN:break;
        case CALL:
            dc_update_containers(packet_body);
            break;
        case CACT:
            dc_update_containers(packet_body);
            break;
        case CSTS:break;
        case CSTR:break;
        case CSTP:break;
        case CRST:break;
        case CRMV:break;
        case CCRT:break;
        case IALL:
            dc_update_images(packet_body);
            break;
        case SSTS:break;
        case ALRT:break;
        case ERRR:break;
        default:break;
    }
}

void dc_make_header() {
    dc_header[1] = dc_session_id[0];
    dc_header[2] = dc_session_id[1];
    dc_header[3] = dc_session_id[2];

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

const uint8_t dc_cmd_copy_data(const uint8_t * const source, const uint16_t start, uint8_t * const destination) {
    uint8_t length;
    for (length = 0; source[length + start] != PACKET_DATA_DELIMITER && source[length + start] != PACKET_DATA_ARRAY_DELIMITER; length++);
    memcpy(destination, source + start, length);
    return length;
}

void dc_cmd_get_data(const uint8_t * const packet_body, uint16_t * const data_length, uint8_t * const destination) {
    const uint8_t length = dc_cmd_copy_data(packet_body, *data_length, destination);
    *(destination + length) = '\0';
    *data_length += length + 1;
}


void dc_set_session_id(const uint8_t * const packet_header) {
    strncpy(dc_session_id, (const char *) packet_header + 1, 3);
    dc_session_id[3] = '\0';
    util_log(dc_session_id);
}

void dc_update_containers(const uint8_t * const packet_body) {
    containers_size = 0;
    uint16_t data_length = PACKET_BODY_DATA_START;
    while (packet_body[data_length] != PACKET_END) {
        dc_cmd_get_data(packet_body, &data_length, containers[containers_size].id);
        dc_cmd_get_data(packet_body, &data_length, containers[containers_size].name);
        dc_cmd_get_data(packet_body, &data_length, containers[containers_size].image);
        dc_cmd_get_data(packet_body, &data_length, containers[containers_size].state);
        dc_cmd_get_data(packet_body, &data_length, containers[containers_size].status);
        containers_size++;
    }
    new_containers = 1;
}

void dc_update_images(const uint8_t * const packet_body) {
    images_size = 0;
    uint16_t data_length = PACKET_BODY_DATA_START;
    while (packet_body[data_length] != PACKET_END) dc_cmd_get_data(packet_body, &data_length, images[images_size]);
    new_images = 1;
}
