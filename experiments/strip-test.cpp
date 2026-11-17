
#include "strip-test.hpp"

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

namespace strip_test {

constexpr int kLedPin = 19;
constexpr int kLedCount = 7;

Adafruit_NeoPixel strip(
    kLedCount,
    kLedPin,
    NEO_GRB + NEO_KHZ800
);

void Setup() {
    strip.begin();
    strip.setBrightness(30);
    strip.clear();
    strip.show();

    Serial.println("STRIP TEST STARTED");
}

void Loop() {
    // Все светодиоды красные
    strip.fill(strip.Color(255, 0, 0));
    strip.show();
    delay(1000);

    // Все светодиоды зелёные
    strip.fill(strip.Color(0, 255, 0));
    strip.show();
    delay(1000);

    // Все светодиоды синие
    strip.fill(strip.Color(0, 0, 255));
    strip.show();
    delay(1000);

    // Выключаем ленту
    strip.clear();
    strip.show();
    delay(500);

    // Последовательно зажигаем 7 светодиодов
    for (int i = 0; i < kLedCount; ++i) {
        strip.clear();
        strip.setPixelColor(
            i,
            strip.Color(255, 255, 255)
        );
        strip.show();
        delay(300);
    }

    strip.clear();
    strip.show();
    delay(1000);
}

}  // namespace strip_test

