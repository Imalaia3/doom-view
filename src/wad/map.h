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

    Thing& findThingByType(Thing::ThingType t);

private:
    void loadThings(WADFile& wad, WAD::FileLump lump);
    void loadVertices(WADFile& wad, WAD::FileLump lump);
    void loadLinedefs(WADFile& wad, WAD::FileLump lump);
    void loadBSPNodes(WADFile& wad, WAD::FileLump lump);

    std::vector<Thing> m_things;
    std::vector<WAD::LinedefEntry> m_linedefs;
    std::vector<WAD::NodeEntry> m_nodes;
    std::vector<Math::Vec2> m_verts;
};