#pragma once
#include <Arduino.h>

#define I2S_PIN_BCK 15
#define I2S_PIN_WS 2
#define I2S_PIN_DOUT -1
#define I2S_PIN_DIN 39

enum RecordState
{
    Record_Standby,
    Record_Busy,
    Record_Finish
};

void MIC_Init(void);

size_t MIC_recordRaw(void *simpleBuffer, size_t bytes_to_read);

void MIC_RecordStart(uint32_t seconds);

void MIC_ForceChangeState(uint8_t state);
// void MIC_RecordStop(void);