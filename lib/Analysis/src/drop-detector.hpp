
#pragma once

#include <stdint.h>

namespace drop_detector {

void Setup();
bool Process(float energy);
void Reset();

}
