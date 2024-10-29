#pragma once
#include "TCA9554PWR.h"

#define LCD_Backlight_PIN   5
// Backlight   ledChannel：PWM Channe 使用的LEDC通道   dutyfactor:与Resolution相关，Resolution=8对应0~255占空比（高电平占比）可选
#define PWM_Channel     1       // PWM Channel   
#define Frequency       20000   // PWM frequencyconst         与分辨率相关，分辨率越高频率就要设置的越低
#define Resolution      10       // PWM resolution ratio     MAX:13
#define Dutyfactor      500     // PWM Dutyfactor       0~（2 的 Resolution 次方） 当前为2的10次方 = 2 ** 10 = 1024 即 0~1024

#define EXAMPLE_LCD_WIDTH                   (360)
#define EXAMPLE_LCD_HEIGHT                  (360)
#define EXAMPLE_LCD_COLOR_BITS              (16)

#define ESP_PANEL_HOST_SPI_ID_DEFAULT       (SPI2_HOST)
#define ESP_PANEL_LCD_SPI_MODE              (0)                   // 0/1/2/3, typically set to 0
#define ESP_PANEL_LCD_SPI_CLK_HZ            (80 * 1000 * 1000)    // Should be an integer divisor of 80M, typically set to 40M
#define ESP_PANEL_LCD_SPI_TRANS_QUEUE_SZ    (10)                  // Typically set to 10
#define ESP_PANEL_LCD_SPI_CMD_BITS          (32)                  // Typically set to 32
#define ESP_PANEL_LCD_SPI_PARAM_BITS        (8)                   // Typically set to 8

#define ESP_PANEL_LCD_SPI_IO_TE             (18)
#define ESP_PANEL_LCD_SPI_IO_SCK            (40)
#define ESP_PANEL_LCD_SPI_IO_DATA0          (46)
#define ESP_PANEL_LCD_SPI_IO_DATA1          (45)
#define ESP_PANEL_LCD_SPI_IO_DATA2          (42)
#define ESP_PANEL_LCD_SPI_IO_DATA3          (41)
#define ESP_PANEL_LCD_SPI_IO_CS             (21)
#define EXAMPLE_LCD_PIN_NUM_RST             (-1)    // EXIO2
#define EXAMPLE_LCD_PIN_NUM_BK_LIGHT        (-1)

#define EXAMPLE_LCD_BK_LIGHT_ON_LEVEL       (1)
#define EXAMPLE_LCD_BK_LIGHT_OFF_LEVEL !EXAMPLE_LCD_BK_LIGHT_ON_LEVEL

#define ESP_PANEL_HOST_SPI_MAX_TRANSFER_SIZE   (2048)
void ST77916_Init();

void LCD_Init();
void LCD_addWindow(uint16_t Xstart, uint16_t Ystart, uint16_t Xend, uint16_t Yend,uint16_t* color);

// backlight
void Backlight_Init();
void Set_Backlight(uint8_t Light);  