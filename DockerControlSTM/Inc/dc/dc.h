#ifndef DOCKERCONTROLSTM_DC_H
#define DOCKERCONTROLSTM_DC_H

#include "dc_commands.h"

void dc_resolve_cmd(const uint8_t * body);

void dc_make_header();
void dc_make_body();

void dc_start_session(UART_HandleTypeDef * huart);

void dc_send(UART_HandleTypeDef * huart);

#endif //DOCKERCONTROLSTM_DC_H
