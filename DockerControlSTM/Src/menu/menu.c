#include <stdarg.h>
#include "menu/menu.h"
#include "main.h"

uint8_t esp_param = PARAM_IP;


// lcd
char menu_first_line[48];
char menu_second_line[48];

// encoder
volatile uint8_t pulse_count;
volatile uint8_t positions;

// system data
extern char server_ip;
extern char server_port;
extern char wifi_name;
extern char wifi_password;

// huart
extern UART_HandleTypeDef huart3;

// dc
extern enum DC_COMMAND_ENUM cmd;
extern uint8_t dc_alert;
extern uint8_t dc_update;

// esp
extern uint8_t esp_packet_received;

// menu
uint8_t menu_finished = 0;
uint8_t current_main_menu = MAIN_START;

// buttons
volatile uint8_t enter_pressed = 0; //encoder
volatile uint8_t confirm_pressed = 0; //green
volatile uint8_t back_pressed = 0; //white
volatile uint8_t shift_pressed = 0; //blue
uint8_t shift = 0;

int button_enter() {
    if(enter_pressed) {
        enter_pressed = 0;
        return 1;
    }
    else{
        return 0;
    }
}

int button_shift() {
    if(shift_pressed) {
        shift_pressed = 0;
        return 1;
    }
    else{
        return 0;
    }
}

int button_back() {
    if(back_pressed) {
        back_pressed = 0;
        return 1;
    }
    else{
        return 0;
    }
}

int button_confirm() {
    if(confirm_pressed) {
        confirm_pressed = 0;
        return 1;
    }
    else{
        return 0;
    }
}

static inline const uint8_t menu_condition(const uint8_t condition) {
    return condition && !dc_update && !esp_packet_received;
}

void update_screen() {
    TM_HD44780_Clear();
    TM_HD44780_Puts(0, 0, menu_first_line);
    TM_HD44780_Puts(0, 1, menu_second_line);
}

void menu_line(uint8_t line, char *format, ...) {
    char buffer[50];

    va_list args;
    va_start(args, format);
    vsprintf(buffer, format, args);
    va_end(args);
    if(line == 0) {
        if (strcmp(menu_first_line, buffer) != 0) {
            strxfrm(menu_first_line, buffer, 24);
            update_screen();
        }
    }
    else if (line == 1) {
        if (strcmp(menu_second_line, buffer) != 0) {
            strxfrm(menu_second_line, buffer, 24);
            update_screen();
        }
    }
}

void connection_menu_custom(uint8_t * const current_menu) {
    switch (positions % 5) {
        case 0:
            menu_line(0, ">Server IP: %s", &server_ip);
            menu_line(1, " Server Port: %s", &server_port);
            if (button_enter()) {
                esp_param = PARAM_IP;
                *current_menu = MENU_CUSTOM_SERVER;

            }
            break;
        case 1:
            menu_line(0, " Server IP: %s", &server_ip);
            menu_line(1, ">Server Port: %s", &server_port);
            if (button_enter()) {
                esp_param = PARAM_PORT;
                *current_menu = MENU_CUSTOM_SERVER;
            }
            break;
        case 2:
            menu_line(0, ">Name: %s", &wifi_name);
            menu_line(1, " Pass: %s", &wifi_password);
            if (button_enter()) {
                esp_param = PARAM_NAME;
                *current_menu = MENU_CUSTOM_WIFI;
            }
            break;
        case 3:
            menu_line(0, " Name: %s", &wifi_name);
            menu_line(1, ">Pass: %s", &wifi_password);
            if (button_enter()) {
                esp_param = PARAM_PASSWORD;
                *current_menu = MENU_CUSTOM_WIFI;
            }
            break;
        case 4:
            menu_line(0, ">Connect");
            menu_line(1, "");
            if (button_enter()) {
                menu_line(0, "Connecting...");
                menu_line(1, "");
                menu_finished = 1;
            }
        default:
            break;
    }
}

void connection_menu_server(uint8_t * const current_menu) {

    uint8_t name[48];
    volatile uint8_t name_pos = 0;
    uint8_t character[1] = {0};
    char temp;
    for(int i=0; i<48; i++) {
        name[i] = 0;
    }

    while(!button_confirm()) {
        pulse_count = (uint8_t) TIM1->CNT;
        positions = (uint8_t) (pulse_count / 4);
        character[0] = (uint8_t) (positions % 10 + 48);
        temp = (positions % 10 + 48);

        menu_line(0, "Number: %c %c >%c< %c %c", temp - 2, temp - 1, temp, temp + 1, temp + 2);
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
            if (name_pos > 0) {
                name_pos--;
                name[name_pos] = ' ';
            }
        }
    }
    switch (esp_param) {
        case PARAM_IP:
            strcpy(&server_ip, name);
            break;
        case PARAM_PORT:
            strcpy(&server_port, name);
            break;
    }
    *current_menu = MENU_CUSTOM;
}

