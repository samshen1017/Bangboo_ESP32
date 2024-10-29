#pragma once

#include <lvgl.h>
#include "lv_conf.h"
#include <esp_heap_caps.h>
#include "Display_ST77916.h"

#define LCD_WIDTH EXAMPLE_LCD_WIDTH
#define LCD_HEIGHT EXAMPLE_LCD_HEIGHT
#define LVGL_BUF_LEN (LCD_WIDTH * LCD_HEIGHT / 20)

#define EXAMPLE_LVGL_TICK_PERIOD_MS 33

// void Lvgl_print(const char * buf);
void Lvgl_Display_LCD(lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p); // Displays LVGL content on the LCD.    This function implements associating LVGL data to the LCD screen
void example_increase_lvgl_tick(void *arg);

void Lvgl_Init(void);
void Lvgl_Loop(void);
