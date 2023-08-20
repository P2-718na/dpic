#ifndef DPIC_ESP_NOW_H
#define DPIC_ESP_NOW_H
#include <string.h>
#include "state.h"

extern State state;

void OnDataRecv(const uint8_t * maxsc, const uint8_t *incomingData, int len);

void setupEspNow();

#endif