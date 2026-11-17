#include "strobe-test.hpp"
#include <Arduino.h>

const int PIN_WHITE = 32;

namespace strobe_test {

void Setup() {
    ledcSetup(0, 5000, 8); // Настраиваем ШИМ-каналы: канал, частота 5 кГц, разрешение 8 бит
    ledcAttachPin(PIN_WHITE, 0);
}

void Loop() {
    ledcWrite(0, 255);
    delay(500); // Полсекунды горит

    ledcWrite(0, 0);
    delay(500); // Полсекунды пауза
}

}