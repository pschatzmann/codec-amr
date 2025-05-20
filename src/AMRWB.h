#pragma once

#include <string.h>

#include "AMRCodec.h"
extern "C" {
#include "wb/dec_if.h"
#include "wb/enc_if.h"
}

/**
 * @brief AMR Wideband codec implementation
 */
class AMRWB : public AMRCodec {
 public:
  /**
   * @brief Available encoding modes for AMR-WB
   */
  enum class Mode {
    MODE_6_60 = 0,  // 6.60 kbit/s
    MODE_8_85,      // 8.85 kbit/s
    MODE_12_65,     // 12.65 kbit/s
    MODE_14_25,     // 14.25 kbit/s
    MODE_15_85,     // 15.85 kbit/s
    MODE_18_25,     // 18.25 kbit/s
    MODE_19_85,     // 19.85 kbit/s
    MODE_23_05,     // 23.05 kbit/s
    MODE_23_85      // 23.85 kbit/s
  };

  AMRWB() = default;

  /**
   * @brief Destructor
   */
  inline ~AMRWB() override {
    if (encoderState) {
      E_IF_exit(encoderState);
      encoderState = nullptr;
    }

    if (decoderState) {
      D_IF_exit(decoderState);
      decoderState = nullptr;
    }
  }

  /**
   * @brief Set encoding mode
   * @param mode Desired encoding mode
   */
  inline void setMode(Mode mode) { currentMode = mode; }

  /**
   * @brief Get current encoding mode
   * @return Current mode
   */
  inline Mode getMode() const { return currentMode; }

  /**
   * @brief Encode PCM samples to AMR-WB format
   * @param pcmSamples Input PCM samples (320 samples = 20ms at 16kHz)
   * @param sampleCount Number of input samples
   * @param amrData Buffer to store encoded AMR data
   * @param amrBufferSize Size of the AMR buffer in bytes
   * @return Number of bytes written to amrData
   */
  inline int encode(const int16_t* pcmSamples, size_t sampleCount,
                    uint8_t* amrData, size_t amrBufferSize) override {
    if (encoderState == nullptr) encoderState = E_IF_init();

    if (encoderState == nullptr || !amrData || amrBufferSize == 0) {
      return 0;
    }

    // AMR-WB works with 320 samples per frame (20ms at 16kHz)
    const size_t samplesPerFrame = 320;
    size_t bytesWritten = 0;

    for (size_t i = 0; i < sampleCount && bytesWritten < amrBufferSize;
         i += samplesPerFrame) {
      // Make sure we have a full frame
      if (i + samplesPerFrame > sampleCount) {
        break;
      }

      // Check if we have enough space for worst-case frame size (64 bytes)
      if (bytesWritten + 64 > amrBufferSize) {
        break;
      }

      // Encode the frame
      int frameBytes = E_IF_encode(encoderState, static_cast<int>(currentMode),
                                   const_cast<short*>(pcmSamples + i),
                                   amrData + bytesWritten,
                                   0  // forceSpeech
      );

      bytesWritten += frameBytes;
    }

    return bytesWritten;
  }

  /**
   * @brief Decode AMR-WB data to PCM samples
   * @param amrData AMR encoded data
   * @param amrSize Size of AMR data in bytes
   * @param pcmSamples Output buffer for PCM samples
   * @param maxSampleCount Maximum capacity of output buffer
   * @return Number of decoded samples written
   */
  inline int decode(const uint8_t* amrData, size_t amrSize, int16_t* pcmSamples,
                    size_t maxSampleCount) override {
    if (decoderState == nullptr) decoderState = D_IF_init();

    if (decoderState == nullptr || !amrData || amrSize == 0 || !pcmSamples ||
        maxSampleCount == 0) {
      return 0;
    }

    // AMR-WB produces 320 samples per frame (20ms at 16kHz)
    const size_t samplesPerFrame = getFrameSizeSamples();
    size_t totalSamplesDecoded = 0;
    size_t offset = 0;

    // Process the input buffer frame by frame
    while (offset < amrSize &&
           totalSamplesDecoded + samplesPerFrame <= maxSampleCount) {
      // Extract frame type from the frame header
      uint8_t frameType = (amrData[offset] >> 3) & 0x0F;

      // Find frame size based on frame type
      size_t frameSize = 1;
      if (frameSize <= 8) {
        frameSize = getEncodedFrameSizeBytes(frameType);
      }

      if (offset + frameSize > amrSize) {
        break;  // Not enough data for another frame
      }

      // Decode this frame
      D_IF_decode(decoderState, const_cast<unsigned char*>(amrData + offset),
                  pcmSamples + totalSamplesDecoded,
                  0  // BFI (bad frame indicator)
      );

      offset += frameSize;
      totalSamplesDecoded += samplesPerFrame;
    }

    return totalSamplesDecoded;
  }

  /**
   * @brief Get sample rate (16000 Hz for AMR-WB)
   * @return Sample rate in Hz
   */
  inline int getSampleRate() const override { return 16000; }

  /**
   * @brief Get frame size in samples (320 for AMR-WB)
   * @return Frame size in samples
   */
  int getFrameSizeSamples() override { return 320; }

  /**
   * @brief Get the size in bytes for one encoded frame in current mode
   * @return Bytes per frame
   */
  int getEncodedFrameSizeBytes() override {
    return getEncodedFrameSizeBytes(static_cast<int>(currentMode));
  }

 private:
  void* encoderState = nullptr;
  void* decoderState = nullptr;
  Mode currentMode = Mode::MODE_23_85;

  int getEncodedFrameSizeBytes(int mode) {
    // Bytes per encoded frame for each mode
    const uint8_t frameSizes[] = {
        18,  // MODE_6_60 (6.60 kbps)
        24,  // MODE_8_85 (8.85 kbps)
        33,  // MODE_12_65 (12.65 kbps)
        37,  // MODE_14_25 (14.25 kbps)
        41,  // MODE_15_85 (15.85 kbps)
        47,  // MODE_18_25 (18.25 kbps)
        51,  // MODE_19_85 (19.85 kbps)
        59,  // MODE_23_05 (23.05 kbps)
        61   // MODE_23_85 (23.85 kbps)
    };
    return frameSizes[mode];
  }
};
