#ifndef DOCKERCONTROLSTM_MENU_H
#define DOCKERCONTROLSTM_MENU_H

#include "main.h"
#include <stdbool.h>
#include<string.h>
#include "dc/dc.h"

typedef enum{
    MENU_START,
    MENU_DEFAULT,
    MENU_CUSTOM,
    MENU_CUSTOM_SERVER,
    MENU_CUSTOM_WIFI
} menu;

typedef enum{
    PARAM_IP,
    PARAM_PORT,
    PARAM_NAME,
    PARAM_PASSWORD
} esp_params;

menu show_menu();
void show_containers(const struct container * containers, const uint8_t * size);
void menu_line(uint8_t line, char * format, ...);
void update_screen();

#endif