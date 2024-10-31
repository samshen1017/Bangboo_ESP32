#include "I2C_Driver.h"
#include "TCA9554PWR.h"
#include "tasklist.h"
#include <lvgl.h>

void setup()
{
    // hardware init
    I2C_Init();
    TCA9554PWR_Init(0x00);

    // task init
    TaskList tlist;
    tlist.init();
}

void loop()
{
    delay(1000);
}
