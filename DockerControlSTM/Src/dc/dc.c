#include "main.h"
#include "esp/esp_driver.h"
#include "dc/dc.h"

enum DC_COMMAND_ENUM cmd = ACKN;

enum DC_COMMAND_ENUM containers_cmd = CALL;

char dc_session_id[4] = "000";

uint8_t dc_ready = 0;
uint8_t dc_wait = 0;

uint8_t dc_header[10] = {PACKET_BEGIN, '0', '0', '0', PACKET_DELIMITER, '0', '0', '0', '0', PACKET_DELIMITER }; //, '\r', '\n'};
uint8_t dc_body[4096];

uint8_t dc_data_size = 0;
uint8_t dc_data[4090];

uint8_t dc_containers_size = 0;
struct container dc_containers[20];

uint8_t dc_images_size = 0;
image dc_images[20];

struct stats dc_stats;

uint8_t dc_alerts_size = 0;
alert dc_alerts[20];

uint8_t dc_update = 0;
int16_t dc_since_update = 0;
uint8_t dc_containers_update = 0;
uint8_t dc_images_update = 0;
uint8_t dc_stats_update = 0;

const uint8_t dc_cmd_copy_data_condition(const uint8_t * const d) {
    if (*d != PACKET_DATA_DELIMITER && *d != PACKET_DATA_ARRAY_DELIMITER && *d != PACKET_END) return 1;
    return 0;
}

const uint8_t dc_cmd_copy_data(const uint8_t * const source, const uint16_t start, uint8_t * const destination) {
    uint8_t length;
    for (length = 0; dc_cmd_copy_data_condition(&(source[length + start])); length++);
    memcpy(destination, source + start, length);
    return length;
}

void dc_start_update(TIM_HandleTypeDef * const htim) {
    dc_since_update = -5 * DC_UPDATE_INTERVAL;
    dc_update_callback();
    HAL_TIM_Base_Start_IT(htim);
    util_log("update timer started");
}

void dc_start(UART_HandleTypeDef * const huart_esp, TIM_HandleTypeDef * const htim_update) {
    esp_start(huart_esp);

    cmd = READ;
    dc_send(huart_esp);

    dc_start_update(htim_update);
}

void dc_cmd_get_data(const uint8_t * const packet_body, uint16_t * const i, uint8_t * const destination) {
    const uint8_t length = dc_cmd_copy_data(packet_body, *i, destination);
    *(destination + length) = '\0';
    *i += length + 1;
}

void dc_set_session_id(const uint8_t * const packet_header) {
    strncpy(dc_session_id, (const char *) packet_header + 1, 3);
    dc_session_id[3] = '\0';
    util_log(dc_session_id);
}

void dc_ready_action(const uint8_t * const packet_header) {
    dc_set_session_id(packet_header);
    dc_since_update = DC_UPDATE_INTERVAL;
    dc_update_callback();
}

void dc_ackn_action() {
    cmd = ACKN;
    dc_add_empty_data();
    dc_set_ready();
}

void dc_update_containers(const uint8_t * const packet_body) {
    dc_containers_size = 0;
    uint16_t i = PACKET_BODY_DATA_START;
    while (packet_body[i] != PACKET_END) {
        dc_cmd_get_data(packet_body, &i, dc_containers[dc_containers_size].id);
        dc_cmd_get_data(packet_body, &i, dc_containers[dc_containers_size].name);
        dc_cmd_get_data(packet_body, &i, dc_containers[dc_containers_size].image);
        dc_cmd_get_data(packet_body, &i, dc_containers[dc_containers_size].state);
        dc_cmd_get_data(packet_body, &i, dc_containers[dc_containers_size].status);

        util_log(dc_containers[dc_containers_size].name);

        dc_containers_size++;
    }
}

void dc_update_images(const uint8_t * const packet_body) {
    dc_images_size = 0;
    uint16_t i = PACKET_BODY_DATA_START;
    while (packet_body[i] != PACKET_END) {
        dc_cmd_get_data(packet_body, &i, dc_images[dc_images_size]);
        dc_images_size++;
    }
}

void dc_update_stats(const uint8_t * const packet_body) {
    uint16_t i = PACKET_BODY_DATA_START;
    dc_cmd_get_data(packet_body, &i, dc_stats.active_containers);
    dc_cmd_get_data(packet_body, &i, dc_stats.paused_containers);
    dc_cmd_get_data(packet_body, &i, dc_stats.stopped_containers);
    dc_cmd_get_data(packet_body, &i, dc_stats.images);
    dc_cmd_get_data(packet_body, &i, dc_stats.cpu);
    dc_cmd_get_data(packet_body, &i, dc_stats.memory);
}

