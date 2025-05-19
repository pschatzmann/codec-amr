/**
 * Example demonstrating the use of the AMR codec library
 *
 * This example shows how to:
 * 1. Initialize the AMR-NB or AMR-WB codec
 * 2. Encode PCM samples to AMR format
 * 3. Decode AMR format back to PCM samples
 */

#include <math.h>
#include <stdlib.h>

#include "AMRNB.h"
#include "AMRWB.h"

#ifdef ESP32
SET_LOOP_TASK_STACK_SIZE(15 * 1024);
#endif

#ifndef ARDUINO
#include <algorithm>  // std::min
#include <iostream>
#define min(a, b) std::min(a, b)
#define PI M_PI
#endif

// Choose between narrowband (8kHz) or wideband (16kHz)
#define USE_WIDEBAND true

// Buffer sizes
#define PCM_BUFFER_SIZE 1600  // Space for 100ms of audio (800 samples at 8kHz or 1600 at 16kHz)
#define AMR_BUFFER_SIZE 640  // Space for encoded AMR data (max ~32 bytes per frame for NB, ~61 for
       // WB)

// Audio buffers
int16_t inputPcm[PCM_BUFFER_SIZE];   // Input PCM samples
uint8_t amrData[AMR_BUFFER_SIZE];    // Encoded AMR data
int16_t outputPcm[PCM_BUFFER_SIZE];  // Decoded PCM samples
char print_buffer[100];

// Print some text
void printBuffer(char *str) {
#ifdef ARDUINO
  Serial.println(str);
#else
  std::cout << str << std::endl;
#endif
}

// For demonstration, fill the input buffer with a sine wave
void generateSineWave(int16_t* buffer, size_t samples, int sampleRate) {
  const float frequency = 440.0;  // A4 note
  for (size_t i = 0; i < samples; i++) {
    float time = (float)i / sampleRate;
    buffer[i] = (int16_t)(10000.0 * sin(2.0 * PI * frequency * time));
  }
}

void setup() {
#ifdef ARDUINO
  Serial.begin(115200);
#endif

  // Create appropriate codec
  AMRCodec* codec;

  if (USE_WIDEBAND) {
    AMRWB* wb = new AMRWB();
    wb->setMode(AMRWB::Mode::MODE_23_85);  // Highest quality
    codec = wb;
  } else {
    AMRNB* nb = new AMRNB();
    nb->setMode(AMRNB::Mode::MR122);  // Highest quality
    codec = nb;
  }

  // Generate test audio
  generateSineWave(inputPcm, PCM_BUFFER_SIZE, codec->getSampleRate());

  // Encode the audio
  int encodedBytes =
      codec->encode(inputPcm, PCM_BUFFER_SIZE, amrData, AMR_BUFFER_SIZE);

  snprintf(print_buffer, 100, "Encoded %d PCM samples to %d AMR bytes",
           PCM_BUFFER_SIZE, encodedBytes);
  printBuffer(print_buffer);
  // Decode the audio
  int decodedSamples =
      codec->decode(amrData, encodedBytes, outputPcm, PCM_BUFFER_SIZE);

  snprintf(print_buffer, 100, "Decoded back to %d PCM samples", decodedSamples);
  printBuffer(print_buffer);

  int samplesToCompare = min(PCM_BUFFER_SIZE, decodedSamples);
  for (int i = 0; i < samplesToCompare; i++) {
    snprintf(print_buffer, 100, "%d", outputPcm[i]);
    printBuffer(print_buffer);
  }


  delete codec;
}

void loop() {
#ifndef ARDUINO
  exit(0);
#endif
}
