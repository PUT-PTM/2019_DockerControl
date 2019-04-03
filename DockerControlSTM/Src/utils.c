#include "utils.h"
#include "usbd_cdc_if.h"

void transmit(const char * const msg) {
    CDC_Transmit_FS((uint8_t *) msg, (uint16_t) strlen(msg));
    HAL_Delay(5);
}

void util_log(const char * const msg) {
    static const char * const END = "\n";
    transmit(msg);
    transmit(END);
}
