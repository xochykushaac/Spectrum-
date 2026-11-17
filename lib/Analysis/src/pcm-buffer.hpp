
#pragma once

#include <Arduino.h>

namespace pcm_buffer {

constexpr size_t FRAME_COUNT = 512;
constexpr size_t CHANNELS = 2;

struct Block {
    int16_t samples[FRAME_COUNT * CHANNELS];
    uint32_t sampleRate;
};

void Setup();

void SetSampleRate(uint32_t rate);

// Вызывается из Bluetooth-колбэка
void Push(const uint8_t* data, uint32_t length);

// Вызывается из основного loop()
bool Pop(Block& block);

}