void dc_update_alerts(const uint8_t * const packet_body) {
    util_log("update alerts");
    dc_alerts_size = 0;
    uint16_t i = PACKET_BODY_DATA_START;
    while (packet_body[i] != PACKET_END) {
        dc_cmd_get_data(packet_body, &i, dc_alerts[dc_alerts_size]);
        dc_alerts_size++;
    }
}

void dc_clear_alerts() {
    dc_alerts_size = 0;
}

void dc_new_cmd(const uint8_t * const packet_header, const uint8_t * const packet_body) {
    dc_resolve_cmd(packet_body);
    util_log(DC_COMMAND_STRING[cmd]);
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
            dc_ready_action(packet_header);
            break;
        case ACKN:
            dc_ackn_action();
            break;
        case CALL:
            dc_update_containers(packet_body);
            break;
        case CACT:
            dc_update_containers(packet_body);
            break;
        case CSTS:
            dc_update_stats(packet_body);
            break;
        case CSTR:
            // not receive
            break;
        case CSTP:
            // not receive
            break;
        case CRST:
            // not receive
            break;
        case CRMV:
            // not receive
            break;
        case CCRT:
            // not receive
            break;
        case IALL:
            dc_update_images(packet_body);
            break;
        case SSTS:
            dc_update_stats(packet_body);
            break;
        case ALRT:
            dc_update_alerts(packet_body);
            break;
        case ERRR:
            util_log((char*)packet_body);
            break;
        default:break;
    }
}

void dc_add_data(const uint8_t * const data, const uint8_t size) {
    memcpy(dc_data, data, size);
    dc_data_size = size;
}

void dc_add_empty_data() {
    static const uint8_t empty[0] = {};
    dc_add_data(empty, 0);
}

void dc_add_data_container(const uint8_t * const container_index) {
    dc_add_data(dc_containers[*container_index].id, 64);
}

void dc_add_data_image(const uint8_t * const image_index) {
    uint8_t size = 0;
    while (dc_images[*image_index][size] != '\0') size++;
    dc_add_data(dc_images[*image_index], size);
}

void dc_set_ready() { dc_ready = 1; }

void dc_make_header() {
    dc_header[1] = dc_session_id[0];
    dc_header[2] = dc_session_id[1];
    dc_header[3] = dc_session_id[2];

    const uint16_t body_size = 6 + dc_data_size;

    uint8_t body_size_string[4];
    sprintf((char *) body_size_string, "%04d", body_size);
    dc_header[5] = body_size_string[0];
    dc_header[6] = body_size_string[1];
    dc_header[7] = body_size_string[2];
    dc_header[8] = body_size_string[3];
}

void dc_make_body() {
    memcpy(dc_body, DC_COMMAND_STRING[cmd], 4);
    dc_body[4] = PACKET_DELIMITER;
    memcpy(dc_body + 5, dc_data, dc_data_size);
    dc_body[5 + dc_data_size] = PACKET_END;
}

void dc_send(UART_HandleTypeDef * const huart) {
    util_log("sending packet");
    dc_make_body();
    dc_make_header();
    esp_send_command(huart, dc_header, 10);
    esp_send_command(huart, dc_body, 6 + dc_data_size);
    dc_wait = 1;
}

inline void dc_update_callback() {
    dc_since_update += DC_UPDATE_TIMER_INTERVAL;
    if (dc_since_update >= DC_UPDATE_INTERVAL) {
        if (!dc_update) {
            dc_update = 1;
            dc_containers_update = 1;
            dc_images_update = 1;
            dc_stats_update = 1;
            dc_since_update = 0;
        }
    }
}

inline void dc_update_action() {
    if (dc_containers_update) {
        cmd = containers_cmd;
        dc_add_empty_data();
        dc_set_ready();
        dc_containers_update = 0;
    }
    else if (dc_images_update) {
        cmd = IALL;
        dc_add_empty_data();
        dc_set_ready();
        dc_images_update = 0;
    }
    else if (dc_stats_update) {
        cmd = SSTS;
        dc_add_empty_data();
        dc_set_ready();
        dc_stats_update = 0;
    }
    else {
        dc_update = 0;
    }
    dc_wait = 1;
}

void dc_set_containers_update_all() {
    containers_cmd = CALL;
}

void dc_set_containers_update_active() {
    containers_cmd = CACT;
}