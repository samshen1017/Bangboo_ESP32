#include "EDGEImpulse.h"
#include "Audio_PCM5101.h"
#include "MIC_MSM.h"
#include <Eous_inferencing.h>

/** Audio buffers, pointers and selectors */
typedef struct
{
    signed short *buffers[2];
    unsigned char buf_select;
    unsigned char buf_ready;
    unsigned int buf_count;
    unsigned int n_samples;
} inference_t;

static inference_t inference;
static const uint32_t sample_buffer_size = 7168;
static signed short sampleBuffer[sample_buffer_size];
static bool debug_nn = false; // Set this to true to see e.g. features generated from the raw signal
static int print_results = -(EI_CLASSIFIER_SLICES_PER_MODEL_WINDOW);
static bool record_status = true;
static bool record_enable = true;

static bool microphone_inference_start(uint32_t n_samples);
static bool edge_impulse_task_start(void);

void EDGEImpulse_Enable(bool en)
{
    record_enable = en;
}

void EDGEImpulse_init(void)
{
    // summary of inferencing settings (from model_metadata.h)
    ei_printf("Inferencing settings:\n");
    ei_printf("\tInterval: ");
    ei_printf_float((float)EI_CLASSIFIER_INTERVAL_MS);
    ei_printf(" ms.\n");
    ei_printf("\tFrame size: %d\n", EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE);
    ei_printf("\tSample length: %d ms.\n", EI_CLASSIFIER_RAW_SAMPLE_COUNT / 16);
    ei_printf("\tNo. of classes: %d\n", sizeof(ei_classifier_inferencing_categories) / sizeof(ei_classifier_inferencing_categories[0]));
    run_classifier_init();
    ei_printf("\nStarting continious inference in 2 seconds...\n");
    ei_sleep(2000);
    if (microphone_inference_start(EI_CLASSIFIER_SLICE_SIZE) == false)
    {
        ei_printf("ERR: Could not allocate audio buffer (size %d), this could be due to the window length of your model\r\n", EI_CLASSIFIER_RAW_SAMPLE_COUNT);
        return;
    }

    if (edge_impulse_task_start() == false)
    {
        ei_printf("ERR: EDGE Impulse task create failed.\r\n");
        return;
    }
    ei_printf("Recording...\n");
}

static void audio_inference_callback(uint32_t n_bytes)
{
    for (int i = 0; i < n_bytes >> 1; i++)
    {
        inference.buffers[inference.buf_select][inference.buf_count++] = sampleBuffer[i];

        if (inference.buf_count >= inference.n_samples)
        {
            inference.buf_select ^= 1;
            inference.buf_count = 0;
            inference.buf_ready = 1;
        }
    }
}

static void capture_samples(void *arg)
{

    const int32_t i2s_bytes_to_read = (uint32_t)arg;
    size_t bytes_read = i2s_bytes_to_read;

    while (record_status)
    {
        if (record_enable = false)
        {
            vTaskDelay(pdMS_TO_TICKS(100));
            continue;
        }

        /* read data at once from i2s */
        bytes_read = MIC_recordRaw((void *)sampleBuffer, i2s_bytes_to_read);

        if (bytes_read <= 0)
        {
            ei_printf("Error in I2S read : %d", bytes_read);
        }
        else
        {
            if (bytes_read < i2s_bytes_to_read)
            {
                ei_printf("Partial I2S read");
            }

            // scale the data (otherwise the sound is too quiet)
            for (int x = 0; x < i2s_bytes_to_read / 2; x++)
            {
                sampleBuffer[x] = (int16_t)(sampleBuffer[x]) * 8;
            }

            if (record_status)
            {
                audio_inference_callback(i2s_bytes_to_read);
            }
            else
            {
                break;
            }
        }
    }
    vTaskDelete(NULL);
}

/**
 * @brief      Init inferencing struct and setup/start PDM
 *
 * @param[in]  n_samples  The n samples
 *
 * @return     { description_of_the_return_value }
 */
