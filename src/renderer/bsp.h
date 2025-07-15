#pragma once
#include "../wad/wad.h"
#include "../wad/map.h"
#include "../wad/elements/thing.h"
#include "../display/window.h"
#include "../math/angles.h"
#include "overlap.h"

class BSPRenderer {
public:
    BSPRenderer(WADMap& wad, SDLWindow& renderTarget);
    void drawFrame();
    Thing& getPlayer() { return m_player; };
    float getFOV() const { return m_FOV; }

    // TODO: Migrate player values to struct/class
    const float PLAYER_VIEWHEIGHT = 41; // Player body height is 56 but eyes are at 41. https://doomwiki.org/wiki/Player
private:
    // Return the front bounding box. Either 0 for right or 1 for left (See WAD::NodeEntry for bounding boxes). TODO: Inline?
    bool calculateSide(WAD::NodeEntry& node);
    inline float toAngle(Math::Vec2 point) {
        return std::atan2(point.y - m_player.position.y , point.x - m_player.position.x);
    }

    // Calculates xtoviewangle table
    void calculateAngleTable();
    // generates a scale based off of the screen x, the seg angle and the distance from the viewer
    float scaleFromGlobalAngle(float x, float rw_normalangle, float rw_distance);
    bool insideFrustum(Math::BoundingBox bbox);
    void traverseBSP(int16_t bspNodeID, void* pixels);
    inline int32_t viewAngleToX(float angle);
    // Classify if seg is withing the player's FOV and is not a back face.
    bool isSegVisible(Seg& seg);
    // FIXME: This can be optimized by incorporating it into isSegVisible()
    //        because we know what type of angle it would be (a1/a2)
    // Clip angle to -FOV/2 or +FOV/2, assumes angle is adjusted by -m_player.getAngleRadians()
    inline float clipAngle(float angle) {
        if (angle < -m_clipangle) return -m_clipangle;
        if (angle > m_clipangle)  return m_clipangle;
        return angle;
    };
    void drawSeg(Seg seg, void* pixels);
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
    float m_FOV;
    float m_clipangle;
    OverlapManager m_overlaps;
    std::vector<float> xtoviewangle;
};