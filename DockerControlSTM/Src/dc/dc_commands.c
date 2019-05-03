#include "main.h"
#include <string.h>
#include "dc/dc.h"
#include "dc/dc_commands.h"

extern char dc_session_id[];

void dc_cmd_ready(const uint8_t * const packet_header) {
    strncpy(dc_session_id, (const char *) packet_header + 1, 3);
    dc_session_id[3] = '\0';
    util_log(dc_session_id);
}