#include "bsp.h"

BSPRenderer::BSPRenderer(WADMap& map, SDLWindow &renderTarget) : m_map(map), m_target(renderTarget) {    
    m_player = Thing(map.findThingByType(Thing::ThingType::PLAYERONE));
    m_nodes = m_map.getNodes();

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
    m_count = 0;
    void* drawPixels = m_target.renderBegin();

    auto& verts = m_map.getVertices();
    for (const auto& def : m_map.getLinedefs()) {
        //todo: implement this in LinedefEntry
        auto v1 = worldToScreen2D(Math::Vec2(verts[def.v1].x, verts[def.v1].y));
        auto v2 = worldToScreen2D(Math::Vec2(verts[def.v2].x, verts[def.v2].y));
        // printf("%u %u - %u %u\n", (uint32_t)v1.x, (uint32_t)v1.y, (uint32_t)v2.x, (uint32_t)v2.y);
        m_target.drawLine(v1.x, v1.y, v2.x, v2.y, 0xFF, 0xFF, 0xFF, drawPixels);
    }

    auto playerScreenPos = worldToScreen2D(m_player.position);
    m_target.drawRectFilled(playerScreenPos.x - 2.0f, playerScreenPos.y - 2.0f, 4, 4, 0x00, 0xFF, 0xFF, drawPixels);

    WAD::NodeEntry& rootNode = getBSPRoot();
    auto leftBbox = rootNode.getLeftBox();
    auto rightBbox = rootNode.getRightBox();
    {
        auto p1 = worldToScreen2D(Math::Vec2(leftBbox.x1, leftBbox.y1));
        auto p2 = worldToScreen2D(Math::Vec2(leftBbox.x2, leftBbox.y2));

        m_target.drawRectHollow(
            p1.x, p1.y,
            p2.x, p2.y,
            0xFF, 0x00, 0x00, drawPixels
        );
    }
    {
        auto p1 = worldToScreen2D(Math::Vec2(rightBbox.x1, rightBbox.y1));
        auto p2 = worldToScreen2D(Math::Vec2(rightBbox.x2, rightBbox.y2));

        m_target.drawRectHollow(
            p1.x, p1.y,
            p2.x, p2.y,
            0x00, 0xFF, 0x00, drawPixels
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
    traverseBSP(frontSide ? node.leftChild : node.rightChild, pixels);
    
    // Back Space
    // TODO: Frustum culling on back node due to the chance of it being directly behind the player frustum
    traverseBSP(!frontSide ? node.leftChild : node.rightChild, pixels);
}

void BSPRenderer::renderSubsector(uint16_t subsectorID, void *pixels) {
    auto subsector = m_map.getSubsectors()[subsectorID];
    auto& segs = m_map.getSegs();
    auto& verts = m_map.getVertices();
    for (size_t i = 0; i < static_cast<uint16_t>(subsector.segCount); i++) {
        WAD::SegEntry seg = segs[subsector.firstSeg + i];
        Math::Vec2 vb = worldToScreen2D(Math::Vec2(verts[seg.vbeg].x, verts[seg.vbeg].y));
        Math::Vec2 ve = worldToScreen2D(Math::Vec2(verts[seg.vend].x, verts[seg.vend].y));
        m_target.drawLine(vb.x, vb.y, ve.x, ve.y, (10+m_count) & 0xFF, 100+((m_count>>8) & 0xFF), (m_count>>16) & 0xFF, pixels);
    }
    m_count++;
    
}

Math::Vec2 BSPRenderer::worldToScreen2D(Math::Vec2 in)
{
    return Math::Vec2(
        ((m_target.getWidth() - 1) / (m_maxX - m_minX)) * (in.x - m_minX),
        -((m_target.getHeight() - 1) / (m_maxY - m_minY)) * (in.y - m_maxY)
    );
}