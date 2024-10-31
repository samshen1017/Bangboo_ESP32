#pragma once
#include <Arduino.h>
#include <SerialTerminal.hpp>
#include <task.h>

class TaskList
{
public:
  void init(void);

private:
  TaskHandle_t term_xHandle;
  TaskHandle_t lvgl_xHandle;
  maschinendeck::SerialTerminal *term;
  void terminal_init(void);
  void lvgl_init(void);
  void sdcard_init(void);
  void audio_init(void);
};
