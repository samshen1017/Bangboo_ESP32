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
    Serial.printf("PSRAM Size: %lu bytes\r\n", psramSize);
    // 获取 PSRAM 剩余内存
    uint32_t FreePSRAM = ESP.getFreePsram();
    Serial.printf("Free PSRAM: %lu bytes\r\n", FreePSRAM);

    // 获取剩余内存
    uint32_t freeMemory = ESP.getFreeHeap();
    Serial.printf("Free Memory: %lu bytes\r\n", freeMemory);
    // 获取总内存
    uint32_t Totalheap = ESP.getHeapSize();
    Serial.printf("Total heap: %lu bytes\r\n", Totalheap);
}

/* ====================== FS Port ====================== */
#include "SD_Card.h"
static String current_path = "/";

void cd(String opts)
{
    maschinendeck::Pair<String, String> operands = maschinendeck::SerialTerminal::ParseCommand(opts);
    String temp_path;
    if (operands.first().equals(".."))
    {
        if (current_path.length() < 2)
        {
            Serial.println("Current directory in root");
            return;
        }
        current_path.remove(current_path.lastIndexOf('/'));
        current_path.remove(current_path.lastIndexOf('/') + 1, current_path.length() - current_path.lastIndexOf('/'));
        temp_path = current_path;
        temp_path.remove(current_path.lastIndexOf('/'));
        term_global->changePath(temp_path);
    }
    else
    {
        String temp_path = current_path + operands.first();
        File root = SD.open(temp_path);
        if (!root)
        {
            Serial.println("Failed to open directory");
            return;
        }
        if (!root.isDirectory())
        {
            Serial.println("Not a directory");
            return;
        }
        current_path = temp_path + "/";
        term_global->changePath(temp_path);
    }
}

void ls(String opts)
{
    maschinendeck::Pair<String, String> operands = maschinendeck::SerialTerminal::ParseCommand(opts);
    if (operands.first().length() <= 0)
    {
        listDir(SD, current_path.c_str(), 0);
    }
    else
    {
        listDir(SD, operands.first().c_str(), 0);
    }
}

void mkdir(String opts)
{
    maschinendeck::Pair<String, String> operands = maschinendeck::SerialTerminal::ParseCommand(opts);
    String path = current_path + operands.first();
    createDir(SD, path.c_str());
}

void rmdir(String opts)
{
    maschinendeck::Pair<String, String> operands = maschinendeck::SerialTerminal::ParseCommand(opts);
    String path = current_path + operands.first();
    removeDir(SD, path.c_str());
}

void cat(String opts)
{
    maschinendeck::Pair<String, String> operands = maschinendeck::SerialTerminal::ParseCommand(opts);
    String path = current_path + operands.first();
    readFile(SD, path.c_str());
}

void touch(String opts)
{
    maschinendeck::Pair<String, String> operands = maschinendeck::SerialTerminal::ParseCommand(opts);
    String path = current_path + operands.first();
    String message = operands.second();
    writeFile(SD, path.c_str(), message.c_str());
}

void mv(String opts)
{
    maschinendeck::Pair<String, String> operands = maschinendeck::SerialTerminal::ParseCommand(opts);
    String path_src = current_path + operands.first();
    String path_dest = current_path + operands.first();
    renameFile(SD, path_src.c_str(), path_dest.c_str());
}

void rm(String opts)
{
    maschinendeck::Pair<String, String> operands = maschinendeck::SerialTerminal::ParseCommand(opts);
    String path = current_path + operands.first();
    deleteFile(SD, path.c_str());
}

/* ====================== Audio Test ====================== */
#include "Audio_PCM5101.h"
void audio_play(String opts)
{
    maschinendeck::Pair<String, String> operands = maschinendeck::SerialTerminal::ParseCommand(opts);
    Audio_PlayByFS(operands.first());
}

void audio_setVolume(String opts)
{
    maschinendeck::Pair<String, String> operands = maschinendeck::SerialTerminal::ParseCommand(opts);
    uint8_t vol = operands.first().toInt();
    Audio_setVolume(vol);
}

void audio_state(String opts)
{
    maschinendeck::Pair<String, String> operands = maschinendeck::SerialTerminal::ParseCommand(opts);
    printf("Volume: %u\r\n", Audio_Volume());
    printf("VUlevel: %u\r\n", Audio_VUlevel());
}

void commands_init()
{
    term_global->add("hello", &hello, "HelloWorld");
    term_global->add("querymem", &querymem, "query memory");
    /*===================== FS Port =====================*/
    term_global->add("cd", &cd, "cd");
    term_global->add("ls", &ls, "ls");
    term_global->add("mkdir", &mkdir, "mkdir");
    term_global->add("rmdir", &rmdir, "rmdir");
    term_global->add("cat", &cat, "cat");
    term_global->add("touch", &touch, "touch");
    term_global->add("mv", &mv, "mv");
    term_global->add("rm", &rm, "rm");
    /*===================== Audio =====================*/
    term_global->add("audio-p", &audio_play, "Audio play MP3");
    term_global->add("audio-s", &audio_state, "Audio state");
    term_global->add("audio-vol", &audio_setVolume, "Audio set Volume");
}