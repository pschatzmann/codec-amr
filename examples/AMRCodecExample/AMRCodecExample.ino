/**
 * Example demonstrating the use of the AMR codec library
 * 
 * This example shows how to:
 * 1. Initialize the AMR-NB or AMR-WB codec
 * 2. Encode PCM samples to AMR format
 * 3. Decode AMR format back to PCM samples
 */

#include <Arduino.h>
#include "AMRNB.h"
#include "AMRWB.h"

// Choose between narrowband (8kHz) or wideband (16kHz)
#define USE_WIDEBAND false

// Buffer sizes
#define PCM_BUFFER_SIZE 1600  // Space for 100ms of audio (800 samples at 8kHz or 1600 at 16kHz)
#define AMR_BUFFER_SIZE 640   // Space for encoded AMR data (max ~32 bytes per frame for NB, ~61 for WB)

// Audio buffers
int16_t inputPcm[PCM_BUFFER_SIZE];   // Input PCM samples
uint8_t amrData[AMR_BUFFER_SIZE];    // Encoded AMR data
int16_t outputPcm[PCM_BUFFER_SIZE];  // Decoded PCM samples

// For demonstration, fill the input buffer with a sine wave
void generateSineWave(int16_t* buffer, size_t samples, int sampleRate) {
  const float frequency = 440.0; // A4 note
  for (size_t i = 0; i < samples; i++) {
    float time = (float)i / sampleRate;
    buffer[i] = (int16_t)(10000.0 * sin(2.0 * PI * frequency * time));
  }
}

void setup() {
  Serial.begin(115200);
  while (!Serial && millis() < 3000); // Wait for serial connection
  
  Serial.println("AMR Codec Example");
  
  // Create appropriate codec
  AMRCodec* codec;
  
  if (USE_WIDEBAND) {
    AMRWB* wb = new AMRWB();
    wb->setMode(AMRWB::Mode::MODE_23_85);  // Highest quality
    codec = wb;
    Serial.println("Using AMR-WB (16kHz)");
  } else {
    AMRNB* nb = new AMRNB();
    nb->setMode(AMRNB::Mode::MR122);  // Highest quality
    codec = nb;
    Serial.println("Using AMR-NB (8kHz)");
  }
  
  // Generate test audio
  generateSineWave(inputPcm, PCM_BUFFER_SIZE, codec->getSampleRate());
  
  // Encode the audio
  int encodedBytes = codec->encode(inputPcm, PCM_BUFFER_SIZE, amrData, AMR_BUFFER_SIZE);
  
  Serial.print("Encoded ");
  Serial.print(PCM_BUFFER_SIZE);
  Serial.print(" PCM samples to ");
  Serial.print(encodedBytes);
  Serial.println(" AMR bytes");
  
  // Decode the audio
  int decodedSamples = codec->decode(amrData, encodedBytes, outputPcm, PCM_BUFFER_SIZE);
  
  Serial.print("Decoded back to ");
  Serial.print(decodedSamples);
  Serial.println(" PCM samples");
  
  // Compare original and decoded audio (calculate basic signal-to-noise ratio)
  float sumSquaredError = 0;
  float sumSquaredSignal = 0;
  
  int samplesToCompare = min(PCM_BUFFER_SIZE, decodedSamples);
  for (int i = 0; i < samplesToCompare; i++) {
    float error = inputPcm[i] - outputPcm[i];
    sumSquaredError += error * error;
    sumSquaredSignal += inputPcm[i] * (float)inputPcm[i];
  }
  
  if (sumSquaredSignal > 0) {
    float snr = 10 * log10(sumSquaredSignal / sumSquaredError);
    Serial.print("Signal-to-Noise Ratio: ");
    Serial.print(snr);
    Serial.println(" dB");
  }
  
  delete codec;
}

void loop() {
  // Nothing to do here
}
