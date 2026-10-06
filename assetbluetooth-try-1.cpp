#include "AudioTools.h"
#include "BluetoothA2DPSink.h"

I2SStream i2s;
BluetoothA2DPSink a2dp_sink(i2s);

void setup() {
    Serial.begin(115200);

    auto config = i2s.defaultConfig();

    config.pin_bck  = 14;
    config.pin_ws   = 15;
    config.pin_data = 22;

    i2s.begin(config);

    a2dp_sink.start("Spectrum");
}

void loop() {
}
