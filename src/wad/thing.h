#pragma once
#include "structures.h"
#include "../math/vector.h"

struct Thing {
    Thing(WAD::ThingEntry entry) {
        position = Math::Vec2(entry.x, entry.y);
        andgleDeg = static_cast<float>(entry.angle);
        flags = entry.flags;
    }

    bool inDifficulty(uint8_t level) const;
    bool shouldAmbush() const{ return (flags & 0x0008) != 0; }
    bool inSingleplayer() const { return (flags & 0x0010) == 0; }

    Math::Vec2 position;
    float andgleDeg;
    uint16_t flags;
};