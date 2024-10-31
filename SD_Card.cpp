#include "SD_Card.h"

#include "Arduino.h"
#include <cstring>
#include <esp_log.h>
#include <SPI.h>
#include "TCA9554PWR.h"

SPIClass sdSPI(HSPI);
uint16_t SDCard_Size = 0;
uint16_t Flash_Size = 0;

void SD_CS_EN()
{
  Set_EXIO(EXIO_PIN3, Low);
  vTaskDelay(pdMS_TO_TICKS(10));
}
void SD_CS_Dis()
{
  Set_EXIO(EXIO_PIN3, High);
  vTaskDelay(pdMS_TO_TICKS(10));
}
void SD_Init()
{
  sdSPI.setHwCs(false);
  sdSPI.setBitOrder(MSBFIRST);
  sdSPI.setDataMode(SPI_MODE0);
  sdSPI.setFrequency(4000000);
  sdSPI.begin(SD_CLK_PIN, SD_MISO_PIN, SD_MOSI_PIN);

  // SD
  SD_CS_EN();
  if (SD.begin(-1, sdSPI, 4000000, "/sd", 5, false))
  { // CS 使用外部 IO
    ESP_LOGI("", "SD card initialization successful!");
  }
  else
  {
    ESP_LOGI("", "Try format SD card!");
    if (SD.begin(-1, sdSPI, 4000000, "/sd", 5, true))
    {
      ESP_LOGI("", "SD card format success!");
    }
    else
    {
      ESP_LOGI("", "SD card initialization faield!");
    }
  }

  uint8_t cardType = SD.cardType();
  if (cardType == CARD_NONE)
  {
    ESP_LOGI("", "No SD card attached");
    return;
  }
  else
  {
    if (cardType == CARD_MMC)
    {
      ESP_LOGI("", "SD Card Type: MMC");
    }
    else if (cardType == CARD_SD)
    {
      ESP_LOGI("", "SD Card Type: SDSC");
    }
    else if (cardType == CARD_SDHC)
    {
      ESP_LOGI("", "SD Card Type: SDHC");
    }
    else
    {
      ESP_LOGI("", "SD Card Type: UNKNOWN");
    }

    uint64_t totalBytes = SD.totalBytes();
    uint64_t usedBytes = SD.usedBytes();
    SDCard_Size = totalBytes / (1024 * 1024);
    ESP_LOGI("", "Total space: %llu", totalBytes);
    ESP_LOGI("", "Used space: %llu", usedBytes);
    ESP_LOGI("", "Free space: %llu", totalBytes - usedBytes);
  }
}

void listDir(fs::FS &fs, const char *dirname, uint8_t levels)
{
  Serial.printf("Listing directory: %s\r\n", dirname);
  File root = fs.open(dirname);
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

  File file = root.openNextFile();
  while (file)
  {
    if (file.isDirectory())
    {
      Serial.print("  DIR : ");
      Serial.println(file.name());
      if (levels)
      {
        listDir(fs, file.path(), levels - 1);
      }
    }
    else
    {
      Serial.print("  FILE: ");
      Serial.print(file.name());
      Serial.print("  SIZE: ");
      Serial.println(file.size());
    }
    file = root.openNextFile();
  }
}

void createDir(fs::FS &fs, const char *path)
{
  Serial.printf("Creating Dir: %s\r\n", path);
  if (fs.mkdir(path))
  {
    Serial.println("Dir created");
  }
  else
  {
    Serial.println("mkdir failed");
  }
}

void removeDir(fs::FS &fs, const char *path)
{
  Serial.printf("Removing Dir: %s\r\n", path);
  if (fs.rmdir(path))
  {
    Serial.println("Dir removed");
  }
  else
  {
    Serial.println("rmdir failed");
  }
}

void readFile(fs::FS &fs, const char *path)
{
  Serial.printf("Reading file: %s\r\n", path);

  File file = fs.open(path);
  if (!file)
  {
    Serial.println("Failed to open file for reading");
    return;
  }
  Serial.print("Read from file: ");
  while (file.available())
  {
    Serial.write(file.read());
  }
  file.close();
}

void writeFile(fs::FS &fs, const char *path, const char *message) {
  Serial.printf("Writing file: %s\r\n", path);

  File file = fs.open(path, FILE_WRITE);
  if (!file) {
    Serial.println("Failed to open file for writing");
    return;
  }
  if (file.print(message)) {
    Serial.println("File written");
  } else {
    Serial.println("Write failed");
  }
  file.close();
}

void renameFile(fs::FS &fs, const char *path1, const char *path2)
{
  Serial.printf("Renaming file %s to %s\r\n", path1, path2);
  if (fs.rename(path1, path2))
  {
    Serial.println("File renamed");
  }
  else
  {
    Serial.println("Rename failed");
  }
}

void deleteFile(fs::FS &fs, const char *path)
{
  Serial.printf("Deleting file: %s\r\n", path);
  if (fs.remove(path))
  {
    Serial.println("File deleted");
  }
  else
  {
    Serial.println("Delete failed");
  }
}