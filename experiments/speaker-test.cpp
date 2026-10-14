
#include <Arduino.h>
#include <driver/i2s.h>
#include <math.h>

#define I2S_BCLK 26
#define I2S_LRC  25
#define I2S_DOUT 22

#define SAMPLE_RATE 44100

void playTone(int frequency, int durationMs) {
    const int chunkSize = 256;
    int16_t samples[chunkSize * 2];

    int totalFrames = SAMPLE_RATE * durationMs / 1000;
    float phase = 0.0f;
    float phaseStep = 2.0f * PI * frequency / SAMPLE_RATE;

    while (totalFrames > 0) {
        int frames = min(totalFrames, chunkSize);

        for (int i = 0; i < frames; i++) {
            int16_t sample = 0;

            if (frequency > 0) {
                sample = (int16_t)(12000.0f * sinf(phase));
                phase += phaseStep;

                if (phase >= 2.0f * PI) {
                    phase -= 2.0f * PI;
                }
            }

            samples[2 * i] = sample;
            samples[2 * i + 1] = sample;
        }

        size_t written;
        i2s_write(
            I2S_NUM_0,
            samples,
            frames * 2 * sizeof(int16_t),
            &written,
            portMAX_DELAY
        );

        totalFrames -= frames;
    }
}

void setup() {
Serial.begin(115200);
delay(1000);
Serial.println("ESP32 STARTED");

    i2s_config_t config = {};
    config.mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX);
    config.sample_rate = SAMPLE_RATE;
    config.bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT;
    config.channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT;
    config.communication_format = I2S_COMM_FORMAT_STAND_I2S;
    config.intr_alloc_flags = 0;
    config.dma_buf_count = 8;
    config.dma_buf_len = 256;
    config.use_apll = false;
    config.tx_desc_auto_clear = true;

    i2s_pin_config_t pins = {};
    pins.bck_io_num = I2S_BCLK;
    pins.ws_io_num = I2S_LRC;
    pins.data_out_num = I2S_DOUT;
    pins.data_in_num = I2S_PIN_NO_CHANGE;

    i2s_driver_install(I2S_NUM_0, &config, 0, NULL);
    i2s_set_pin(I2S_NUM_0, &pins);

Serial.println("I2S INITIALIZED");

    i2s_zero_dma_buffer(I2S_NUM_0);
}

void loop() {

Serial.println("PLAYING MELODY");

    playTone(262, 300);  // До
    playTone(330, 300);  // Ми
    playTone(392, 300);  // Соль
    playTone(523, 600);  // До

    playTone(0, 500);    // Пауза
}
