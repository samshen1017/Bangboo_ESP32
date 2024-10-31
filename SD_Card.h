#pragma once

#include "Arduino.h"
#include "SD.h"
#include "FS.h"

#define SD_CLK_PIN 14
#define SD_MOSI_PIN 17
#define SD_MISO_PIN 16

extern uint16_t SDCard_Size;
extern uint16_t Flash_Size;

void SD_Init();

void listDir(fs::FS &fs, const char *dirname, uint8_t levels);

void createDir(fs::FS &fs, const char *path);

void removeDir(fs::FS &fs, const char *path);

void readFile(fs::FS &fs, const char *path);

void writeFile(fs::FS &fs, const char *path, const char *message);

void appendFile(fs::FS &fs, const char *path, const char *message);

void renameFile(fs::FS &fs, const char *path1, const char *path2);

void deleteFile(fs::FS &fs, const char *path);
