#pragma once
#include <Arduino.h>
#include <SerialTerminal.hpp>
#include <task.h>

class TaskList
{
public:
  void init();

private:
  TaskHandle_t term_xHandle;
  TaskHandle_t lvgl_xHandle;
  maschinendeck::SerialTerminal *term;
  void terminal_init();
  void lvgl_init();
};
