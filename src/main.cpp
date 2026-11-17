#include <Arduino.h>
#include "bluetooth.hpp"
#include "playback.hpp"
// #include "strobe-test.hpp"
// #include "strip-test.hpp"
// #include "rgb-test.hpp"
#include "fog-test.hpp"

void setup() {
    Serial.begin(115200);
    delay(1000); // Wait for Serial to initialize

    // Serial.println("MAIN STARTED");
    // delay(1000); // Wait for Serial to initialize

    startPlayback();
    startBluetooth();

    // Serial.println("BEFORE RGB");
    // rgb_test::Setup();
    // Serial.println("RGB INITIALIZED");

    //strobe_test::Setup();
    //strip_test::Setup();

    // fog_test::Setup();
    
}

void loop() {
    delay(100);

    // fog_test::Loop();
    // rgb_test::Loop();
    // strip_test::Loop();
    // strobe_test::Loop();

    
}
