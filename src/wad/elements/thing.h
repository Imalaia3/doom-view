#pragma once
#include "../structures.h"
#include "../../math/vector.h"
#include "../../math/angles.h"

class Thing {
public:
    Thing() {}
    Thing(WAD::ThingEntry entry) {
        position = Math::Vec2(entry.x, entry.y);
        m_angleDeg = static_cast<float>(entry.angleDeg);
        m_angleRad = Math::radians(m_angleDeg);
        m_flags = entry.flags;
        m_type = entry.type;
    }

    bool inDifficulty(uint8_t level) const;
    bool shouldAmbush() const{ return (m_flags & 0x0008) != 0; }
    bool inSingleplayer() const { return (m_flags & 0x0010) == 0; }
    uint16_t getType() const { return m_type; }
    float getAngleDegrees() const { return m_angleDeg; }
    float getAngleRadians() const { return m_angleRad; }
    void setAngleRadians(float radians) { m_angleRad = radians; m_angleDeg = Math::degrees(radians); }
    void setAngleDegrees(float degrees) { m_angleDeg = degrees; m_angleRad = Math::radians(degrees); }
    void setAngle(float radians, float degrees) { m_angleDeg = degrees; m_angleRad = radians; }

    Math::Vec2 position;
    enum class ThingType {
        PLAYERONE = 1
    };
    static constexpr float PLAYER_VIEWHEIGHT = 41; // Player body height is 56 but eyes are at 41. https://doomwiki.org/wiki/Player

private:
    float m_angleRad, m_angleDeg;
    uint16_t m_type;
    uint16_t m_flags;
};