#pragma once
#include "Arduino.h"

// Digital I/O used
#define I2S_DOUT 47
#define I2S_BCLK 48
#define I2S_LRC 38 // I2S_WS

#define EXAMPLE_Audio_TICK_PERIOD_MS 20

void Audio_Init();
void Audio_Play(String path);
uint16_t Audio_VUlevel(void);
uint8_t Audio_Volume(void);
void Audio_setVolume(uint8_t vol);
