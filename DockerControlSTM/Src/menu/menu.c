#include <hd44780.h>
#include <stdarg.h>
#include "menu/menu.h"
#include "main.h"


int button_enter(){
    if(enter_pressed){
        enter_pressed = 0;
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

void menu_line(int line, char *format, ...) {
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

    struct container_mock containers_mock = {
            1,
            "Docker Nothing",
            "vrqe3WGRWEFV2Q",
            "Stopped",
            "idk"
    };

    while(!menu_finished) {
        pulse_count = (uint8_t) TIM1->CNT;
        positions = (uint8_t) (pulse_count / 4);

        switch (current_menu) {
            case MENU_START:
                menu_start();
                break;
            case MENU_DEFAULT:
                menu_line(0, "Connecting using defaults");
                for(int x=0; x<10000000;x++){} //TODO: Remove
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
                if (shift == 1) {
                    shift = 0;
                    name[name_pos] = '.';
                    name_pos++;
                }
                if (back_pressed == 1) {
                    back_pressed = 0;
                    name_pos--;
                    name[name_pos] = ' ';
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
                shift = 0;
                break;
            case MENU_CUSTOM_WIFI:
                character[0] = (uint8_t) ((positions + 65 + shift * 32));
                char temp2 = (positions % 26 + 65);
                menu_line(0, "Letter: %c %c >%c< %c %c", temp2-2, temp2-1, temp2, temp2+1, temp2+2);
                menu_line(1, name);

                if (button_enter()) {
                    name[name_pos] = character[0];
                    name_pos++;
                }
                if (shift == 1) {
                    shift = 0;
                    name[name_pos] = '.';
                    name_pos++;
                }
                if (back_pressed == 1) {
                    back_pressed = 0;
                    name_pos--;
                    name[name_pos] = ' ';
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
                shift = 0;
                break;
        }
    }
}


void show_containers() {
    struct container_mock containers_mock[] = {
            {
                    1,
                    "Docker Nothing",
                    "nothing:latest",
                    "created",
                    "idk"
            },
            {
                    2,
                    "LED Manager",
                    "nothing:latest",
                    "running",
                    "idk"
            },
            {
                    3,
                    "LCD",
                    "nothing:latest",
                    "dead",
                    "idk"
            },
            {
                    4,
                    "STM Docker",
                    "nothing:latest",
                    "running",
                    "idk"
            },
            {
                    5,
                    "Docker Nothing",
                    "nothing:latest",
                    "paused",
                    "idk"
            }
    };
    int containers_mock_size = 5;
//struct container
//    id[65];
//    name[51];
//    image[51];
//    state[11];
//    status[11];
//};
    while(!show_containers_finished){
        pulse_count = (uint8_t) TIM1->CNT;
        positions = (uint8_t) (pulse_count / 4);
        int index = positions % containers_mock_size;
        menu_line(0,"%d %s", containers_mock[index].id, containers_mock[index].name);
        menu_line(1,"%s, %s, %s", containers_mock[index].image, containers_mock[index].state, containers_mock[index].status);
    }

}