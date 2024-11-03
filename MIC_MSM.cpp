#include "MIC_MSM.h"
#include "ESP_I2S.h"
#include "SD_Card.h"

I2SClass i2s;

struct MIC_Recorder
{
  bool start = false;
  uint8_t state = Record_Standby;
  uint32_t seconds; // 需要录制的总时长
  uint8_t *wav_buffer;
  size_t wav_size;
};

static MIC_Recorder recorder;

size_t MIC_recordRaw(void *simpleBuffer, size_t bytes_to_read)
{
  uint32_t sample_rate = i2s.rxSampleRate();
  uint16_t sample_width = (uint16_t)i2s.rxDataWidth();
  uint16_t num_channels = (uint16_t)i2s.rxSlotMode();

  log_d("Record WAV: rate:%lu, bits:%u, channels:%u, size:%lu", sample_rate, sample_width, num_channels, bytes_to_read);

  size_t wav_size = i2s.readBytes((char *)simpleBuffer, bytes_to_read);
  
  if (i2s.lastError())
  {
    log_e("Read Failed! %d", i2s.lastError());
  }

  return wav_size;
}

void _MIC_Init()
{
  i2s.setPins(I2S_PIN_BCK, I2S_PIN_WS, I2S_PIN_DOUT, I2S_PIN_DIN);
  i2s.setTimeout(1000);
  i2s.begin(I2S_MODE_STD, 16000, I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_STEREO);
}

void MICTask(void *parameter)
{
  _MIC_Init();
  while (1)
  {
    switch (recorder.state)
    {
    case Record_Standby:
      if (recorder.start)
      {
        printf("start record.\r\n");
        recorder.state = Record_Busy;
        recorder.start = false;
        // Record 5 seconds of audio data
        recorder.wav_buffer = i2s.recordWAV(recorder.seconds, &recorder.wav_size);
      }
      break;
    case Record_Busy:
    {
      uint32_t w_size = writeRawFile(SD, "/record/test.wav", recorder.wav_buffer, recorder.wav_size);
      printf("w_size: %lu, wav_size: %lu\r\n", w_size, recorder.wav_size);
      recorder.state = Record_Finish;
      break;
    }
    case Record_Finish:
      if (recorder.wav_buffer != nullptr)
      {
        free(recorder.wav_buffer);
        recorder.wav_buffer = nullptr;
      }
      printf("Record finish.\r\n");
      recorder.state = Record_Standby;
      break;
    default:
      break;
    }
    vTaskDelay(pdMS_TO_TICKS(10));
  }
  vTaskDelete(NULL);
}

void MIC_Init(void)
{
  _MIC_Init();
  
  xTaskCreatePinnedToCore(
      MICTask,
      "MICTask",
      8192,
      NULL,
      5,
      NULL,
      1);
}

void MIC_RecordStart(uint32_t seconds)
{
  if (recorder.state == Record_Standby)
  {
    recorder.seconds = seconds;
    recorder.start = true;
  }
}

void MIC_ForceChangeState(uint8_t state)
{
  recorder.state = state;
}
