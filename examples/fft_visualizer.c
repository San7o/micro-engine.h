// SPDX-License-Identifier: MIT
// Author:  Giovanni Santini
// Mail:    giovanni.santini@proton.me
// Github:  @San7o

#include <micro-engine/micro-engine.h>

#define MICRO_APP_MAIN
#include <micro-engine/micro-app.h>
#define MICRO_FFT_IMPLEMENTATION
#include <micro-engine/micro-fft.h>

#define SDL3_PLATFORM_IMPLEMENTATION
#include <micro-engine/platforms/sdl3_platform.h>
#define MA_IMPLEMENTATION
#include <micro-engine/external/miniaudio.h>

#define WINDOW_WIDTH  800
#define WINDOW_HEIGHT 800

MicroDrawCanvas canvas = {0};
unsigned char red[4]   = {255, 0, 0, 255};
unsigned char black[4] = {0, 0, 0, 255};
MicroDrawText fps_text = {
  .text  = "",
  .x     = 10,
  .y     = WINDOW_WIDTH - 50,
  .scale = 0.3,
};
char fps_str[20] = {0};

ma_decoder decoder;
ma_device  device;

// Must be a power of two for FFT
//#define FRAME_COUNT_MAX (1024)
#define FRAME_COUNT_MAX (1<<7)
#define FREQUENCY_SCALING 800.0
float frames[FRAME_COUNT_MAX] = {0};       // audio samples
float frequencies[FRAME_COUNT_MAX] = {0};  // filled by FFT
float smoothed_frequencies[FRAME_COUNT_MAX] = {0}; // For visualization
float decay_speed = 0.7f;

// Copies [in_frames] to [out_frequencies], used for visualizations or
// debugging
void frames_as_frequencies(const float* in_frames,
                           float *out_frequencies,
                           unsigned int n_frames)
{
  for (unsigned int i = 0; i < n_frames; ++i)
    out_frequencies[i] = in_frames[i];
  return;
}

void audio_data_callback(ma_device* pDevice, void* pOutput,
                         const void* pInput, ma_uint32 frameCount)
{
  ma_decoder* pDecoder = (ma_decoder*)pDevice->pUserData;
  if (pDecoder == NULL)
    return;

  ma_decoder_read_pcm_frames(pDecoder, pOutput, frameCount, NULL);

  for (unsigned int i = 0; i < frameCount && i < FRAME_COUNT_MAX; ++i)
  {
    frames[i] = ((float*)pOutput)[i*2];
  }

  (void)pInput;
}

bool micro_app_setup(int argc, char **argv)
{
  (void) argc;
  (void) argv;
  
  micro_platform.init("hello app", WINDOW_WIDTH, WINDOW_HEIGHT);
  micro_log_init();
  micro_draw_canvas_init(&canvas, WINDOW_WIDTH, WINDOW_HEIGHT, MICRO_DRAW_RGBA8);

  ma_result result;
  ma_device_config deviceConfig;
  
  result = ma_decoder_init_file("utils/test_audio.mp3", NULL, &decoder);
  if (result != MA_SUCCESS)
  {
    printf("Could not load file\n");
    return -2;
  }

  deviceConfig = ma_device_config_init(ma_device_type_playback);
  deviceConfig.playback.format   = decoder.outputFormat;
  deviceConfig.playback.channels = decoder.outputChannels;
  deviceConfig.sampleRate        = decoder.outputSampleRate;
  deviceConfig.dataCallback      = audio_data_callback;
  deviceConfig.pUserData         = &decoder;

  if (ma_device_init(NULL, &deviceConfig, &device) != MA_SUCCESS)
  {
    micro_log_error("Failed to open playback device.\n");
    ma_decoder_uninit(&decoder);
    return -3;
  }

  if (ma_device_start(&device) != MA_SUCCESS)
  {
    micro_log_error("Failed to start playback device.\n");
    ma_device_uninit(&device);
    ma_decoder_uninit(&decoder);
    return -4;
  }
  
  return true;
}

bool micro_app_update(float delta_time)
{
  if (micro_platform.get_key(MICRO_KEY_ESCAPE))
    return false;
  
  sprintf(fps_str, "FPS: %.2f", 1.0 / (delta_time / 1000.0));
  fps_text.text = &fps_str[0];

  // Simple Hann Window
  // If the audio waveform doesn't start and end exactly at zero at
  // the edges of the frame buffer, the FFT sees a sharp "cliff."
  // This creates artificial noise across all frequencies, known as
  // spectral leakage.
  for (int i = 0; i < FRAME_COUNT_MAX; i++)
  {
    float window = 0.5f * (1.0f - cosf(2.0f * PI * i / (FRAME_COUNT_MAX - 1)));
    frames[i] = frames[i] * window;
  }
  
  //frames_as_frequencies(frames, frequencies, FRAME_COUNT_MAX);  
  micro_fft(frames, frequencies, FRAME_COUNT_MAX);

  // Calculate smoothed frequencies
  for (int i = 0; i < FRAME_COUNT_MAX; i++)
  {
    float target = frequencies[i];
    if (target > smoothed_frequencies[i]) {
      smoothed_frequencies[i] = target; 
    } else {
      smoothed_frequencies[i] -= decay_speed * delta_time / 1000.0f;
    }
  }
  
  return true;
}

bool micro_app_draw(void)
{
  micro_draw_clear(&canvas, black);

  for (unsigned int i = 0; i < FRAME_COUNT_MAX / 2; ++i)
  {
    if (smoothed_frequencies[i] <= 0.0) continue;
    float bar_width = WINDOW_WIDTH / (float)FRAME_COUNT_MAX;
    Rect rect = (Rect){
      .a_x = i * bar_width * 2,
      .a_y = 0,
      .b_x = i * bar_width * 2 + bar_width,
      .b_y = 0,
      .c_x = i * bar_width * 2 + bar_width,
      .c_y = smoothed_frequencies[i] * FREQUENCY_SCALING,
      .d_x = i * bar_width * 2,
      .d_y = smoothed_frequencies[i] * FREQUENCY_SCALING,
      
    };
    micro_draw_fill_rect(&canvas, rect, red);
  }
  
  micro_draw_text(&canvas, fps_text, red);
  
  micro_platform.draw_frame(canvas.data, canvas.width, canvas.height);
  micro_platform.sleep_ms(64);
  return true;
}

void micro_app_cleanup(void)
{
  micro_draw_canvas_free(&canvas);
  micro_log_close();
  micro_platform.terminate();
}
