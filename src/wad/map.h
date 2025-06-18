#pragma once
#include "wad.h"
#include "../utils.h"
#include "elements/thing.h"


class WADMap {
public:
    WADMap(WADFile& wad, WAD::FileLump mapLump);

    std::vector<Thing>& getThings() { return m_things; }
    std::vector<Math::Vec2>& getVertices() { return m_verts; }
    std::vector<WAD::LinedefEntry>& getLinedefs() { return m_linedefs; }
    std::vector<WAD::NodeEntry>& getNodes() { return m_nodes; }
    std::vector<WAD::SegEntry>& getSegs() { return m_segs; }
    std::vector<WAD::SubsectorEntry>& getSubsectors() { return m_subsectors; }

    Thing& findThingByType(Thing::ThingType t);

private:
    void loadThings(WADFile& wad, WAD::FileLump lump);
    void loadVertices(WADFile& wad, WAD::FileLump lump);
    void loadLinedefs(WADFile& wad, WAD::FileLump lump);
    void loadBSPNodes(WADFile& wad, WAD::FileLump lump);
    void loadSubsectors(WADFile& wad, WAD::FileLump lump);
    void loadSegs(WADFile& wad, WAD::FileLump lump);

    std::vector<Thing> m_things;
    std::vector<WAD::LinedefEntry> m_linedefs;
    std::vector<WAD::NodeEntry> m_nodes;
    std::vector<Math::Vec2> m_verts;
    std::vector<WAD::SubsectorEntry> m_subsectors;
    std::vector<WAD::SegEntry> m_segs;
};