#pragma once

#include <stdint.h>
#include <stddef.h>

/**
 * @brief Base class for AMR codec implementations
 */
class AMRCodec {
public:
    virtual ~AMRCodec() = default;
    
    /**
     * @brief Encodes PCM audio samples to AMR format
     * @param pcmSamples Input PCM audio samples
     * @param sampleCount Number of samples to encode
     * @param amrData Buffer to store encoded AMR data
     * @param amrBufferSize Size of the AMR buffer in bytes
     * @return Number of bytes written to amrData
     */
    virtual int encode(const int16_t* pcmSamples, size_t sampleCount, 
                      uint8_t* amrData, size_t amrBufferSize) = 0;
    
    /**
     * @brief Decodes AMR format to PCM audio samples
     * @param amrData Input AMR encoded data
     * @param amrSize Size of AMR data in bytes
     * @param pcmSamples Output buffer for decoded PCM samples
     * @param maxSampleCount Maximum number of samples that can be written to pcmSamples
     * @return Number of decoded samples
     */
    virtual int decode(const uint8_t* amrData, size_t amrSize, 
                      int16_t* pcmSamples, size_t maxSampleCount) = 0;
    
    /**
     * @brief Get sample rate used by the codec
     * @return Sample rate in Hz (8000 for NB, 16000 for WB)
     */
    virtual int getSampleRate() const = 0;
    
    /**
     * @brief Get the number of samples per frame.
     * @return NB: 160 samples/frame, WB: 320 samples/frame
     */

    inline int getFrameSizeSamples() = 0;

protected:
    bool isInitialized = false;
};

