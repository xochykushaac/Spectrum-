#include "rgb-effect.hpp"

#include <Arduino.h>
#include <math.h>

namespace rgb_effect {

constexpr int PIN_RED = 16;
constexpr int PIN_GREEN = 27;
constexpr int PIN_BLUE = 33;

constexpr int PWM_RED = 1;
constexpr int PWM_GREEN = 2;
constexpr int PWM_BLUE = 3;

float targetHue = 0;
float currentHue = 0;
float targetBrightness = 0;
float currentBrightness = 0;
float activityLevel = 0;
float averageFrequency = 0;

unsigned long lastUpdate = 0;
unsigned long lastAudioUpdate = 0;

constexpr float MIN_BRIGHTNESS = 25.0f;
constexpr float MAX_BRIGHTNESS = 180.0f;

constexpr float MIN_ENERGY = 0.012f;
constexpr float MAX_ENERGY = 0.070f;

constexpr float BRIGHTNESS_ATTACK = 0.070f;
constexpr float BRIGHTNESS_RELEASE = 0.250f;

constexpr float COLOR_SLOW = 2.0f;
constexpr float COLOR_FAST = 0.75f;
constexpr float COLOR_SPREAD = 1.65f;

void SetColor(float hue, float brightness) {
    // Hue может быть отрицательным: это нужно для плавного
    // перехода через красный цвет (0 градусов).
    hue = fmodf(hue, 360.0f);

    if (hue < 0.0f) {
        hue += 360.0f;
    }

    brightness = constrain(
        brightness,
        0.0f,
        255.0f
    );

    float chroma = brightness;
    float h = hue / 60.0f;

    float x = chroma *
        (1.0f - fabsf(fmodf(h, 2.0f) - 1.0f));

    float r = 0;
    float g = 0;
    float b = 0;

    if (h < 1.0f) {
        r = chroma;
        g = x;
    } else if (h < 2.0f) {
        r = x;
        g = chroma;
    } else if (h < 3.0f) {
        g = chroma;
        b = x;
    } else if (h < 4.0f) {
        g = x;
        b = chroma;
    } else if (h < 5.0f) {
        r = x;
        b = chroma;
    } else {
        r = chroma;
        b = x;
    }

    ledcWrite(PWM_RED, static_cast<int>(r));
    ledcWrite(PWM_GREEN, static_cast<int>(g));
    ledcWrite(PWM_BLUE, static_cast<int>(b));
}

void Setup() {
    ledcSetup(PWM_RED, 5000, 8);
    ledcAttachPin(PIN_RED, PWM_RED);

    ledcSetup(PWM_GREEN, 5000, 8);
    ledcAttachPin(PIN_GREEN, PWM_GREEN);

    ledcSetup(PWM_BLUE, 5000, 8);
    ledcAttachPin(PIN_BLUE, PWM_BLUE);

    SetColor(0, 0);

    lastUpdate = millis();
}

void SetMusic(
    float centroidHz,
    float energy,
    float activity,
    bool playing
) {
    if (!playing) {
        targetBrightness = 0;
        averageFrequency = 0;
        lastAudioUpdate = 0;
        activityLevel = 0;
        return;
    }

    // --------------------------
    // 1. ЦВЕТ
    // --------------------------

    float frequency = constrain(
        centroidHz,
        150.0f,
        8000.0f
    );

    unsigned long now = millis();

    if (averageFrequency == 0) {
        averageFrequency = frequency;
        lastAudioUpdate = now;
    }

    float dtAudio = min(
        now - lastAudioUpdate,
        100UL
    ) / 1000.0f;

    lastAudioUpdate = now;

    // Медленно отслеживаем средний тембр композиции
    float referenceAlpha =
        1.0f - expf(-dtAudio / 10.0f);

    averageFrequency +=
        (frequency - averageFrequency)
        * referenceAlpha;

    // Отклонение текущего тембра от среднего
    float ratio = frequency / averageFrequency;

    // Более сильный разброс цветов, чем у ленты
    float position =
        0.5f +
        COLOR_SPREAD *
        logf(ratio) / logf(1.4f);

    position = constrain(
        position,
        0.0f,
        1.0f
    );

    // Среднему тембру соответствует красный
    // Диапазон оттенков: от -180 до +180 градусов
    targetHue = (position - 0.5f) * 360.0f;

    // --------------------------
    // 2. ЯРКОСТЬ
    // --------------------------

    energy = constrain(
        energy,
        0.0f,
        1.0f
    );

    float normalizedEnergy =
        (energy - MIN_ENERGY) /
        (MAX_ENERGY - MIN_ENERGY);

    normalizedEnergy = constrain(
        normalizedEnergy,
        0.0f,
        1.0f
    );

    targetBrightness =
        MIN_BRIGHTNESS +
        (MAX_BRIGHTNESS - MIN_BRIGHTNESS)
        * normalizedEnergy;

    // --------------------------
    // 3. АКТИВНОСТЬ
    // --------------------------

    activityLevel = constrain(
        activity,
        0.0f,
        1.0f
    );
}

void Loop() {
    unsigned long now = millis();
    unsigned long elapsed = now - lastUpdate;

    if (elapsed < 20) {
        return;
    }

    lastUpdate = now;

    float dt = min(elapsed, 100UL) / 1000.0f;

    // --------------------------
    // 1. ПЛАВНОСТЬ ЦВЕТА
    // --------------------------

    float transitionTime =
        COLOR_SLOW -
        (COLOR_SLOW - COLOR_FAST)
        * activityLevel;

    float hueAlpha =
        1.0f - expf(-dt / transitionTime);

    currentHue +=
        (targetHue - currentHue) * hueAlpha;

    // --------------------------
    // 2. ПЛАВНОСТЬ ЯРКОСТИ
    // --------------------------

    float brightnessTime =
        targetBrightness > currentBrightness
            ? BRIGHTNESS_ATTACK
            : BRIGHTNESS_RELEASE;

    float brightnessAlpha =
        1.0f - expf(-dt / brightnessTime);

    currentBrightness +=
        (targetBrightness - currentBrightness)
        * brightnessAlpha;

    // --------------------------
    // 3. ОБНОВЛЕНИЕ RGB
    // --------------------------

    SetColor(currentHue, currentBrightness);
}

}  // namespace rgb_effect
