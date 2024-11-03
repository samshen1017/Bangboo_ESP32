#pragma once

// If your target is limited in memory remove this macro to save 10K RAM
// #define EIDSP_QUANTIZE_FILTERBANK   0

/*
 ** NOTE: If you run into TFLite arena allocation issue.
 **
 ** This may be due to may dynamic memory fragmentation.
 ** Try defining "-DEI_CLASSIFIER_ALLOCATION_STATIC" in boards.local.txt (create
 ** if it doesn't exist) and copy this file to
 ** `<ARDUINO_CORE_INSTALL_PATH>/arduino/hardware/<mbed_core>/<core_version>/`.
 **
 ** See
 ** (https://support.arduino.cc/hc/en-us/articles/360012076960-Where-are-the-installed-cores-located-)
 ** to find where Arduino installs cores on your machine.
 **
 ** If the problem persists then there's not enough memory for this model and application.
 */

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define I2S_PIN_BCK 15
#define I2S_PIN_WS 2
#define I2S_PIN_DOUT -1
#define I2S_PIN_DIN 39

void EDGEImpulse_init(void);
void EDGEImpulse_Enable(bool en);
