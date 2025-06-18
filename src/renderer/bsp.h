#pragma once
#include "../wad/wad.h"
#include "../wad/map.h"
#include "../wad/elements/thing.h"
#include "../display/window.h"

class BSPRenderer {
public:
    BSPRenderer(WADMap& wad, SDLWindow& renderTarget);
    void drawFrame();
    Thing& getPlayer() { return m_player; };
private:
    // Return the front bounding box. Either 0 for right or 1 for left (See WAD::NodeEntry for bounding boxes)
    bool calculateSide(WAD::NodeEntry& node);    

    void traverseBSP(int16_t bspNodeID, void* pixels);
    //Draw 2D representation of the subsector
    void renderSubsector(uint16_t subsectorID, void* pixels);

    Math::Vec2 worldToScreen2D(Math::Vec2 in);
    inline WAD::NodeEntry& getBSPRoot() {return m_nodes[m_nodes.size() - 1]; }

    
    float m_minX, m_minY, m_maxX, m_maxY;
    std::vector<WAD::NodeEntry> m_nodes;
    WADMap& m_map;
    SDLWindow& m_target;
    Thing m_player;
    // Only for testing
    uint32_t m_count = 0;
};