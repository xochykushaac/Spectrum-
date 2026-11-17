#pragma once

namespace strip_effect {

void Setup();

void SetMusic(
    float centroidHz,
    float energy,
    float activity,
    bool playing
);

void Loop();

}