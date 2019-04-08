#include "main.h"
#include <string.h>
#include "dc/dc.h"

enum DC_COMMAND_ENUM cmd = ACKN;

enum DC_COMMAND_ENUM dc_resolve_cmd(const uint8_t * const body) {
    char command[5];
    strncpy(command, (const char *) body, 4);
    command[4] = '\0';

    enum DC_COMMAND_ENUM i;
    for (i = ACKN; i <= ALRT; i++)
        if (strcmp(DC_COMMAND_STRING[i], command) == 0) return i;

    return ERRR;
}