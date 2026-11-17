// #include "bluetooth.hpp"
// #include "playback.hpp"
// #include "BluetoothA2DPSink.h"

// void startBluetooth() {
//     static BluetoothA2DPSink a2dp_sink(playbackStream());
//     a2dp_sink.start("Spectrum");
// }


#include "bluetooth.hpp"
#include "playback.hpp"
#include "pcm-buffer.hpp"

#include "BluetoothA2DPSink.h"

namespace {

void onPcmReceived(
    const uint8_t* data,
    uint32_t length
) {
    pcm_buffer::Push(data, length);
}

void onSampleRateChanged(uint16_t rate) {
    pcm_buffer::SetSampleRate(rate);
}

}

void startBluetooth() {
    static BluetoothA2DPSink a2dp_sink(
        playbackStream()
    );

    a2dp_sink.set_stream_reader(
        onPcmReceived,
        true
    );

    a2dp_sink.set_sample_rate_callback(
        onSampleRateChanged
    );

    a2dp_sink.start("Spectrum");
}
