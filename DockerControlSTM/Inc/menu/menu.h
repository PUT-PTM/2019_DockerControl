#ifndef DOCKERCONTROLSTM_MENU_H
#define DOCKERCONTROLSTM_MENU_H

#include <sched.h>
#include <stdbool.h>


extern char menu_first_line[48];
extern char menu_second_line[48];

extern char name[48];
extern volatile int name_pos;
extern uint8_t character[1];
extern bool shift;
extern uint8_t back;

//buttons
extern volatile int enter_pressed;
extern volatile int confirm_pressed;
extern volatile int back_pressed;

//encoder
extern volatile uint8_t positions;

// system data
extern char server_ip;
extern char server_port;
extern char wifi_name;
extern char wifi_password;

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
extern esp_param;
extern current_menu;
menu show_menu();

#endif