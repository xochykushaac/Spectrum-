#include "bluetooth.hpp"
#include "playback.hpp"
#include "BluetoothA2DPSink.h"

void startBluetooth() {
    static BluetoothA2DPSink a2dp_sink(playbackStream());
    a2dp_sink.start("Spectrum");
}