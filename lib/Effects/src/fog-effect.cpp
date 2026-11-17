
#include "fog-effect.hpp"

#include <Arduino.h>

namespace fog_effect {

constexpr int RELAY_PIN = 18;

// GPIO18 управляет Gate AO3400
constexpr int RELAY_ON = HIGH;
constexpr int RELAY_OFF = LOW;

// Время удержания кнопки K1
constexpr unsigned long FOG_DURATION = 6000;

// Пауза после выключения тумана
constexpr unsigned long COOLDOWN = 20000;

bool active = false;
bool hasFinishedCycle = false;

unsigned long startTime = 0;
unsigned long stopTime = 0;

void Setup() {
    // Сначала задаём безопасное состояние,
    // затем переводим GPIO в режим OUTPUT
    digitalWrite(RELAY_PIN, RELAY_OFF);
    pinMode(RELAY_PIN, OUTPUT);

    active = false;
    hasFinishedCycle = false;

    Serial.println("FOG INITIALIZED");
}

bool Trigger() {
    unsigned long now = millis();

    // Уже работает — не включаем повторно
    if (active) {
        return false;
    }

    // После предыдущего включения
    // обязательно выдерживаем паузу
    if (hasFinishedCycle &&
        now - stopTime < COOLDOWN) {
        return false;
    }

    digitalWrite(RELAY_PIN, RELAY_ON);

    active = true;
    startTime = now;

    Serial.println("FOG ON");

    return true;
}

void Stop() {
    digitalWrite(RELAY_PIN, RELAY_OFF);

    if (active) {
        active = false;
        stopTime = millis();
        hasFinishedCycle = true;

        Serial.println("FOG OFF");
    }
}

void Loop() {
    if (!active) {
        return;
    }

    if (millis() - startTime >= FOG_DURATION) {
        Stop();
    }
}

}
