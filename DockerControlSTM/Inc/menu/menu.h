#ifndef DOCKERCONTROLSTM_MENU_H
#define DOCKERCONTROLSTM_MENU_H

#include <sched.h>
#include <stdbool.h>
#include<string.h>
//#include "dc/dc.h"

//screen
extern char menu_first_line[48];
extern char menu_second_line[48];

extern char name[48];
extern volatile int name_pos;
extern uint8_t character[1];
extern bool shift;
extern uint8_t back;

//mock
struct container_mock {
    int id;
    char name[51];
    uint8_t image[51];
    uint8_t state[11];
    uint8_t status[11];
};

//buttons
extern volatile int enter_pressed;
extern volatile int confirm_pressed;
extern volatile int back_pressed;

//encoder
extern volatile uint8_t pulse_count;
extern volatile uint8_t positions;

// system data
extern char server_ip;
extern char server_port;
extern char wifi_name;
extern char wifi_password;

//menu
extern bool menu_finished;
extern bool show_containers_finished;
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
void show_containers();


void menu_line(int line, char * format, ...);
void update_screen();

#endif