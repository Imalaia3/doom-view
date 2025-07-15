#include "bsp.h"

BSPRenderer::BSPRenderer(WADMap& map, SDLWindow &renderTarget) : m_map(map), m_target(renderTarget) {    
    m_player = Thing(map.findThingByType(Thing::ThingType::PLAYERONE));
    m_nodes = m_map.getNodes();
    m_player.setAngleDegrees(90);
    m_FOV = Math::radians(90); // 90 degrees as per the doom wiki
    m_clipangle = m_FOV/2.0f;

    // Calculate vertex ranges
    m_minX = std::numeric_limits<float>::max();
    m_maxX = std::numeric_limits<float>::lowest();
    m_minY = std::numeric_limits<float>::max();
    m_maxY = std::numeric_limits<float>::lowest();
    for (const auto& vert : map.getVertices()) {
        m_minX = std::min(m_minX, vert.x);
        m_maxX = std::max(m_maxX, vert.x);
        m_minY = std::min(m_minY, vert.y);
        m_maxY = std::max(m_maxY, vert.y);
    }
    calculateAngleTable();
}

void BSPRenderer::drawFrame() {
    m_overlaps.clear();
    m_count = 0;
    void* drawPixels = m_target.renderBegin();
    m_target.clearPixels(drawPixels); // for debugging

    auto& verts = m_map.getVertices();
    for (const auto& def : m_map.getLinedefs()) {
        //todo: implement this in LinedefEntry
        auto v1 = worldToScreen2D(Math::Vec2(verts[def.v1].x, verts[def.v1].y));
        auto v2 = worldToScreen2D(Math::Vec2(verts[def.v2].x, verts[def.v2].y));
        m_target.drawLine(v1.x, v1.y, v2.x, v2.y, 0xFF, 0xFF, 0xFF, drawPixels);
    }

    auto playerScreenPos = worldToScreen2D(m_player.position);
    m_target.drawRectFilled(playerScreenPos.x - 2.0f, playerScreenPos.y - 2.0f, 4, 4, 0x00, 0xFF, 0xFF, drawPixels);

    // Draw player frustum
    printf("Player Angle: %f deg\n", m_player.getAngleDegrees());
    printf("Player FOV: %f rad\n", m_FOV);
    {
        float s = std::sin(m_FOV/2.0f + m_player.getAngleRadians());
        float c = std::cos(m_FOV/2.0f + m_player.getAngleRadians());
        auto p1 = worldToScreen2D(Math::Vec2(
            m_player.position.x + c * 90000, m_player.position.y + s * 90000
        ));
        m_target.drawLine(playerScreenPos.x, playerScreenPos.y,
            p1.x, p1.y,           
            0xFF, 0xFF, 0x00, drawPixels
        );
    }
    {
        float s = std::sin(-m_FOV/2.0f + m_player.getAngleRadians());
        float c = std::cos(-m_FOV/2.0f + m_player.getAngleRadians());
        auto p1 = worldToScreen2D(Math::Vec2(
            m_player.position.x + c * 90000, m_player.position.y + s * 90000
        ));
        m_target.drawLine(playerScreenPos.x, playerScreenPos.y,
            p1.x, p1.y,           
            0xFF, 0xFF, 0x00, drawPixels
        );
    }

    traverseBSP(m_nodes.size() - 1, drawPixels);

    m_target.renderEnd();
    m_target.updateWindow();
}

bool BSPRenderer::calculateSide(WAD::NodeEntry &node) {
    float product = (m_player.position.x - node.splitterX) * (float)(node.splitterDeltaY) -
        (m_player.position.y - node.splitterY) * (float)(node.splitterDeltaX);
    return (product > 0.0f);
}

void BSPRenderer::calculateAngleTable() {
    // https://github.com/id-Software/DOOM/blob/master/linuxdoom-1.10/r_main.c#L579
    int32_t viewwidth = m_target.getWidth();
    xtoviewangle.resize(viewwidth + 1);
    for (int x = 0; x <= viewwidth; x++) {
        // solve x = ((m_target.getWidth()-1)/2)*(1.0f + tan(angle)/(tan(-m_clipangle))) for angle:
        float angle = std::atan(std::tan(m_clipangle)*(1.0f - ((2.0f * (float)x) / (viewwidth - 1.0f))));
        xtoviewangle[x] = angle;
    }
}

