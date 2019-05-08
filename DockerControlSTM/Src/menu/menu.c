#include <hd44780.h>
#include <stdarg.h>
#include "menu/menu.h"
#include "main.h"

uint8_t esp_param = PARAM_IP;
uint8_t current_menu = MENU_START;

// lcd
char menu_first_line[48];
char menu_second_line[48];

// string
uint8_t name[48];
volatile uint8_t name_pos = 0;
uint8_t character[1] = {0};

// encoder
volatile uint8_t pulse_count;
volatile uint8_t positions;

// system data
extern char server_ip;
extern char server_port;
extern char wifi_name;
extern char wifi_password;

// menu
bool menu_finished = false;

// buttons
volatile uint8_t enter_pressed = 0; //encoder
volatile uint8_t confirm_pressed = 0; //green
volatile uint8_t back_pressed = 0; //white
volatile uint8_t shift_pressed = 0; //blue
bool shift = 0;

int button_enter(){
    if(enter_pressed){
        enter_pressed = 0;
        return 1;
    }
    else{
        return 0;
    }
}

int button_shift(){
    if(shift_pressed){
        shift_pressed = 0;
        return 1;
    }
    else{
        return 0;
    }
}

int button_back(){
    if(back_pressed){
        back_pressed = 0;
        return 1;
    }
    else{
        return 0;
    }
}

int button_confirm(){
    if(confirm_pressed){
        confirm_pressed = 0;
        return 1;
    }
    else{
        return 0;
    }
}

void update_screen() {
    hd44780_clear();
    hd44780_position(0, 1);
    hd44780_printf("%s", menu_first_line);
    hd44780_position(1, 1);
    hd44780_printf("%s", menu_second_line);
}

void menu_line(uint8_t line, char *format, ...) {
    char buffer[50];

    va_list args;
    va_start(args, format);
    vsprintf(buffer, format, args);
    va_end(args);
    if(line == 0) {
        if (strcmp(menu_first_line, buffer) != 0) {
            strcpy(menu_first_line, buffer);
            update_screen();
        }
    }
    else if (line == 1){
        if (strcmp(menu_second_line, buffer) != 0) {
            strcpy(menu_second_line, buffer);
            update_screen();
        }
    }
}

void menu_custom(){
    switch (positions % 5) {
        case 0:
            menu_line(0, ">Server IP: %s", &server_ip);
            menu_line(1, " Server Port: %s", &server_port);
            if (button_enter()) {
                esp_param = PARAM_IP;
                current_menu = MENU_CUSTOM_SERVER;

            }
            break;
        case 1:
            menu_line(0, " Server IP: %s", &server_ip);
            menu_line(1, ">Server Port: %s", &server_port);
            if (button_enter()) {
                esp_param = PARAM_PORT;
                current_menu = MENU_CUSTOM_SERVER;
            }
            break;
        case 2:
            menu_line(0, ">WiFi Name: %s", &wifi_name);
            menu_line(1, " WiFi Password: %s", &wifi_password);
            if (button_enter()) {
                esp_param = PARAM_NAME;
                current_menu = MENU_CUSTOM_WIFI;
            }
            break;
        case 3:
            menu_line(0, " WiFi Name: %s", &wifi_name);
            menu_line(1, ">WiFi Password: %s", &wifi_password);
            if (button_enter()) {
                esp_param = PARAM_PASSWORD;
                current_menu = MENU_CUSTOM_WIFI;
            }
            break;
        case 4:
            menu_line(0, ">Connect");
            menu_line(1, "");
            if (button_enter()) {
                menu_line(0, "Connecting...");
                menu_line(1, "");
                menu_finished = true;
            }
        default:
            break;
    }
}

void menu_start(){
    switch (positions % 2) {
        case 0:
            menu_line(0, ">Default");
            menu_line(1, " Custom");
            if (button_enter()) {
                current_menu = MENU_DEFAULT;
            }
            break;
        case 1:
            menu_line(0, " Default");
            menu_line(1, ">Custom");
            if (button_enter()) {
                current_menu = MENU_CUSTOM;
            }
            break;
        default:
            break;
    }
}

menu show_menu(){
    util_log("Show_menu begin");

    while(!menu_finished) {
        pulse_count = (uint8_t) TIM1->CNT;
        positions = (uint8_t) (pulse_count / 4);
        switch (current_menu) {
            case MENU_START:
                menu_start();
                break;
            case MENU_DEFAULT:
                menu_line(0, "Connecting using defaults");
                menu_line(1, "");
                menu_finished = true;
                break;
            case MENU_CUSTOM:
                menu_custom();
                break;
            case MENU_CUSTOM_SERVER:
                character[0] = (uint8_t) (positions % 10 + 48);
                char temp = (positions % 10 + 48);
                menu_line(0, "Number: %c %c >%c< %c %c", temp-2, temp-1, temp, temp+1, temp+2);
                menu_line(1, name);

                if (button_enter()) {
                    name[name_pos] = character[0];
                    name_pos++;
                }
                if (button_shift()) {
                    name[name_pos] = '.';
                    name_pos++;
                }
                if (button_back()) {
                    if(name_pos > 0) {
                        name_pos--;
                        name[name_pos] = ' ';
                    }
                }

                if (button_confirm()) {
                    switch (esp_param) {
                        case PARAM_IP:
                            strcpy(&server_ip, name);
                            break;
                        case PARAM_PORT:
                            strcpy(&server_port, name);
                            break;
                    }
                    current_menu = MENU_CUSTOM;
                }
                break;
            case MENU_CUSTOM_WIFI:
                character[0] = (uint8_t) ((positions % 26 + 65 + shift * 32));
                char temp2 = (positions % 26 + 65 + shift * 32);
                menu_line(0, "Letter: %c %c >%c< %c %c  %d", temp2-2, temp2-1, temp2, temp2+1, temp2+2, name_pos);
                menu_line(1, name);

                if (button_enter()) {
                    name[name_pos] = character[0];
                    name_pos++;
                }
                if (button_shift()) {
                    shift = !shift;
                }
                if (button_back()) {
                    if(name_pos > 0) {
                        name_pos--;
                        name[name_pos] = ' ';
                    }
                }

                if (button_confirm()) {
                    switch (esp_param) {
                        case PARAM_NAME:
                            strcpy(&wifi_name, name);
                            break;
                        case PARAM_PASSWORD:
                            strcpy(&wifi_password, name);
                            break;
                    }
                    current_menu = MENU_CUSTOM;
                }
                break;
        }
    }
}

void show_containers(const struct container * const containers, const uint8_t * const size) {
    uint8_t prev_i = 21;
    bool show_details = false;
    bool show_containers_finished = false;

    while(!show_containers_finished) {
        pulse_count = (uint8_t) TIM1->CNT;
        positions = (uint8_t) (pulse_count / 4);
        uint8_t i = positions % *size;
            if (!show_details) {
                menu_line(0, (uint8_t *) "%s", containers[i].name);
                menu_line(1, (uint8_t *) "%-11s  %11s", containers[i].state, containers[i].status);
            } else {
                menu_line(0, (uint8_t *) "%s", containers[i].name);
                menu_line(1, (uint8_t *) "%s", containers[i].image);
            }
        if (button_enter()) {
            show_details = !show_details;
        }
    }
}
