# 3GPP AMR Codec

The 3GPP AMR codec is a narrowband, low-bitrate speech codec specifically designed for low-latency voice communications such as Push-to-Talk (PoC) and Mission-Critical Push-to-Talk (MCPTT). It's not widely used in consumer apps, but important in specialized telecom and emergency communication systems.

The main differences between AMR-NB (Adaptive Multi-Rate Narrowband) and AMR-WB (Adaptive Multi-Rate Wideband) lie in audio bandwidth, quality, sampling rate, and use cases. Here's a breakdown:

### Bandwidth and Audio Quality

- AMR-NB (Narrowband):
    - Optimized for speech in the 300 Hz to 3400 Hz range.
    - Suitable for traditional telephony.
    - Lower audio quality due to narrower frequency range.

- AMR-WB (Wideband):
    - Covers a broader range: 50 Hz to 7000 Hz.
    - Offers better voice clarity, especially for consonants and natural tone.
    - Used in HD Voice services.

### Sampling Rate

- AMR-NB: 8 kHz (samples per second).
- AMR-WB: 16 kHz — doubles the temporal resolution, improving fidelity.

### Bitrates

- AMR-NB: Supports 8 bitrates from 4.75 kbps to 12.2 kbps.
- AMR-WB: Supports 9 bitrates from 6.6 kbps to 23.85 kbps.

### Codec Complexity

AMR-WB is more complex and requires more processing power and bandwidth, but yields significantly better sound quality.

### Use Cases

| Codec  | Typical Use Cases                |
| ------ | -------------------------------- |
| AMR-NB | 2G/3G voice calls, legacy phones |
| AMR-WB | VoLTE, HD Voice, modern phones   |

### Summary

| Codec  | PCM Bitrate | AMR Bitrate | Compression Ratio |
| ------ | ----------- | ----------- | ----------------- |
| AMR-NB | 128 kbps    | 12.2 kbps   | \~10.5:1          |
| AMR-WB | 256 kbps    | 23.85 kbps  | \~10.7:1          |
| AMR-WB | -           | 12.65 kbps  | \~20.2:1          |   
| AMR-NB | -           |  4.75 kbps  | \~27:1            |  

### Licence

This code implements both an AMR-NarrowBand (AMR-NB) and an AMR-WideBand (AMR-WB) audio encoder/decoder through external reference code from http://www.3gpp.org/. 

The license of the code from 3gpp is unclear!

