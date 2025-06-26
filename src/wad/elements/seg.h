#pragma once
#include "../structures.h"
#include "../../math/vector.h"
#include "../../math/angles.h"
#include <vector>


class Seg {
public:
    Seg() {}
    Seg(WAD::SegEntry entry) {
        m_entry = entry;
    }

    void updateSectors(uint32_t front, uint32_t back, bool backValid) {
        m_frontSectorID = front;
        m_backSectorID  = back;
        m_hacBackSector = backValid;
    }

    uint32_t getFrontSector() const {
        return m_frontSectorID;
    }
    bool hasBackSector() const { return m_hacBackSector; }
    uint32_t getBackSector() const {
        return m_backSectorID;
    }

    Math::Vec2 vbeg(const std::vector<Math::Vec2>& vecs) const {
        return vecs[m_entry.vbeg];
    }
    Math::Vec2 vend(const std::vector<Math::Vec2>& vecs) const {
        return vecs[m_entry.vend];
    }
    WAD::LinedefEntry linedef(const std::vector<WAD::LinedefEntry>& linedefs) const {
        return linedefs[m_entry.linedef];
    }
    
    WAD::SegEntry getEntry() const { return m_entry; }
    void setAngleRad(float angle) { m_angleRad = angle; }
    float getAngleRad() const { return m_angleRad; }


private:
    uint32_t m_frontSectorID;
    uint32_t m_backSectorID;
    float m_angleRad;
    bool m_hacBackSector = false;
    WAD::SegEntry m_entry;
};