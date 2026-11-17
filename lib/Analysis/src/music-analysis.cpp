// centroidHz управляет цветом.
// energy управляет яркостью.
// activity управляет скоростью перехода между цветами.

#include "music-analysis.hpp"

#include <Arduino.h>
#include <arduinoFFT.h>
#include <math.h>

namespace music_analysis {

constexpr int FFT_SIZE = 512;

float realPart[FFT_SIZE];
float imagPart[FFT_SIZE];
float previousSpectrum[FFT_SIZE / 2] = {};

Features Analyze(
    const int16_t* pcm,
    size_t frames,
    uint32_t sampleRate,
    uint8_t channels
) {
    Features result = {};

    if (pcm == nullptr ||
        frames != FFT_SIZE ||
        sampleRate == 0 ||
        (channels != 1 && channels != 2)) {
        return result;
    }

    float squareSum = 0;

    for (int i = 0; i < FFT_SIZE; ++i) {
        float sample = 0;

        for (int c = 0; c < channels; ++c) {
            sample += pcm[i * channels + c];
        }

        sample /= channels;

        realPart[i] = sample;
        imagPart[i] = 0;

        squareSum += sample * sample;
    }

    // RMS: оценка энергии звукового сигнала
    float rms = sqrtf(squareSum / FFT_SIZE);
    result.energy = constrain(
        rms / 32768.0f * 5.0f,
        0.0f,
        1.0f
    );

    ArduinoFFT<float> fft(
        realPart,
        imagPart,
        FFT_SIZE,
        static_cast<float>(sampleRate)
    );

    fft.dcRemoval();
    fft.windowing(
        FFTWindow::Hamming,
        FFTDirection::Forward
    );
    fft.compute(FFTDirection::Forward);
    fft.complexToMagnitude();

    float weightedSum = 0;
    float magnitudeSum = 0;
    float positiveChange = 0;

    for (int i = 1; i < FFT_SIZE / 2; ++i) {
        float frequency =
            static_cast<float>(i) * sampleRate /
            FFT_SIZE;

        if (frequency < 120.0f ||
            frequency > 8000.0f) {
            continue;
        }

        float magnitude = realPart[i];

        weightedSum += frequency * magnitude;
        magnitudeSum += magnitude;

        // Измеряем появление новой спектральной энергии
        float difference =
            magnitude - previousSpectrum[i];

        if (difference > 0) {
            positiveChange += difference;
        }

        previousSpectrum[i] = magnitude;
    }

    // Спектральный центроид
    if (magnitudeSum > 0.001f) {
        result.centroidHz =
            weightedSum / magnitudeSum;

        // Приближённая оценка музыкальной активности
        result.activity = constrain(
            4.0f * positiveChange /
                magnitudeSum,
            0.0f,
            1.0f
        );
    }

    return result;
}

}
