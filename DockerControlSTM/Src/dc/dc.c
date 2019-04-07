#include "main.h"
#include <string.h>
#include "dc/dc.h"

enum DC_COMMAND_ENUM cmd = ACK;

enum DC_COMMAND_ENUM dc_resolve_cmd(const uint8_t * const body) {

    const uint8_t size = cmd_size(body);
    char command[10];
    strncpy(command, (const char *) body, size);
    command[size] = '\0';

    enum DC_COMMAND_ENUM i;
    for (i = ACK; i <= ALRT; i++)
        if (strcmp(DC_COMMAND_STRING[i], command) == 0) break;

    free(command);
    return i;
}

const uint8_t cmd_size(const uint8_t * const body) {
    uint8_t i = 0;
    for (i = 0; body[i] != PACKET_DELIMITER; i++);
    return i;
}