#include "Audio_PCM5101.h"

Audio audio;
void IRAM_ATTR example_increase_audio_tick(void *arg)
{
  audio.loop();
}

void Audio_Init()
{
  // Audio
  audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
  audio.setVolume(3); // 0...21

  esp_timer_handle_t audio_tick_timer = NULL;
  const esp_timer_create_args_t audio_tick_timer_args = {
      .callback = &example_increase_audio_tick,
      .dispatch_method = ESP_TIMER_TASK,
      .name = "audio_tick",
      .skip_unhandled_events = true};
  esp_timer_create(&audio_tick_timer_args, &audio_tick_timer);
  esp_timer_start_periodic(audio_tick_timer, EXAMPLE_Audio_TICK_PERIOD_MS * 1000);
}

void Audio_PlayByFS(String path)
{
  // SD Card
  if (SD.exists(path))
  {
    printf("File %s found in root directory.\r\n", path.c_str());
  }
  else
  {
    printf("File %s not found in root directory.\r\n", path.c_str());
  }

  bool ret = audio.connecttoFS(SD, path.c_str());
  if (ret)
    printf("Music Read OK\r\n");
  else
    printf("Music Read Failed\r\n");
}

uint16_t Audio_VUlevel(void)
{
  uint16_t VUlevel = audio.getVUlevel();
  return VUlevel;
}

uint8_t Audio_Volume(void)
{
  uint8_t Volume = audio.getVolume();
  return Volume;
}

void Audio_setVolume(uint8_t vol)
{
  if (vol < 22)
  {
    audio.setVolume(vol);
  }
  else
  {
    printf("Volume value overload. [0...21]\r\n");
  }
}