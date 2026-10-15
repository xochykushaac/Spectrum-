#include "strobe-test.hpp"
#include <Arduino.h>

const int PIN_RED = 32;
const int PIN_GREEN = 33;
const int PIN_BLUE = 27;

void run_strobe_test() {
    
    ledcSetup(0, 5000, 8); // Настраиваем ШИМ-каналы: канал, частота 5 кГц, разрешение 8 бит
    ledcSetup(1, 5000, 8);
    ledcSetup(2, 5000, 8);

    ledcAttachPin(PIN_RED, 0);
    ledcAttachPin(PIN_GREEN, 1);
    ledcAttachPin(PIN_BLUE, 2);

    while(true) {
        ledcWrite(0, 255);
        ledcWrite(1, 255);
        ledcWrite(2, 255);
        delay(500); // Полсекунды горит

        ledcWrite(0, 0);
        ledcWrite(1, 0);
        ledcWrite(2, 0);
        delay(500); // Полсекунды пауза
    }
}