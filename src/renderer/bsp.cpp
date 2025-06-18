#include "bsp.h"

BSPRenderer::BSPRenderer(WADMap& map, SDLWindow &renderTarget) : m_map(map), m_target(renderTarget) {    
    m_player = Thing(map.findThingByType(Thing::ThingType::PLAYERONE));

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
}

void BSPRenderer::drawFrame() {
    void* drawPixels = m_target.renderBegin();

    auto& verts = m_map.getVertices();
    for (const auto& def : m_map.getLinedefs()) {
        //todo: implement this in LinedefEntry
        auto v1 = worldToScreen2D(Math::Vec2(verts[def.v1].x, verts[def.v1].y));
        auto v2 = worldToScreen2D(Math::Vec2(verts[def.v2].x, verts[def.v2].y));
        // printf("%u %u - %u %u\n", (uint32_t)v1.x, (uint32_t)v1.y, (uint32_t)v2.x, (uint32_t)v2.y);
        m_target.drawLine(v1.x, v1.y, v2.x, v2.y, 0xFF, 0xFF, 0xFF, drawPixels);
    }

    auto projected = worldToScreen2D(m_player.position);
    m_target.drawRectFilled(projected.x - 2.0f, projected.y - 2.0f, 4, 4, 0x00, 0xFF, 0xFF, drawPixels);

    WAD::NodeEntry& rootNode = getBSPRoot();
    auto leftBbox = rootNode.getLeftBox();
    auto rightBbox = rootNode.getRightBox();

    //Back = Left
    {
        auto p1 = worldToScreen2D(Math::Vec2(leftBbox.x1, leftBbox.y1));
        auto p2 = worldToScreen2D(Math::Vec2(leftBbox.x2, leftBbox.y2));

        m_target.drawRectHollow(
            p1.x, p1.y,
            p2.x, p2.y,
            0xFF, 0x00, 0x00, drawPixels
        );
    }
    
    //Front = Right
    {
        auto p1 = worldToScreen2D(Math::Vec2(rightBbox.x1, rightBbox.y1));
        auto p2 = worldToScreen2D(Math::Vec2(rightBbox.x2, rightBbox.y2));

        m_target.drawRectHollow(
            p1.x, p1.y,
            p2.x, p2.y,
            0x00, 0xFF, 0x00, drawPixels
        );
    }


    m_target.renderEnd();
    m_target.updateWindow();
}

Math::Vec2 BSPRenderer::worldToScreen2D(Math::Vec2 in) {
    return Math::Vec2(
        ((m_target.getWidth() - 1) / (m_maxX - m_minX)) * (in.x - m_minX),
        -((m_target.getHeight() - 1) / (m_maxY - m_minY)) * (in.y - m_maxY)
    );
}