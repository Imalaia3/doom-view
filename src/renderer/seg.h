#pragma once
#include "../wad/structures.h"
#include "../wad/elements/thing.h"
#include "../math/vector.h"
#include "../math/angles.h"
#include "../math/projection.h"
#include "../utils.h"
#include "../display/window.h"
#include "overlap.h"
#include <vector>


class Seg {
public:
    Seg() {}
    Seg(WAD::SegEntry entry, const std::vector<Math::Vec2>& verts, const std::vector<WAD::SectorEntry>& sectors, const std::vector<WAD::LinedefEntry>& ldefs, const std::vector<WAD::SidedefEntry>&  sdefs);

    struct RendererState {
        const Thing& player;
        OverlapManager& overlaps;
        float clipangle;
        float fov;
        const std::vector<WAD::SidedefEntry>& sidedefs; // todo: maybe just give our sidedef?
        const std::vector<float>& xtoviewangle; // todo: make shared_ptr for all seg/etc. instances
    };

    void drawSolid(const RendererState& state, SDLWindow& target, void* pixels);
    void drawHollow(const RendererState& state, SDLWindow& target, void* pixels);

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
    bool isVisible(float& rw_angle1, float& angle1, float& angle2, const Math::Vec2& viewpos, float viewangle, float clipangle);


private:
    // startangle = state.xtoviewangle[x]
    float scaleFromGlobalAngle(float startangle, float rw_normalangle, float rw_distance, float viewangle, float clipangle, uint32_t viewwidth);

    float m_angleRad;
    bool m_hasBackSector = false;
    WAD::SegEntry m_entry;
    Math::Vec2 m_vbeg, m_vend;
    WAD::SectorEntry m_frontSector, m_backSector;
    WAD::LinedefEntry m_linedef;
};