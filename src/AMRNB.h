#pragma once

#include <string.h>

#include "AMRCodec.h"
extern "C" {
#include "nb/interf_dec.h"
#include "nb/interf_enc.h"
}

/**
 * @brief AMR Narrowband codec implementation
 */
class AMRNB : public AMRCodec {
 public:
  /**
   * @brief Available encoding modes for AMR-NB
   */
  enum class Mode {
    MR475 = 0,  // 4.75 kbit/s
    MR515,      // 5.15 kbit/s
    MR59,       // 5.9 kbit/s
    MR67,       // 6.7 kbit/s
    MR74,       // 7.4 kbit/s
    MR795,      // 7.95 kbit/s
    MR102,      // 10.2 kbit/s
    MR122       // 12.2 kbit/s
  };

  /**
   * @brief Construct a new AMRNB codec
   * @param enableDTX Enable discontinuous transmission (DTX)
   */
  inline AMRNB(bool enableDTX = false) {
    // Initialize the encoder and decoder
    encoderState = Encoder_Interface_init(enableDTX ? 1 : 0);
    decoderState = Decoder_Interface_init();

    if (encoderState && decoderState) {
      isInitialized = true;
    }
  }

  /**
   * @brief Destructor
   */
  inline ~AMRNB() override {
    if (encoderState) {
      Encoder_Interface_exit(encoderState);
      encoderState = nullptr;
    }

    if (decoderState) {
      Decoder_Interface_exit(decoderState);
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
   * @brief Encode PCM samples to AMR-NB format
   * @param pcmSamples Input PCM samples (160 samples = 20ms at 8kHz)
   * @param sampleCount Number of input samples
   * @param amrData Buffer to store encoded AMR data
   * @param amrBufferSize Size of the AMR buffer in bytes
   * @return Number of bytes written to amrData
   */
  inline int encode(const int16_t* pcmSamples, size_t sampleCount,
                    uint8_t* amrData, size_t amrBufferSize) override {
    if (!isInitialized || !amrData || amrBufferSize == 0) {
      return 0;
    }

    // AMR-NB works with 160 samples per frame (20ms at 8kHz)
    const size_t samplesPerFrame = 160;
    size_t bytesWritten = 0;

    for (size_t i = 0; i < sampleCount && bytesWritten < amrBufferSize;
         i += samplesPerFrame) {
      // Make sure we have a full frame
      if (i + samplesPerFrame > sampleCount) {
        break;
      }

      // Check if we have enough space for worst-case frame size (32 bytes)
      if (bytesWritten + 32 > amrBufferSize) {
        break;
      }

      // Encode the frame
      int frameBytes = Encoder_Interface_Encode(
          encoderState, mapMode(currentMode),
          const_cast<short*>(pcmSamples + i), amrData + bytesWritten,
          0  // forceSpeech
      );

      bytesWritten += frameBytes;
    }

    return bytesWritten;
  }

  /**
   * @brief Decode AMR-NB data to PCM samples
   * @param amrData AMR encoded data
   * @param amrSize Size of AMR data in bytes
   * @param pcmSamples Output buffer for PCM samples
   * @param maxSampleCount Maximum capacity of output buffer
   * @return Number of decoded samples written
   */
  inline int decode(const uint8_t* amrData, size_t amrSize, int16_t* pcmSamples,
                    size_t maxSampleCount) override {
    if (!isInitialized || !amrData || amrSize == 0 || !pcmSamples ||
        maxSampleCount == 0) {
      return 0;
    }

    // AMR-NB produces 160 samples per frame (20ms at 8kHz)
    const size_t samplesPerFrame = 160;
    size_t totalSamplesDecoded = 0;
    size_t offset = 0;

    // Process the input buffer frame by frame
    while (offset < amrSize &&
           totalSamplesDecoded + samplesPerFrame <= maxSampleCount) {
      // Extract frame type from the frame header
      uint8_t frameType = (amrData[offset] >> 3) & 0x0F;

      // Find frame size - this is just an approximation
      size_t frameSize = 0;
      // Simple mapping based on block_size table in interf_rom.h
      switch (frameType) {
        case 0:
          frameSize = 13;
          break;  // MR475
        case 1:
          frameSize = 14;
          break;  // MR515
        case 2:
          frameSize = 16;
          break;  // MR59
        case 3:
          frameSize = 18;
          break;  // MR67
        case 4:
          frameSize = 20;
          break;  // MR74
        case 5:
          frameSize = 21;
          break;  // MR795
        case 6:
          frameSize = 27;
          break;  // MR102
        case 7:
          frameSize = 32;
          break;  // MR122
        default:
          frameSize = 1;
          break;  // Other cases like SID, etc.
      }

      if (offset + frameSize > amrSize) {
        break;  // Not enough data for another frame
      }

      // Decode this frame
      Decoder_Interface_Decode(decoderState,
                               const_cast<unsigned char*>(amrData + offset),
                               pcmSamples + totalSamplesDecoded,
                               0  // BFI (bad frame indicator)
      );

      offset += frameSize;
      totalSamplesDecoded += samplesPerFrame;
    }

    return totalSamplesDecoded;
  }

  /**
   * @brief Get sample rate (8000 Hz for AMR-NB)
   * @return Sample rate in Hz
   */
  inline int getSampleRate() const override { return 8000; }

  /**
   * @brief Get frame size in samples (160 for AMR-NB)
   * @return Frame size in samples
   */
  int getFrameSizeSamples() override { return 160; }

 private:
  void* encoderState = nullptr;
  void* decoderState = nullptr;
  Mode currentMode = Mode::MR122;

  // Map C++ enum to C API enum
  inline enum ModeNB mapMode(AMRNB::Mode mode) {
    return static_cast<ModeNB>(mode);
  }
};
