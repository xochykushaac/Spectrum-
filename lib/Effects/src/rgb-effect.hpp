
#pragma once

namespace rgb_effect {

void Setup();

void SetMusic(
    float centroidHz,
    float energy,
    float activity,
    bool playing
);

void Loop();

}  // namespace rgb_effect
