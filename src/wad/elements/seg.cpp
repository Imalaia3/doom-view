#include "seg.h"

Seg::Seg(WAD::SegEntry entry, const std::vector<Math::Vec2> &verts, const std::vector<WAD::SectorEntry> &sectors, const std::vector<WAD::LinedefEntry> &ldefs, const std::vector<WAD::SidedefEntry> &sdefs) {
    m_entry = entry;
    m_vbeg = verts[m_entry.vbeg];
    m_vend = verts[m_entry.vend];
    m_linedef = ldefs[m_entry.linedef];

    // Front is sidedef1 aka backSidedef
    uint32_t frontSidedefIdx = m_linedef.backSidedef;
    // uint32_t backSidedefIdx = m_linedef.frontSidedef; UNUSED
    if(entry.dir == 0) { // Front is sidedef0 aka frontSidedef
        frontSidedefIdx = m_linedef.frontSidedef;
        // backSidedefIdx = m_linedef.backSidedef; UNUSED
    }

    m_frontSector = sectors[sdefs[frontSidedefIdx].lookatSector]; // always has a front sector
    m_backSector = {};
    // check if it has a back sector by checking bit 2
    m_hasBackSector = false;
    constexpr uint16_t ML_TWOSIDED = 0x0004;
    if(m_linedef.flags & ML_TWOSIDED) {
        m_hasBackSector = true;
        m_backSector = sectors[sdefs[frontSidedefIdx].lookatSector];
    }

    // Convert angles
    setAngleRad((M_PI * (float)m_entry.angleBam)/(float)(32768));
}