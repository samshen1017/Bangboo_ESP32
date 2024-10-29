#include "taskList.h"
#include <ulog.h>
#include <SerialTerminal.hpp>
#include "commands.hpp"

#include "Display_ST77916.h"
#include "LVGL_Driver.h"
#include "LVGL_Example.h"

/*-------------------- ulog --------------------*/
unsigned long get_timestamp()
{
    return millis();
}

void my_console_logger(ulog_level_t severity, char *msg)
{
    Serial.printf("%lu [%s]: %s\r\n",
                  get_timestamp(), // user defined function
                  ulog_level_name(severity),
                  msg);
}

/*-------------------- init --------------------*/
void TaskList::init()
{
    this->terminal_init();
    ULOG_INIT();
    ULOG_SUBSCRIBE(my_console_logger, ULOG_TRACE_LEVEL);
    ULOG_INFO("ULOG init.");
    this->lvgl_init();
    ULOG_INFO("System init success.");
}

/*-------------------- LVGL --------------------*/

// LVGL 的FreeRTOS任务
void LVGL_Task(void *parameter)
{
    Lvgl_BangbooFace();
    while (1)
    {
        Lvgl_Loop();
        vTaskDelay(pdMS_TO_TICKS(3));
    }
}

void TaskList::lvgl_init()
{
    ULOG_INFO("LVGL Init.");
    Backlight_Init();
    Set_Backlight(70);  //0~100
    LCD_Init();
    Lvgl_Init();
    // 创建任务
    xTaskCreatePinnedToCore(
        LVGL_Task,           // 任务函数
        "LVGL_Task",         // 任务名称
        10240,               // 栈大小（字节）
        NULL,                // 传递给任务函数的参数
        3,                   // 优先级（数字越高优先级越高）
        &this->lvgl_xHandle, // 任务句柄
        0                    // 核心编号（0或1）
    );
}

/*-------------------- SerialTerminal --------------------*/

maschinendeck::SerialTerminal *term_global;

// Terminal 的FreeRTOS任务
void TerminalTask(void *parameter)
{
    while (1)
    {
        term_global->loop();
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void TaskList::terminal_init()
{
    this->term = new maschinendeck::SerialTerminal(115200);
    if (this->term == NULL)
    {
        ULOG_ERROR("MEM IS FULL.");
        return;
    }
    term_global = this->term;
    commands_init();
    // 创建任务
    xTaskCreatePinnedToCore(
        TerminalTask,        // 任务函数
        "TerminalTask",      // 任务名称
        8192,                // 栈大小（字节）
        NULL,                // 传递给任务函数的参数
        3,                   // 优先级（数字越高优先级越高）
        &this->term_xHandle, // 任务句柄
        0                    // 核心编号（0或1）
    );
}