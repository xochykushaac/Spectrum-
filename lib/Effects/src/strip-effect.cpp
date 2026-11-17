
#include "strip-effect.hpp"

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <math.h>

namespace strip_effect {

constexpr int LED_PIN = 19;
constexpr int LED_COUNT = 7;

Adafruit_NeoPixel strip(
    LED_COUNT,
    LED_PIN,
    NEO_GRB + NEO_KHZ800
);

// Текущие и целевые параметры
float targetHue = 0;
float currentHue = 0;

float targetBrightness = 0;
float currentBrightness = 0;

float activityLevel = 0;
bool isPlaying = false;

// Средний частотный состав музыки
float averageFrequency = 0;

unsigned long lastUpdate = 0;
unsigned long lastAudioUpdate = 0;

// Настройки яркости
constexpr float MIN_BRIGHTNESS = 30.0f;
constexpr float MAX_BRIGHTNESS = 230.0f;

// Диапазон энергий по твоим измерениям
constexpr float MIN_ENERGY = 0.012f;
constexpr float MAX_ENERGY = 0.070f;

// Время реакции яркости (секунды)
constexpr float BRIGHTNESS_ATTACK = 0.045f;
constexpr float BRIGHTNESS_RELEASE = 0.170f;

// Время изменения цвета (секунды)
constexpr float COLOR_SLOW = 0.75f;
constexpr float COLOR_FAST = 0.28f;

void Setup() {
    strip.begin();
    strip.clear();
    strip.show();

    lastUpdate = millis();
}

// Получаем результаты анализа PCM
void SetMusic(
    float centroidHz,
    float energy,
    float activity,
    bool playing
) {
    isPlaying = playing;

    if (!playing) {
        targetBrightness = 0;
        averageFrequency = 0;
        lastAudioUpdate = 0;
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

    // Медленно отслеживаем средний тембр
    // композиции (характерное время 8 секунд)
    float referenceAlpha =
        1.0f - expf(-dtAudio / 8.0f);

    averageFrequency +=
        (frequency - averageFrequency)
        * referenceAlpha;

    // Сравниваем текущий спектр
    // со средним спектром музыки
    float ratio = frequency / averageFrequency;

    // Ниже среднего -> теплые цвета
    // Выше среднего -> холодные цвета
    float position =
        0.48f +
        0.85f * logf(ratio) / logf(1.4f);

position = constrain(position, 0.0f, 1.0f);

    position = constrain(position, 0.0f, 1.0f);

    // Полный переход от красного к розовому
    targetHue = position * 59000.0f;

    // --------------------------
    // 2. ЯРКОСТЬ
    // --------------------------

    energy = constrain(
        energy,
        0.0f,
        1.0f
    );

    // Приводим реальную энергию PCM
    // к диапазону 0..1
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
    // 2. РЕАКЦИЯ ЯРКОСТИ
    // --------------------------

    float brightnessTime;

    if (targetBrightness > currentBrightness) {
        // Громкость выросла:
        // быстро увеличиваем яркость
        brightnessTime = BRIGHTNESS_ATTACK;
    } else {
        // Громкость уменьшилась:
        // плавнее уменьшаем яркость
        brightnessTime = BRIGHTNESS_RELEASE;
    }

    float brightnessAlpha =
        1.0f - expf(-dt / brightnessTime);

    currentBrightness +=
        (targetBrightness - currentBrightness)
        * brightnessAlpha;

    // --------------------------
    // 3. ОБНОВЛЕНИЕ ЛЕНТЫ
    // --------------------------

    uint8_t brightness = static_cast<uint8_t>(
        constrain(
            currentBrightness,
            0.0f,
            255.0f
        )
    );

    uint32_t color = strip.ColorHSV(
        static_cast<uint16_t>(currentHue),
        255,
        brightness
    );

    for (int i = 0; i < LED_COUNT; ++i) {
        strip.setPixelColor(i, color);
    }

    strip.show();
}

}
