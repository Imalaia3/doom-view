#pragma once
#include "../structures.h"
#include "../../math/vector.h"

class Thing {
public:
    Thing(WAD::ThingEntry entry) {
        position = Math::Vec2(entry.x, entry.y);
        andgleDeg = static_cast<float>(entry.angle);
        m_flags = entry.flags;
        m_type = entry.type;
    }

    bool inDifficulty(uint8_t level) const;
    bool shouldAmbush() const{ return (m_flags & 0x0008) != 0; }
    bool inSingleplayer() const { return (m_flags & 0x0010) == 0; }
    uint16_t getType() const { return m_type; }

    Math::Vec2 position;
    float andgleDeg;

    enum class ThingType {
        PLAYERONE = 1
    };

private:
    uint16_t m_type;
    uint16_t m_flags;
};