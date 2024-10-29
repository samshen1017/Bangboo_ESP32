#pragma once
#include <SerialTerminal.hpp>

extern maschinendeck::SerialTerminal *term_global;

void hello(String opts)
{
    maschinendeck::Pair<String, String> operands = maschinendeck::SerialTerminal::ParseCommand(opts);
    Serial.println("Hello World.");
}

void querymem(String opts)
{
    maschinendeck::Pair<String, String> operands = maschinendeck::SerialTerminal::ParseCommand(opts);
    // 获取 PSRAM 的大小（以字节为单位）
    uint32_t psramSize = ESP.getPsramSize();
    Serial.printf("PSRAM Size: %u bytes\r\n", psramSize);
    // 获取 PSRAM 剩余内存
    uint32_t FreePSRAM = ESP.getFreePsram();
    Serial.printf("Free PSRAM: %u bytes\r\n", FreePSRAM);

    // 获取剩余内存
    uint32_t freeMemory = ESP.getFreeHeap();
    Serial.printf("Free Memory: %u bytes\r\n", freeMemory);
    // 获取总内存
    uint32_t Totalheap = ESP.getHeapSize();
    Serial.printf("Total heap: %u bytes\r\n", Totalheap);
}

void commands_init()
{
    term_global->add("hello", &hello, "HelloWorld");
    term_global->add("querymem", &querymem, "query memory");
}