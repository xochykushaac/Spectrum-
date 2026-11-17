
#pragma once

#include <stddef.h>
#include <stdint.h>

namespace beat_detector {

void Setup();

bool Process(
    const int16_t* pcm,
    size_t frames,
    uint32_t sampleRate,
    uint8_t channels
);

void Reset();

}