static bool microphone_inference_start(uint32_t n_samples)
{
    inference.buffers[0] = (signed short *)malloc(n_samples * sizeof(signed short));

    if (inference.buffers[0] == NULL)
    {
        return false;
    }

    inference.buffers[1] = (signed short *)malloc(n_samples * sizeof(signed short));

    if (inference.buffers[1] == NULL)
    {
        ei_free(inference.buffers[0]);
        return false;
    }
    inference.buf_select = 0;
    inference.buf_count = 0;
    inference.n_samples = n_samples;
    inference.buf_ready = 0;

    ei_sleep(100);

    record_status = true;

    xTaskCreate(capture_samples, "CaptureSamples", 1024 * 32, (void *)sample_buffer_size, 5, NULL);

    return true;
}

/**
 * @brief      Wait on new data
 *
 * @return     True when finished
 */
static bool microphone_inference_record(void)
{
    bool ret = true;

    if (inference.buf_ready == 1)
    {
        ei_printf(
            "Error sample buffer overrun. Decrease the number of slices per model window "
            "(EI_CLASSIFIER_SLICES_PER_MODEL_WINDOW)\n");
        ret = false;
    }

    while (inference.buf_ready == 0)
    {
        delay(1);
    }

    inference.buf_ready = 0;
    return true;
}

/**
 * Get raw audio signal data
 */
static int microphone_audio_signal_get_data(size_t offset, size_t length, float *out_ptr)
{
    numpy::int16_to_float(&inference.buffers[inference.buf_select ^ 1][offset], out_ptr, length);

    return 0;
}

static void EDGEImpulse_loop(void *arg)
{
    while (true)
    {
        if (record_enable = false)
        {
            vTaskDelay(pdMS_TO_TICKS(100));
            continue;
        }

        bool m = microphone_inference_record();
        if (!m)
        {
            ei_printf("ERR: Failed to record audio...\n");
            return;
        }
        signal_t signal;
        signal.total_length = EI_CLASSIFIER_SLICE_SIZE;
        signal.get_data = &microphone_audio_signal_get_data;
        ei_impulse_result_t result = {0};

        EI_IMPULSE_ERROR r = run_classifier_continuous(&signal, &result, debug_nn);
        if (r != EI_IMPULSE_OK)
        {
            ei_printf("ERR: Failed to run classifier (%d)\n", r);
            return;
        }

        if (++print_results >= (EI_CLASSIFIER_SLICES_PER_MODEL_WINDOW))
        {
            for (size_t i = 0; i < 10; i++)
            {
                ei_printf("[%d] ", sampleBuffer[i]);
            }
            ei_printf("\r\n");
            // print the predictions
            ei_printf("Predictions ");
            ei_printf("(DSP: %d ms., Classification: %d ms., Anomaly: %d ms.)",
                      result.timing.dsp, result.timing.classification, result.timing.anomaly);
            ei_printf(": \n");
            for (size_t ix = 0; ix < EI_CLASSIFIER_LABEL_COUNT; ix++)
            {
                ei_printf("    %s: ", result.classification[ix].label);
                ei_printf_float(result.classification[ix].value);
                ei_printf("\n");
                if (result.classification[ix].label[0] == 'E')
                {
                    if (result.classification[ix].value > 0.7)
                    {
                        static int n = 1;
                        String f_str = "/audio/Eous/" + String(n % 3 + 1) + ".mp3";
                        ei_printf("play %s\r\n", f_str.c_str());
                        Audio_PlayByFS(f_str.c_str());
                        n++;
                    }
                }
            }
#if EI_CLASSIFIER_HAS_ANOMALY == 1
            ei_printf("    anomaly score: ");
            ei_printf_float(result.anomaly);
            ei_printf("\n");
#endif
            print_results = 0;
        }
    }
}

static bool edge_impulse_task_start(void)
{
    xTaskCreate(EDGEImpulse_loop, "EDGEImpulseLoop", 1024 * 32, NULL, 10, NULL);
    return true;
}

/**
 * @brief      Stop PDM and release buffers
 */
static void microphone_inference_end(void)
{
    ei_free(inference.buffers[0]);
    ei_free(inference.buffers[1]);
}
