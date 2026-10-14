#include <Arduino.h>
#include "bluetooth.hpp"
#include "playback.hpp"

void setup() {
    Serial.begin(115200);
    startBluetooth();
    startPlayback();
}

void loop() {
    delay(100);
}