void connection_menu_wifi(uint8_t * const current_menu) {
    uint8_t name[48];
    volatile uint8_t name_pos = 0;
    uint8_t character[1] = {0};
    char temp2;
    for(int i=0; i<48; i++) {
        name[i] = 0;
    }

    while(!button_confirm()) {
        pulse_count = (uint8_t) TIM1->CNT;
        positions = (uint8_t) (pulse_count / 4);
        character[0] = (uint8_t) ((positions % 26 + 65 + shift * 32));
        temp2 = (positions % 26 + 65 + shift * 32);

        menu_line(0, "Letter: %c %c >%c< %c %c  %d", temp2 - 2, temp2 - 1, temp2, temp2 + 1, temp2 + 2, name_pos);
        menu_line(1, name);

        if (button_enter()) {
            name[name_pos] = character[0];
            name_pos++;
        }
        if (button_shift()) {
            shift = !shift;
        }
        if (button_back()) {
            if (name_pos > 0) {
                name_pos--;
                name[name_pos] = ' ';
            }
        }
    }
    switch (esp_param) {
        case PARAM_NAME:
            strcpy(&wifi_name, name);
            break;
        case PARAM_PASSWORD:
            strcpy(&wifi_password, name);
            break;
    }
    *current_menu = MENU_CUSTOM;
}

void connection_menu_start(uint8_t * const current_menu) {
    switch (positions % 2) {
        case 0:
            menu_line(0, ">Default");
            menu_line(1, " Custom");
            if (button_enter()) {
                *current_menu = MENU_DEFAULT;
            }
            break;
        case 1:
            menu_line(0, " Default");
            menu_line(1, ">Custom");
            if (button_enter()) {
                *current_menu = MENU_CUSTOM;
            }
            break;
        default:
            break;
    }
}

void connection_menu() {
    util_log("connection_menu begin");
    uint8_t current_menu = MENU_START;

    while(!menu_finished) {
        pulse_count = (uint8_t) TIM1->CNT;
        positions = (uint8_t) (pulse_count / 4);
        switch (current_menu) {
            case MENU_START:
                connection_menu_start(&current_menu);
                break;
            case MENU_DEFAULT:
                menu_line(0, "Connecting...");
                menu_line(1, "");
                menu_finished = 1;
                break;
            case MENU_CUSTOM:
                connection_menu_custom(&current_menu);
                break;
            case MENU_CUSTOM_SERVER:
                connection_menu_server(&current_menu);
                break;
            case MENU_CUSTOM_WIFI:
                connection_menu_wifi(&current_menu);
                break;
            default:
                break;
        }
    }
}

void main_menu_start(uint8_t * const current_menu) {
    switch (positions % 4) {
        case 0:
            menu_line(0, ">Containers");
            menu_line(1, " Images");
            if (button_enter()) {
                *current_menu = MAIN_CONTAINERS;
            }
            break;
        case 1:
            menu_line(0, ">Images");
            menu_line(1, " Alerts");
            if (button_enter()) {
                *current_menu = MAIN_IMAGES;
            }
            break;
        case 2:
            menu_line(0, ">Alerts");
            menu_line(1, " Stats");
            if (button_enter()) {
                *current_menu = MAIN_ALERTS;
            }
            break;
        case 3:
            menu_line(0, ">Stats");
            menu_line(1, " Containers");
            if (button_enter()) {
                *current_menu = MAIN_STATS;
            }
            break;
        default:
            break;
    }
}

const uint8_t main_menu_container_action(const uint8_t * const container_index) {
    while(menu_condition(!button_enter())) {
        pulse_count = (uint8_t) TIM1->CNT;
        positions = (uint8_t) (pulse_count / 4);
        uint8_t i = positions % 4;
        switch(i) {
            case 0:
                cmd = CSTR;
                menu_line(1, ">start restart stop del");
                break;
            case 1:
                cmd = CRST;
                menu_line(1, " start>restart stop del");
                break;
            case 2:
                cmd = CSTP;
                menu_line(1, " start restart>stop del");
                break;
            case 3:
                cmd = CRMV;
                menu_line(1, " start restart stop>del");
                break;
            default:
                break;
        }

        if (button_back()) return 0;
    }
    dc_add_data_container(container_index);
    dc_set_ready();
    return 1;
}

