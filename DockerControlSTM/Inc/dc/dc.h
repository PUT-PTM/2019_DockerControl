#ifndef DOCKERCONTROLSTM_DC_H
#define DOCKERCONTROLSTM_DC_H

#include "dc_commands.h"

struct container {
    uint8_t id[65];
    uint8_t name[51];
    uint8_t image[51];
    uint8_t state[11];
    uint8_t status[11];
};

typedef uint8_t image[51];

struct stats {
    uint8_t active_containers[3];
    uint8_t paused_containers[3];
    uint8_t stopped_containers[3];
    uint8_t images[5];
    uint8_t cpu[3];
    uint8_t memory[7];
};

void dc_new_cmd(const uint8_t * packet_header, const uint8_t * packet_body);
void dc_resolve_cmd(const uint8_t * packet_body);
void dc_apply_cmd(const uint8_t * packet_header, const uint8_t * packet_body);

void dc_add_data_container(const uint8_t * container_index);
void dc_add_data_image(const uint8_t * image_index);

void dc_set_ready();

void dc_start_session(UART_HandleTypeDef * huart);

void dc_send(UART_HandleTypeDef * huart);

#endif //DOCKERCONTROLSTM_DC_H
