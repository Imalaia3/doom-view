#include "bsp.h"

BSPRenderer::BSPRenderer(WADMap& map, SDLWindow &renderTarget) : m_map(map), m_target(renderTarget), m_overlaps(renderTarget.getWidth(), renderTarget.getHeight()) {    
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
    m_overlaps.clear(m_target.getWidth(), m_target.getHeight());
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
    
    float a1 = Math::normalizeRad(Math::pointToAngle(points[lut[luty*3+lutx][0]], m_player.position) - m_player.getAngleRadians());
    float a2 = Math::normalizeRad(Math::pointToAngle(points[lut[luty*3+lutx][1]], m_player.position) - m_player.getAngleRadians());
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

void BSPRenderer::drawSeg(Seg seg, void *pixels) {
    // Classify whether the wall is completely solid or partially hollow
    auto state = Seg::RendererState {
        .player = m_player,
        .overlaps = m_overlaps,
        .clipangle = m_clipangle,
        .fov = m_FOV,
        .sidedefs = m_map.getSidedefs(),
        .xtoviewangle = xtoviewangle
    };
    auto frontsector = seg.frontSector();
    auto backsector = seg.backSector();

    // Entirely solid
    if (!seg.hasBackSector()) {
        seg.drawSolid(state, m_target, pixels);
        return;
    }
    // Closed door.
    if (backsector.ceilHeight <= frontsector.floorHeight || backsector.floorHeight >= frontsector.ceilHeight) {
        seg.drawSolid(state, m_target, pixels);
        return;
    }
    // Window.
    if (backsector.ceilHeight != frontsector.ceilHeight || backsector.floorHeight != frontsector.floorHeight) {
        seg.drawHollow(state, m_target, pixels);
        return;
    }

    // Triggers (or other invisible walls/seperators).
    if (std::strncmp(backsector.ceilTex, frontsector.ceilTex, 8) == 0
    && std::strncmp(backsector.floorTex, frontsector.floorTex, 8) == 0
    && backsector.lightLvl == frontsector.lightLvl && m_map.getSidedefs()[seg.linedef().frontSidedef].middleTex[0] == '-') {
        return;
    }

    // Anything is hollow/passed
    seg.drawHollow(state, m_target, pixels);
}

void BSPRenderer::renderSubsector(uint16_t subsectorID, void *pixels) {
    auto subsector = m_map.getSubsectors()[subsectorID];
    auto& segs = m_map.getSegs();
    for (size_t i = 0; i < static_cast<uint16_t>(subsector.segCount); i++) {
        auto seg = segs[subsector.firstSeg + i];        
        drawSeg(seg, pixels);
    }
}

Math::Vec2 BSPRenderer::worldToScreen2D(Math::Vec2 in) {
    return Math::Vec2(
        ((m_target.getWidth() - 1) / (m_maxX - m_minX)) * (in.x - m_minX),
        -((m_target.getHeight() - 1) / (m_maxY - m_minY)) * (in.y - m_maxY)
    );
}