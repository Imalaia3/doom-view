#pragma once
#include "../structures.h"
#include "../../math/vector.h"
#include "../../math/angles.h"
#include <vector>


class Seg {
public:
    Seg() {}
    Seg(WAD::SegEntry entry, const std::vector<Math::Vec2>& verts, const std::vector<WAD::SectorEntry>& sectors, const std::vector<WAD::LinedefEntry>& ldefs, const std::vector<WAD::SidedefEntry>&  sdefs);

    inline WAD::SectorEntry frontSector() const {
        return m_frontSector;
    }
    bool hasBackSector() const { return m_hasBackSector; }
    inline WAD::SectorEntry backSector() const {
        return m_backSector;
    }

    inline Math::Vec2 vbeg() const {
        return m_vbeg;
    }
    inline Math::Vec2 vend() const {
        return m_vend;
    }
    inline WAD::LinedefEntry linedef() const {
        return m_linedef;
    }
    
    WAD::SegEntry getEntry() const { return m_entry; }
    void setAngleRad(float angle) { m_angleRad = angle; }
    float getAngleRad() const { return m_angleRad; }


private:
    float m_angleRad;
    bool m_hasBackSector = false;
    WAD::SegEntry m_entry;
    Math::Vec2 m_vbeg, m_vend;
    WAD::SectorEntry m_frontSector, m_backSector;
    WAD::LinedefEntry m_linedef;
};