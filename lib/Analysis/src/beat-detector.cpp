
#include "beat-detector.hpp"

#include <Arduino.h>
#include <math.h>

namespace beat_detector {

// Частоты глубокого баса
constexpr float LOW_CUTOFF = 20.0f;
constexpr float HIGH_CUTOFF = 75.0f;

// Чувствительность к басовым ударам
constexpr float BEAT_THRESHOLD = 1.40f;
constexpr float MIN_BASS_LEVEL = 0.0025f;
// constexpr float HIGH_CUTOFF = 75.0f;

// Минимальный интервал между вспышками
constexpr unsigned long MIN_BEAT_INTERVAL = 280;

// Двухступенчатые фильтры
float low30_1 = 0.0f;
float low30_2 = 0.0f;

float low95_1 = 0.0f;
float low95_2 = 0.0f;

// Отслеживание басовой энергии
float averageBass = 0.0f;
float previousBass = 0.0f;

unsigned long lastBeatTime = 0;
bool armed = true;

void Reset() {
    low30_1 = 0.0f;
    low30_2 = 0.0f;

    low95_1 = 0.0f;
    low95_2 = 0.0f;

    averageBass = 0.0f;
    previousBass = 0.0f;

    lastBeatTime = 0;
    armed = true;
}

void Setup() {
    Reset();
}

bool Process(
    const int16_t* pcm,
    size_t frames,
    uint32_t sampleRate,
    uint8_t channels
) {
    if (pcm == nullptr ||
        frames == 0 ||
        sampleRate == 0 ||
        (channels != 1 && channels != 2)) {
        return false;
    }

    float alpha30 = 1.0f - expf(
        -2.0f * PI * LOW_CUTOFF / sampleRate
    );

    float alpha95 = 1.0f - expf(
        -2.0f * PI * HIGH_CUTOFF / sampleRate
    );

    float bassSquareSum = 0.0f;

    for (size_t i = 0; i < frames; ++i) {

        // Объединяем два канала в моно
        float sample = 0.0f;

        for (uint8_t c = 0; c < channels; ++c) {
            sample += pcm[i * channels + c];
        }

        sample /= (32768.0f * channels);

        // Фильтр ниже 30 Гц
        low30_1 += alpha30 * (sample - low30_1);
        low30_2 += alpha30 * (low30_1 - low30_2);

        // Фильтр ниже 95 Гц
        low95_1 += alpha95 * (sample - low95_1);
        low95_2 += alpha95 * (low95_1 - low95_2);

        // Приближенно выделяем 30–95 Гц
        float bass = low95_2 - low30_2;

        bassSquareSum += bass * bass;
    }

    // Текущая энергия глубокого баса
    float bassLevel = sqrtf(
        bassSquareSum / frames
    );

    // Усреднение за более длительный период
    float duration =
        static_cast<float>(frames) / sampleRate;

    float alphaAverage =
        1.0f - expf(-duration / 0.7f);

    // Используем предыдущий средний уровень,
    // чтобы не ослаблять только что начавшийся удар
    float referenceBass = averageBass;

    if (averageBass == 0.0f) {
        averageBass = bassLevel;
    } else {
        averageBass += alphaAverage *
            (bassLevel - averageBass);
    }

    unsigned long now = millis();

    // После удара ждём снижения басовой энергии,
    // прежде чем разрешить следующую вспышку
    if (bassLevel < averageBass * 1.05f) {
        armed = true;
    }

    bool detected =
        armed &&
        bassLevel > MIN_BASS_LEVEL &&
        bassLevel > referenceBass * BEAT_THRESHOLD &&
        bassLevel > previousBass * 1.10f &&
        now - lastBeatTime >= MIN_BEAT_INTERVAL;

    previousBass = bassLevel;

    if (detected) {
        lastBeatTime = now;
        armed = false;
    }

    return detected;
}

}

