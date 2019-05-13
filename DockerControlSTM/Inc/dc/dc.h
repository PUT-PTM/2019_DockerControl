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

void dc_new_cmd(const uint8_t * packet_header, const uint8_t * packet_body);
void dc_resolve_cmd(const uint8_t * packet_body);
void dc_apply_cmd(const uint8_t * packet_header, const uint8_t * packet_body);

void dc_add_data_container(const uint8_t * container_index);
void dc_add_data_image(const uint8_t * image_index);

void dc_set_ready();

void dc_start_session(UART_HandleTypeDef * huart);

void dc_send(UART_HandleTypeDef * huart);

void dc_set_session_id(const uint8_t * packet_header);
void dc_update_containers(const uint8_t * packet_body);
void dc_update_images(const uint8_t * packet_body);


#endif //DOCKERCONTROLSTM_DC_H
