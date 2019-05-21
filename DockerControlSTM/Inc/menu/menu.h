#ifndef DOCKERCONTROLSTM_MENU_H
#define DOCKERCONTROLSTM_MENU_H

#include "main.h"
#include <string.h>
#include "dc/dc.h"

typedef enum {
    MENU_START,
    MENU_DEFAULT,
    MENU_CUSTOM,
    MENU_CUSTOM_SERVER,
    MENU_CUSTOM_WIFI,
} menu_connection;

typedef enum{
    PARAM_IP,
    PARAM_PORT,
    PARAM_NAME,
    PARAM_PASSWORD
} esp_params;


typedef enum {
    MAIN_START,
    MAIN_CONTAINERS,
    MAIN_IMAGES,
    MAIN_STATS,
    MAIN_ALERTS
} menu_main;

void menu_line(uint8_t line, char * format, ...);
void update_screen();

void connection_menu();

void main_menu(const struct container * containers, const uint8_t * containers_size,
               const image * images, const uint8_t * images_size,
               const struct stats * _stats,
               const alert * _alert, const uint8_t * alerts_size
                       );

#endif