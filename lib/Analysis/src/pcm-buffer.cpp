
#include "pcm-buffer.hpp"

#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <string.h>

namespace pcm_buffer {

static QueueHandle_t queue = nullptr;

static Block assembling = {};
static size_t accumulatedFrames = 0;

static volatile uint32_t currentSampleRate = 44100;

void Setup() {
    queue = xQueueCreate(1, sizeof(Block));
}

void SetSampleRate(uint32_t rate) {
    if (rate > 0) {
        currentSampleRate = rate;
    }
}

void Push(const uint8_t* data, uint32_t length) {
    if (queue == nullptr || data == nullptr) {
        return;
    }

    // 16 бит * 2 канала = 4 байта на аудиокадр
    const size_t bytesPerFrame =
        sizeof(int16_t) * CHANNELS;

    size_t frames = length / bytesPerFrame;
    size_t offset = 0;

    while (frames > 0) {
        size_t available =
            FRAME_COUNT - accumulatedFrames;

        size_t count =
            frames < available ? frames : available;

        memcpy(
            assembling.samples +
                accumulatedFrames * CHANNELS,
            data + offset * bytesPerFrame,
            count * bytesPerFrame
        );

        accumulatedFrames += count;
        offset += count;
        frames -= count;

        if (accumulatedFrames == FRAME_COUNT) {
            assembling.sampleRate = currentSampleRate;

            // Храним только самый свежий блок.
            // Если анализ не успел, старый блок заменяем.
            xQueueOverwrite(queue, &assembling);

            accumulatedFrames = 0;
        }
    }
}

bool Pop(Block& block) {
    if (queue == nullptr) {
        return false;
    }

    return xQueueReceive(
        queue,
        &block,
        0
    ) == pdTRUE;
}

}
