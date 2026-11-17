
#pragma once

#include <stddef.h>
#include <stdint.h>

namespace music_analysis {

struct Features {
    float centroidHz;
    float energy;
    float activity;
};

// Анализ 512 аудиокадров PCM 16-bit.
// channels = 1 или 2.
Features Analyze(
    const int16_t* pcm,
    size_t frames,
    uint32_t sampleRate,
    uint8_t channels
);

}
