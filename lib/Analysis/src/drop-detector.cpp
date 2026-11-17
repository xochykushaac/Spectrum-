
#include "drop-detector.hpp"

#include <Arduino.h>
#include <math.h>

namespace drop_detector {

// Высокая избирательность:
// абсолютный уровень энергии
constexpr float MIN_DROP_ENERGY = 0.055f;

// Насколько текущая энергия должна
// превосходить недавнюю среднюю
constexpr float RELATIVE_THRESHOLD = 1.75f;

// Минимальный абсолютный прирост энергии
constexpr float MIN_ENERGY_INCREASE = 0.025f;

// Временные характеристики (секунды)
constexpr float FAST_TIME = 0.12f;
constexpr float SLOW_TIME = 6.0f;

// Рост должен удерживаться минимум 180 мс
constexpr unsigned long CONFIRM_TIME = 180;

// Первые 1.5 секунды собираем историю
constexpr unsigned long WARMUP_TIME = 1500;

float fastEnergy = 0.0f;
float slowEnergy = 0.0f;

unsigned long startTime = 0;
unsigned long lastUpdate = 0;
unsigned long candidateStart = 0;

bool initialized = false;
bool candidate = false;

void Reset() {
    fastEnergy = 0.0f;
    slowEnergy = 0.0f;

    startTime = 0;
    lastUpdate = 0;
    candidateStart = 0;

    initialized = false;
    candidate = false;
}

void Setup() {
    Reset();
}

bool Process(float energy) {
    unsigned long now = millis();

    energy = constrain(energy, 0.0f, 1.0f);

    if (!initialized) {
        fastEnergy = energy;
        slowEnergy = energy;

        startTime = now;
        lastUpdate = now;
        initialized = true;

        return false;
    }

    float dt = min(
        now - lastUpdate,
        100UL
    ) / 1000.0f;

    lastUpdate = now;

    // Быстрая оценка текущей энергии
    float fastAlpha =
        1.0f - expf(-dt / FAST_TIME);

    fastEnergy +=
        (energy - fastEnergy) * fastAlpha;

    // Медленная оценка фоновой энергии
    float slowAlpha =
        1.0f - expf(-dt / SLOW_TIME);

    slowEnergy +=
        (energy - slowEnergy) * slowAlpha;

    // Не анализируем без накопленной истории
    if (now - startTime < WARMUP_TIME) {
        return false;
    }

    // Текущая энергия должна быть:
    // 1. Достаточно высокой
    // 2. Значительно выше обычной
    // 3. Заметно выше в абсолютном выражении

    bool powerful =
        fastEnergy >= MIN_DROP_ENERGY;

    bool relativeGrowth =
        fastEnergy >=
        slowEnergy * RELATIVE_THRESHOLD;

    bool absoluteGrowth =
        fastEnergy - slowEnergy >=
        MIN_ENERGY_INCREASE;

    bool strongRise =
        powerful &&
        relativeGrowth &&
        absoluteGrowth;

    // Проверяем продолжительность роста
    if (!strongRise) {
        candidate = false;
        return false;
    }

    if (!candidate) {
        candidate = true;
        candidateStart = now;
        return false;
    }

    if (now - candidateStart >= CONFIRM_TIME) {
        candidate = false;

        // Один подтверждённый DROP.
        // Повторные включения ограничит Fog Effect.
        return true;
    }

    return false;
}

}
