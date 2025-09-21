#pragma once

#include <stdint.h>
#include "btController.h"

namespace BTAnimations {
    struct Frame {
        uint8_t red;
        uint8_t green;
        uint8_t blue;
        uint32_t duration; // Duration in milliseconds
    };

    void Animate();
    void StopAnimation();
    void StartAnimation(uint8_t animation);
}