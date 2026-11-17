#include "rgb-test.hpp"
#include <Arduino.h>

namespace rgb_test {

const int PIN_RED = 16;
const int PIN_GREEN = 27;

void Setup() {
    ledcSetup(1, 5000, 8);
    ledcAttachPin(PIN_RED, 1);

    ledcSetup(2, 5000, 8);
    ledcAttachPin(PIN_GREEN, 2);

    ledcWrite(1, 0);
    ledcWrite(2, 0);
}

void Loop() {
    // Красный
    ledcWrite(1, 255);
    ledcWrite(2, 0);
    delay(1000);

    // Зелёный
    ledcWrite(1, 0);
    ledcWrite(2, 255);
    delay(1000);

    // Красный и зелёный одновременно
    ledcWrite(1, 255);
    ledcWrite(2, 255);
    delay(1000);

    // Выключаем оба
    ledcWrite(1, 0);
    ledcWrite(2, 0);
    delay(1000);
}

}  // namespace rgb_test