float BSPRenderer::scaleFromGlobalAngle(float x, float rw_normalangle, float rw_distance) {
    float viewangle = m_player.getAngleRadians();
    float startangle = xtoviewangle[x];
    // float visangle = viewangle + startangle;
    float projection = (m_target.getWidth() / 2.0f) + std::tan(m_clipangle); // if this was doom it would just be width/2.0 but here fov isn't always 90
    float numerator = projection * std::cos(rw_normalangle - (viewangle + startangle)); // DOOM: angleb = ANG90 + (visangle-rw_normalangle); Inverting for cos()
    float denominator = rw_distance * std::cos(startangle); // DOOM: anglea = ANG90 + (visangle-viewangle); Inverting for cos()
    // max = 64*FRACUNIT, min = 256, fracunit = (1 << 16) fixed point -> below
    return std::clamp(numerator / denominator, 0.00390625f, 64.0f);
}

bool BSPRenderer::insideFrustum(Math::BoundingBox bbox) {
    // 1 = hi 0 = lo
    //v00, v10, v01, v11
    // https://github.com/id-Software/DOOM/blob/a77dfb96cb91780ca334d0d4cfd86957558007e0/linuxdoom-1.10/r_bsp.c#L365
    constexpr uint8_t lut[9][2] = {
        {3, 0},
        {3, 2},
        {1, 2},

        {2, 0},
        {0, 0},
        {3, 1},
        
        {2, 1},
        {0, 1},
        {0, 3}
    };
    const Math::Vec2 points[4] = {
        Math::Vec2(bbox.x1, bbox.y1), // v00
        Math::Vec2(bbox.x2, bbox.y1), // v10
        Math::Vec2(bbox.x1, bbox.y2), // v01
        Math::Vec2(bbox.x2, bbox.y2)  // v11
    };

    uint8_t lutx, luty;
    if (m_player.position.x <= bbox.x1)
	    lutx = 0;
    else if (m_player.position.x < bbox.x2)
	    lutx = 1;
    else
	    lutx = 2;
		
    if (m_player.position.y >= bbox.y2)
	    luty = 0;
    else if (m_player.position.y > bbox.y1)
        luty = 1;
    else
        luty = 2;

    if (lutx == 1 && luty == 1) { return true; }
    
    float a1 = Math::normalizeRad(toAngle(points[lut[luty*3+lutx][0]]) - m_player.getAngleRadians());
    float a2 = Math::normalizeRad(toAngle(points[lut[luty*3+lutx][1]]) - m_player.getAngleRadians());
    if (a1 < -m_clipangle && a2 < -m_clipangle)
        return false;
    if (a1 > m_clipangle && a2 > m_clipangle)
        return false;
    return true;
}

void BSPRenderer::traverseBSP(int16_t bspNodeID, void *pixels)
{
    // Check if a leaf node (subsector) was found (traverseBSP was called from another BSP node)
    constexpr uint16_t SUBSECTOR_IDENTIFIER = (1 << 15);
    if (bspNodeID & SUBSECTOR_IDENTIFIER) {
        uint16_t subsectorIndex = bspNodeID & (~SUBSECTOR_IDENTIFIER);
        renderSubsector(subsectorIndex, pixels);
        return;
    }
    auto node = m_nodes[bspNodeID];
    
    bool frontSide = calculateSide(node);

    // Front space
    // https://github.com/id-Software/DOOM/blob/a77dfb96cb91780ca334d0d4cfd86957558007e0/linuxdoom-1.10/r_defs.h#L265
    traverseBSP(!frontSide ? node.leftChild : node.rightChild, pixels);
    
    // Back Space
    if (insideFrustum(frontSide ? node.getLeftBox() : node.getRightBox())) {
        traverseBSP(frontSide ? node.leftChild : node.rightChild, pixels);
        
    } else {
            auto box = frontSide ? node.getLeftBox() : node.getRightBox();
            auto p1 = worldToScreen2D(Math::Vec2(box.x1, box.y1));
            auto p2 = worldToScreen2D(Math::Vec2(box.x2, box.y2));

            m_target.drawRectHollow(
                p1.x, p1.y,
                p2.x, p2.y,
                0xFF, 0x00, 0x00, pixels
            );
    }
}

inline int32_t BSPRenderer::viewAngleToX(float angle) {
    // could also be 1 - tan(angle)/tan(clipangle) but that makes it more difficult to
    // understand that when angle -FOV/2 the result is 0.
    return ((m_target.getWidth()-1)/2)*(1.0f + tan(angle)/(tan(-m_clipangle)));
}

