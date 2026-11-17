// #include <Arduino.h>
// #include "bluetooth.hpp"
// #include "playback.hpp"
// // #include "strobe-test.hpp"
// // #include "strip-test.hpp"
// // #include "rgb-test.hpp"
// #include "fog-test.hpp"

// void setup() {
//     Serial.begin(115200);
//     delay(1000); // Wait for Serial to initialize

//     // Serial.println("MAIN STARTED");
//     // delay(1000); // Wait for Serial to initialize

//     startPlayback();
//     startBluetooth();

//     // Serial.println("BEFORE RGB");
//     // rgb_test::Setup();
//     // Serial.println("RGB INITIALIZED");

//     //strobe_test::Setup();
//     //strip_test::Setup();

//     // fog_test::Setup();
    
// }

// void loop() {
//     delay(100);

//     // fog_test::Loop();
//     // rgb_test::Loop();
//     // strip_test::Loop();
//     // strobe_test::Loop();

    
// }



#include <Arduino.h>

#include "bluetooth.hpp"
#include "playback.hpp"

#include "pcm-buffer.hpp"
#include "music-analysis.hpp"
#include "beat-detector.hpp"
#include "drop-detector.hpp"

#include "strip-effect.hpp"
#include "rgb-effect.hpp"
#include "strobe-effect.hpp"
#include "fog-effect.hpp"

// Защита от запуска тумана в начале музыки
constexpr unsigned long FOG_START_DELAY = 12000;
constexpr unsigned long AUDIO_TIMEOUT = 1000;

void setup() {
    Serial.begin(115200);
    delay(1000);

    // Подготовка анализа
    pcm_buffer::Setup();
    beat_detector::Setup();
    drop_detector::Setup();

    // Подготовка эффектов
    strip_effect::Setup();
    rgb_effect::Setup();
    strobe_effect::Setup();
    fog_effect::Setup();

    // Воспроизведение
    startPlayback();
    startBluetooth();

    Serial.println("SPECTRUM STARTED");
}

void loop() {
    static pcm_buffer::Block block;

    static unsigned long lastAudioTime = 0;
    static unsigned long audioStartTime = 0;
    static unsigned long lastPrint = 0;

    static bool audioTimedOut = true;

    // --------------------------
    // 1. ПРОВЕРКА ПАУЗЫ В МУЗЫКЕ
    // --------------------------

    unsigned long now = millis();

    if (!audioTimedOut &&
        now - lastAudioTime > AUDIO_TIMEOUT) {

        audioTimedOut = true;

        strip_effect::SetMusic(
            120.0f, 0.0f, 0.0f, false
        );

        rgb_effect::SetMusic(
            120.0f, 0.0f, 0.0f, false
        );

        strobe_effect::Stop();
        beat_detector::Reset();
        drop_detector::Reset();
        fog_effect::Stop();
    }

    // --------------------------
    // 2. ОБРАБОТКА PCM
    // --------------------------

    if (pcm_buffer::Pop(block)) {
        now = millis();

        // Первый PCM после запуска или паузы
        if (audioTimedOut) {
            audioStartTime = now;
            audioTimedOut = false;

            drop_detector::Reset();
        }

        lastAudioTime = now;

        // Общий анализ музыкального сигнала
        auto features = music_analysis::Analyze(
            block.samples,
            pcm_buffer::FRAME_COUNT,
            block.sampleRate,
            pcm_buffer::CHANNELS
        );

        // 3. Адресная лента
        strip_effect::SetMusic(
            features.centroidHz,
            features.energy,
            features.activity,
            true
        );

        // 4. RGB-прожектор
        rgb_effect::SetMusic(
            features.centroidHz,
            features.energy,
            features.activity,
            true
        );

        // 5. Стробоскоп — басовые удары
        bool beat = beat_detector::Process(
            block.samples,
            pcm_buffer::FRAME_COUNT,
            block.sampleRate,
            pcm_buffer::CHANNELS
        );

        if (beat) {
            strobe_effect::Trigger();
        }

        // --------------------------
        // 6. ТУМАН
        // --------------------------

        // Детектор продолжает получать энергию,
        // но первые 12 секунд туман не запускаем
        bool drop = drop_detector::Process(
            features.energy
        );

        if (drop &&
            now - audioStartTime >= FOG_START_DELAY) {

            Serial.println("DROP DETECTED");
            fog_effect::Trigger();
        }

        // --------------------------
        // 7. ДИАГНОСТИКА
        // --------------------------

        if (now - lastPrint >= 1000) {
            lastPrint = now;

            Serial.printf(
                "Frequency: %.0f Hz | "
                "Energy: %.3f | "
                "Activity: %.2f\n",
                features.centroidHz,
                features.energy,
                features.activity
            );
        }
    }

    // --------------------------
    // 8. ОБНОВЛЕНИЕ ЭФФЕКТОВ
    // --------------------------

    strip_effect::Loop();
    rgb_effect::Loop();
    strobe_effect::Loop();
    fog_effect::Loop();

    delay(1);
}
 