uint8_t main_menu_containers(uint8_t * const current_menu, const struct container * const containers, const uint8_t * const size) {
    uint8_t show_details = 0;
    uint8_t show_containers_finished = 0;
    uint8_t action_performed = 0;

    while(menu_condition(!show_containers_finished)) {
        pulse_count = (uint8_t) TIM1->CNT;
        positions = (uint8_t) (pulse_count / 4);
        uint8_t i = positions % *size;

        if (!show_details) {
            menu_line(0, "%s", containers[i].name);
            menu_line(1, "%-11s  %11s", containers[i].state, containers[i].status);
        } else {
            menu_line(0, "%s", containers[i].name);
            menu_line(1, "%s", containers[i].image);
        }

        if (button_enter()) {
            show_details = !show_details;
        }
        if (button_shift()) {
            action_performed = main_menu_container_action(&i);
            if (action_performed) {
                *current_menu = MAIN_START;
                show_containers_finished = 1;
            }
        }
        if (button_back()) {
            *current_menu = MAIN_START;
            show_containers_finished = 1;
        }
        if (button_confirm()) {
            *current_menu = MAIN_START;
            show_containers_finished = 1;
        }
    }
    return action_performed;
}

const uint8_t main_menu_images(uint8_t * const current_menu, const image * const images, const uint8_t * const size) {
    uint8_t show_images_finished = 0;
    uint8_t action_performed = 0;

    while(menu_condition(!show_images_finished)) {
        pulse_count = (uint8_t) TIM1->CNT;
        positions = (uint8_t) (pulse_count / 4);
        uint8_t i = positions % *size;

        menu_line(0, "%s", images[i]);
        menu_line(1, ">create<");

        if (button_back()) {
            show_images_finished = 1;
            *current_menu = MAIN_START;
        }
        if (button_confirm()) {
            cmd = CCRT;
            dc_add_data_image(&i);
            dc_set_ready();
            *current_menu = MAIN_START;
            show_images_finished = 1;
            action_performed = 1;
        }
    }

    return action_performed;
}

void main_menu_alerts(uint8_t * const current_menu, const alert * const _alert, const uint8_t * const alerts_size) {
    uint8_t show_alerts_finished = 0;

    while(menu_condition(!show_alerts_finished)) {
        pulse_count = (uint8_t) TIM1->CNT;
        positions = (uint8_t) (pulse_count / 4);
        uint8_t i = positions % *alerts_size;
        if(*alerts_size == 0){
            menu_line(0, "No alerts");
            menu_line(1," ");
        } else {
            menu_line(0, "%d: %.20s", i, _alert[i]);
            if(i+1 < *alerts_size) {
                menu_line(1, "%d: %.20s", i + 1, _alert[i + 1]);
            }
            else{
                menu_line(1, "%d: %.20s", 0, _alert[0]);
            }
        }
        if (button_back()) {
            show_alerts_finished = 1;
            *current_menu = MAIN_START;
        }
    }

    dc_clear_alerts();

}

void main_menu_stats(uint8_t * const current_menu, const struct stats * const stats) {
    uint8_t show_stats_finished = 0;
    uint8_t stats_containers = 0;
    while(menu_condition(!show_stats_finished)) {

        if(stats_containers) {
            menu_line(0, "Active: %3s Paused: %3s", stats->active_containers, stats->paused_containers);
            menu_line(1, "Stopped: %3s", stats->stopped_containers);
        }
        else{
            menu_line(0, "Images: %5s CPU: %3s", stats->images, stats->cpu);
            menu_line(1, "Memory: %7s", stats->memory);
        }

        if(button_enter()){
            stats_containers = !stats_containers;
        }
        if (button_back()) {
            show_stats_finished = 1;
            *current_menu = MAIN_START;
        }
    }
}

inline void main_menu(
    const struct container * const containers, const uint8_t * const containers_size,
    const image * const images, const uint8_t * const images_size,
    const struct stats * const _stats,
    const alert * const _alert, const uint8_t * const alerts_size
) {
    util_log("main menu begin");
    menu_finished = 0;
    while (menu_condition(!menu_finished)) {
        pulse_count = (uint8_t) TIM1->CNT;
        positions = (uint8_t) (pulse_count / 4);
        switch (current_main_menu) {
            case MAIN_START:
                main_menu_start(&current_main_menu);
                break;
            case MAIN_CONTAINERS:
                menu_finished = main_menu_containers(&current_main_menu, containers, containers_size);
                break;
            case MAIN_IMAGES:
                menu_finished = main_menu_images(&current_main_menu, images, images_size);
                break;
            case MAIN_ALERTS:
                main_menu_alerts(&current_main_menu, _alert, alerts_size);
                break;
            case MAIN_STATS:
                main_menu_stats(&current_main_menu, _stats);
                break;
            default:
                break;
        }
    }
    util_log("main menu end");
}
