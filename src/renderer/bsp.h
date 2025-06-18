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
    // void renderSubsector();
    // void traverseBSP();
    
    Math::Vec2 worldToScreen2D(Math::Vec2 in);
    inline WAD::NodeEntry& getBSPRoot() {
        auto& n = m_map.getNodes();
        return n[n.size() - 1];
    }

    
    float m_minX, m_minY, m_maxX, m_maxY;
    WADMap& m_map;
    SDLWindow& m_target;
    Thing m_player;
};