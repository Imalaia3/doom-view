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

void Seg::draw(const RendererState& state, SDLWindow& target, void* pixels) {
    float rw_angle1;
    float a1;
    float a2;
    if (!isVisible(rw_angle1, a1, a2, state.player.position, state.player.getAngleRadians(), state.clipangle)) {
        return;
    }
    a1 = std::clamp(static_cast<float>(a1), -state.clipangle, state.clipangle); // != rw_angle1 (!!!)
    a2 = std::clamp(static_cast<float>(a2), -state.clipangle, state.clipangle);


    uint32_t viewwidth = target.getWidth();
    float viewangle = state.player.getAngleRadians(); // todo: give access to m_angleRad directly
    auto& sdefs = state.sidedefs;

    int32_t x1 = Math::viewAngleToX(a1, state.clipangle, viewwidth);
    int32_t x2 = Math::viewAngleToX(a2, state.clipangle, viewwidth);

    // https://github.com/id-Software/DOOM/blob/master/linuxdoom-1.10/r_segs.c#L370
    float rw_normalangle = m_angleRad + M_PI_2; // angle + 90deg
    float offsetangle = std::fabs(rw_normalangle - rw_angle1);
    float hyp = Math::distance(state.player.position, m_vbeg);
    float rw_distance = hyp * std::cos(offsetangle); // doom does sin(pi/2 - offsetangle) which is equal to cos(offsetangle)

    float rw_scale = scaleFromGlobalAngle(state.xtoviewangle[x1], rw_normalangle, rw_distance, viewangle, state.clipangle, viewwidth);
    float rw_scalestep = 0.0f;
    if (x2 > x1) {
        float scale2 = scaleFromGlobalAngle(state.xtoviewangle[x2], rw_normalangle, rw_distance, viewangle, state.clipangle, viewwidth);
        rw_scalestep = (scale2 - rw_scale) / (x2 - x1);
    }

    float worldtop = m_frontSector.ceilHeight - Thing::PLAYER_VIEWHEIGHT;
    float worldbottom = m_frontSector.floorHeight - Thing::PLAYER_VIEWHEIGHT;
    float height_2 = target.getHeight() / 2.0f;

    float origin_y1 = height_2 - worldtop * rw_scale;
    float step_y1 = -rw_scalestep * worldtop;
    float origin_y2 = height_2 - worldbottom * rw_scale;
    float step_y2 = -rw_scalestep * worldbottom;

    // check if middle texture is available
    bool hasMiddle = sdefs[m_linedef.frontSidedef].middleTex[0] !=  '-'; // NOTE: Maybe make - a const?

    // just so it looks pretty
    auto hash_texture = [](const char* texname) {
        uint32_t result = 0x00;
        for (size_t i = 0; i < 8; i++) {
            if (texname[i] == '\0') break;
            // https://stackoverflow.com/questions/2351087/what-is-the-best-32bit-hash-function-for-short-strings-tag-names/2351171#2351171
            result = 37 * result + texname[i];
        }
        return result;
    };

    uint32_t drawColor = hash_texture(sdefs[m_linedef.frontSidedef].middleTex) * (abs(m_entry.angleBam)+1);
    auto wallCallback = [this, pixels, &origin_y1, &origin_y2, step_y1, step_y2, hasMiddle, &target, drawColor](int32_t s, int32_t e) {
        if (hasMiddle) {
            for (int32_t x = s; x <= e; x++) {
                target.verticalLine(x, Utils::ftou32_safe(origin_y1 - 1), Utils::ftou32_safe(origin_y2),
                    drawColor & 0xFF, (drawColor & 0xFF00) >> 8,(drawColor & 0xFF0000) >> 16, pixels
                );
                origin_y1 += step_y1;
                origin_y2 += step_y2;
            }
        } else {
            printf("Walls without middle textures are not supported.\n");
        }
    };
    state.overlaps.addWall(OverlapManager::Interval(x1, x2), wallCallback);
}

float Seg::scaleFromGlobalAngle(float startangle, float rw_normalangle, float rw_distance, float viewangle, float clipangle, uint32_t viewwidth) {
    // float visangle = viewangle + startangle;
    float projection = (viewwidth / 2.0f) / std::tan(clipangle); // if this was doom it would just be width/2.0 but here fov isn't always 90
    float numerator = projection * std::cos(rw_normalangle - (viewangle + startangle)); // DOOM: angleb = ANG90 + (visangle-rw_normalangle); Inverting for cos()
    float denominator = rw_distance * std::cos(startangle); // DOOM: anglea = ANG90 + (visangle-viewangle); Inverting for cos()
    // max = 64*FRACUNIT, min = 256, fracunit = (1 << 16) fixed point -> below
    return std::clamp(numerator / denominator, 0.00390625f, 64.0f);
}

bool Seg::isVisible(float &rw_angle1, float &angle1, float &angle2, const Math::Vec2 &viewpos, float viewangle, float clipangle) {
    // Custom (cleaner and faster(?)) version.
    // Vector based backface culling. 1 less atan2 and normalizeRad.
    auto dir = m_vend - m_vbeg;
    auto n = Math::Vec2(-dir.y, dir.x); // this works for some reason
    auto v = Math::Vec2((m_vbeg.x + m_vend.x)/2.0f - viewpos.x, (m_vbeg.y + m_vend.y)/2.0f - viewpos.y);
    auto ndotv = Math::dot(n, v);
    if (ndotv < 0.0f) {
        return false;
    }
    // Angle based frustum culling
    rw_angle1 = Math::pointToAngle(m_vbeg, viewpos);
    angle1 = Math::normalizeRad(rw_angle1 - viewangle);
    angle2 = Math::normalizeRad(Math::pointToAngle(m_vend, viewpos) - viewangle);
    //FIXME: this fill fail on walls that take up the whole frustum. Fix edge case!
    if (std::fabs(angle1) > clipangle && std::fabs(angle2) > clipangle) {
        return false;
    }
    return true;
}