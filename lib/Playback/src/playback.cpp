#include "playback.hpp"

I2SStream& playbackStream() {
    static I2SStream i2s;
    return i2s;
}

void startPlayback() {
    I2SStream& i2s = playbackStream();

    auto config = i2s.defaultConfig();

    config.pin_bck = 26;
    config.pin_ws = 25;
    config.pin_data = 22;

    i2s.begin(config);
}