#pragma once
#include <stdint.h>

namespace WAD {
    struct FileLump {
        uint32_t filepos;
        uint32_t size;
        char name[8];
    } __attribute__((packed));

    struct PatchHeader {
        uint16_t width;
        uint16_t height;
        uint16_t leftOffset;
        uint16_t topOffset;
    } __attribute__((packed));
}