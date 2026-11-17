#include "rgb-test.hpp"
#include <Arduino.h>

namespace rgb_test {

const int PIN_RED = 16;
const int PIN_GREEN = 27;
const int PIN_BLUE = 33;

void Setup() {
    ledcSetup(1, 5000, 8);
    ledcAttachPin(PIN_RED, 1);

    ledcSetup(2, 5000, 8);
    ledcAttachPin(PIN_GREEN, 2);

    ledcSetup(3, 5000, 8);
    ledcAttachPin(PIN_BLUE, 3);

    ledcWrite(1, 0);
    ledcWrite(2, 0);
    ledcWrite(3, 0);
}

void Loop() {
    // Красный
    ledcWrite(1, 255);
    ledcWrite(2, 0);
    ledcWrite(3, 0);
    delay(1000);

    // Зелёный
    ledcWrite(1, 0);
    ledcWrite(2, 255);
    ledcWrite(3, 0);
    delay(1000);

    // Синий
    ledcWrite(1, 0);
    ledcWrite(2, 0);
    ledcWrite(3, 255);
    delay(1000);

    // Жёлтый: красный + зелёный
    ledcWrite(1, 255);
    ledcWrite(2, 255);
    ledcWrite(3, 0);
    delay(1000);

    // Фиолетовый: красный + синий
    ledcWrite(1, 255);
    ledcWrite(2, 0);
    ledcWrite(3, 255);
    delay(1000);

    // Голубой: зелёный + синий
    ledcWrite(1, 0);
    ledcWrite(2, 255);
    ledcWrite(3, 255);
    delay(1000);

    // Все три канала
    ledcWrite(1, 255);
    ledcWrite(2, 255);
    ledcWrite(3, 255);
    delay(1000);

    // Выключаем
    ledcWrite(1, 0);
    ledcWrite(2, 0);
    ledcWrite(3, 0);
    delay(1000);
}

}  // namespace rgb_test