bool BSPRenderer::isSegVisible(Seg &seg) {
    float a1 = Math::normalizeRad(toAngle(seg.vbeg(m_map.getVertices())) - m_player.getAngleRadians());
    float a2 = Math::normalizeRad(toAngle(seg.vend(m_map.getVertices())) - m_player.getAngleRadians());
    // Backface culling. Although logically shouln't it be > 0.0?
    if (a1 - a2 < 0.0)
        return false;
    // Frustum Culling
    if (a1 < -m_clipangle && a2 < -m_clipangle)
        return false;
    if (a1 > m_clipangle && a2 > m_clipangle)
        return false;
    return true;
}

void BSPRenderer::drawSeg(Seg seg, void *pixels) {
    // TODO: MOVE THIS TO seg.h?
    if (!isSegVisible(seg) || seg.hasBackSector()) {
        return;
    }
    auto& verts = m_map.getVertices();
    auto& defs = m_map.getLinedefs();
    auto& sdefs = m_map.getSidedefs();
    Math::Vec2 vbeg = seg.vbeg(verts);
    WAD::SectorEntry& frontsector = m_map.getSectors()[seg.getFrontSector()];
    float a1 = clipAngle(toAngle(vbeg) - m_player.getAngleRadians()); // = rw_angle1
    float a2 = clipAngle(toAngle(seg.vend(verts)) - m_player.getAngleRadians());
    int32_t x1 = viewAngleToX(a1);
    int32_t x2 = viewAngleToX(a2);

    // https://github.com/id-Software/DOOM/blob/master/linuxdoom-1.10/r_segs.c#L370
    float rw_normalangle = seg.getAngleRad() + M_PI_2; // angle + 90deg
    float offsetangle = rw_normalangle - a1;
    float hyp = Math::distance(m_player.position, vbeg);
    float rw_distance = hyp * std::sin(offsetangle); // doom does sin(pi/2 - offsetangle) which is equal to sin(offsetangle)

    float rw_scale = scaleFromGlobalAngle(x1, rw_normalangle, rw_distance);
    float rw_scalestep = 0.0f;
    if (x2 > x1) {
        float scale2 = scaleFromGlobalAngle(x2, rw_normalangle, rw_distance);
        rw_scalestep = (scale2 - rw_scale) / (x2 - x1); 
    }

    float worldtop = frontsector.ceilHeight - PLAYER_VIEWHEIGHT;
    float worldbottom = frontsector.floorHeight - PLAYER_VIEWHEIGHT;
    float height_2 = m_target.getHeight() / 2.0f;

    float origin_y1 = height_2 - worldtop * rw_scale;
    float step_y1 = -rw_scalestep * worldtop;
    float origin_y2 = height_2 - worldbottom * rw_scale;
    float step_y2 = -rw_scalestep * worldbottom;

    printf("Front Sidedef Info:\n");
    printf("\tUpper Texture:  %.8s\n", sdefs[seg.linedef(defs).frontSidedef].upperTex);
    printf("\tMiddle Texture: %.8s\n", sdefs[seg.linedef(defs).frontSidedef].middleTex);
    printf("\tLower Texture:  %.8s\n", sdefs[seg.linedef(defs).frontSidedef].lowerTex);

    // check if middle texture is available
    bool hasMiddle = sdefs[seg.linedef(defs).frontSidedef].middleTex[0] !=  WADFile::NO_TEXTURE;
    m_count = sdefs[seg.linedef(defs).frontSidedef].middleTex[0];
    auto wallCallback = [this, pixels, &origin_y1, &origin_y2, step_y1, step_y2, hasMiddle](int32_t s, int32_t e) {
        if (hasMiddle) {      
            for (int32_t x = s; x <= e; x++) {
                m_target.verticalLine(x, origin_y1 - 1, origin_y2, m_count & 0xF, m_count & 0xF0, m_count, pixels);
                origin_y1 += step_y1;
                origin_y2 += step_y2;
            }
        } else {
            printf("Walls without middle textures are not supported.\n");
        }
    };
    m_overlaps.addWall(OverlapManager::Interval(x1, x2), wallCallback);
}

void BSPRenderer::renderSubsector(uint16_t subsectorID, void *pixels) {
    auto subsector = m_map.getSubsectors()[subsectorID];
    auto& segs = m_map.getSegs();
    for (size_t i = 0; i < static_cast<uint16_t>(subsector.segCount); i++) {
        auto seg = segs[subsector.firstSeg + i];        
        drawSeg(seg, pixels);
    }
}

Math::Vec2 BSPRenderer::worldToScreen2D(Math::Vec2 in)
{
    return Math::Vec2(
        ((m_target.getWidth() - 1) / (m_maxX - m_minX)) * (in.x - m_minX),
        -((m_target.getHeight() - 1) / (m_maxY - m_minY)) * (in.y - m_maxY)
    );
}