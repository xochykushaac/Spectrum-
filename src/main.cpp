#include <Arduino.h>
#include "bluetooth.hpp"
#include "playback.hpp"
#include "strobe-test.hpp"

void setup() {

    Serial.begin(115200);

    startPlayback();
    startBluetooth();
    
}

void loop() {
    delay(100);
}