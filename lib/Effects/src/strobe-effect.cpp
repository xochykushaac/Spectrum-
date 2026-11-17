
#include "strobe-effect.hpp"

#include <Arduino.h>

namespace strobe_effect {

constexpr int STROBE_PIN = 32;
constexpr int PWM_CHANNEL = 0;

constexpr unsigned long FLASH_DURATION = 35;

bool flashing = false;
unsigned long flashStart = 0;

void Setup() {
    ledcSetup(PWM_CHANNEL, 5000, 8);
    ledcAttachPin(STROBE_PIN, PWM_CHANNEL);

    ledcWrite(PWM_CHANNEL, 0);
}

void Trigger() {
    // Не запускаем вторую вспышку поверх первой
    if (flashing) {
        return;
    }

    flashing = true;
    flashStart = millis();

    ledcWrite(PWM_CHANNEL, 255);
}

void Stop() {
    ledcWrite(PWM_CHANNEL, 0);
    flashing = false;
}

void Loop() {
    if (!flashing) {
        return;
    }

    if (millis() - flashStart >= FLASH_DURATION) {
        Stop();
    }
}

}
