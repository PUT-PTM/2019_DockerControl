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

void temp_confirm() {}

menu show_menu(){

    char temp1[50];
    char temp2[50];
    char temp3[50];
    char temp4[50];


    switch (current_menu) {
        case MENU_START:
            switch (positions % 2) {
                case 0:
                    strcpy(menu_first_line, ">Default");
                    strcpy(menu_second_line, " Custom");
//          menu_first_line = ">Default";
//          menu_second_line = " Custom";
                    if (button_enter()) {
                        current_menu = MENU_DEFAULT;
                    }
                    break;
                case 1:
                    strcpy(menu_first_line, " Default");
                    strcpy(menu_second_line, ">Custom");
//          menu_first_line = " Default";
//          menu_second_line = ">Custom";
                    if (button_enter()) {
                        current_menu = MENU_CUSTOM;
                    }
                    break;
                default:break;
            }
            break;
        case MENU_DEFAULT:
//      menu_first_line = "Connecting using defaults";
            break;
        case MENU_CUSTOM:
            switch (positions % 4) {
                case 0:
                    strcpy(menu_first_line, ">Server IP: ");
                    strcat(menu_first_line, server_ip);
                    strcpy(menu_second_line, " Server Port: ");
                    strcat(menu_second_line, server_port);
//          menu_first_line = ">Server IP";
//          menu_second_line = " Server port";
                    if (button_enter()) {
                        esp_param = PARAM_IP;
                        current_menu = MENU_CUSTOM_SERVER;

                    }
                    else if(button_confirm()){
                        temp_confirm();
                        current_menu = MENU_START;
                    }
                    break;
                case 1:
                    strcpy(menu_first_line, " Server IP: ");
                    strcat(menu_first_line, server_ip);
                    strcpy(menu_second_line, ">Server Port: ");
                    strcat(menu_second_line, server_port);
//          menu_first_line = ">Server port";
//          menu_second_line = " WiFi Name";
                    if (button_enter()) {
                        esp_param = PARAM_PORT;
                        current_menu = MENU_CUSTOM_SERVER;
                    }
                    else if(button_confirm()){
                        temp_confirm();
                        current_menu = MENU_START;
                    }
                    break;
                case 2:
                    strcpy(menu_first_line, ">WiFi Name: ");
                    strcat(menu_first_line, wifi_name);
                    strcpy(menu_second_line, " WiFi Password: ");
                    strcat(menu_second_line, wifi_password);
//          menu_first_line = ">WiFi Name";
//          menu_second_line = " WiFi Password";
                    if (button_enter()) {
                        esp_param = PARAM_NAME;
                        current_menu = MENU_CUSTOM_WIFI;
                    }
                    else if(button_confirm()){
                        temp_confirm();
                        current_menu = MENU_START;
                    }
                    break;
                case 3:
                    strcpy(menu_first_line, " WiFi Name: ");
                    strcat(menu_first_line, wifi_name);
                    strcpy(menu_second_line, ">WiFi Password: ");
                    strcat(menu_second_line, wifi_password);
//          menu_first_line = " WiFi Name";
//          menu_second_line = ">WiFi Password";
                    if (button_enter()) {
                        esp_param = PARAM_PASSWORD;
                        current_menu = MENU_CUSTOM_WIFI;
                    }
                    else if(button_confirm()){
                        temp_confirm();
                        current_menu = MENU_START;
                    }
                    break;
                default:break;
            }
            break;
        case MENU_CUSTOM_SERVER:
            character[0] = (uint8_t) (positions%10 + 48);
            strcpy(menu_first_line, "Number: ");
            strcat(menu_first_line, ">");
            strcat(menu_first_line, (const char *) character);
            strcat(menu_first_line, "<");
            strcat(menu_first_line, " ");
            strcpy(menu_second_line, name);
            if(button_enter()){
                name[name_pos] = character[0];
                name_pos++;
            }
            if(shift == 1){
                shift = 0;
                name[name_pos] = '.';
                name_pos++;
            }
            if(back_pressed == 1){
                back_pressed = 0;
                name_pos--;
                name[name_pos] = ' ';
            }
            if(button_confirm()){
                switch(esp_param){
                    case PARAM_IP:
                        strcpy(temp1,name);
                        server_ip = temp1;
                        break;
                    case PARAM_PORT:
                        strcpy(temp2, name);
                        server_port = temp2;
                        break;
                }
                current_menu = MENU_CUSTOM;
            }
            shift = 0;
            break;
        case MENU_CUSTOM_WIFI:
            character[0] = (uint8_t) ((positions + 65 + shift*32));
            strcpy(menu_first_line, "Letter: ");
            strcat(menu_first_line, ">");
            strcat(menu_first_line, (const char *) character);
            strcat(menu_first_line, "<");
            strcpy(menu_second_line, name);
            if(button_enter()){
                name[name_pos] = character[0];
                name_pos++;
            }
            if(shift == 1){
                shift = 0;
                name[name_pos] = '.';
                name_pos++;
            }
            if(back_pressed == 1){
                back_pressed = 0;
                name_pos--;
                name[name_pos] = ' ';
            }
            if(button_confirm()){
                switch(esp_param){
                    case PARAM_NAME:
                        strcpy(temp3, name);
                        wifi_name = temp3;
                        break;
                    case PARAM_PASSWORD:
                        strcpy(temp4, name);
                        wifi_password = temp4;
                        break;
                }
                current_menu = MENU_CUSTOM;
            }
            shift = 0;
            break;
    }
}