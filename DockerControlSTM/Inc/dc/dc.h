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

typedef uint8_t alert[65];

#define DC_UPDATE_TIMER_INTERVAL 30
#define DC_UPDATE_INTERVAL 30

void dc_new_cmd(const uint8_t * packet_header, const uint8_t * packet_body);
void dc_resolve_cmd(const uint8_t * packet_body);
void dc_apply_cmd(const uint8_t * packet_header, const uint8_t * packet_body);

void dc_add_empty_data();
void dc_add_data_container(const uint8_t * container_index);
void dc_add_data_image(const uint8_t * image_index);

void dc_clear_alerts();

void dc_set_ready();

void dc_start(UART_HandleTypeDef * huart_esp, TIM_HandleTypeDef * htim_update);

void dc_send(UART_HandleTypeDef * huart);

void dc_update_callback();
void dc_update_action();
void dc_set_containers_update_all();
void dc_set_containers_update_active();

#endif //DOCKERCONTROLSTM_DC_H
