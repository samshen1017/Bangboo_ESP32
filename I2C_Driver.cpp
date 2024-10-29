#include "I2C_Driver.h"


void I2C_Init(void) {
  Wire1.begin( I2C_SDA_PIN, I2C_SCL_PIN);                       
}
