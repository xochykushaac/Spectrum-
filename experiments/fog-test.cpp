
#include "fog-test.hpp"
#include <Arduino.h>

namespace fog_test {

const int PIN_RELAY = 18;

void Setup() {
    digitalWrite(PIN_RELAY, LOW);
    pinMode(PIN_RELAY, OUTPUT);

    Serial.println("FOG TEST INITIALIZED");
}

void Loop() {
    // Включаем туман, удерживаем кнопку 3 секунды
    digitalWrite(PIN_RELAY, HIGH);
    Serial.println("FOG ON");
    delay(1200);

    // Выключаем туман, отпускаем кнопку
    digitalWrite(PIN_RELAY, LOW);
    Serial.println("FOG OFF");
    delay(1200);
}

}
