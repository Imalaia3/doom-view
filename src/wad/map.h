#pragma once
#include "wad.h"
#include "../utils.h"
#include "elements/thing.h"
#include "elements/seg.h"


class WADMap {
public:
    WADMap(WADFile& wad, WAD::FileLump mapLump);

    inline std::vector<Thing>& getThings() { return m_things; }
    inline std::vector<Math::Vec2>& getVertices() { return m_verts; }
    inline std::vector<WAD::LinedefEntry>& getLinedefs() { return m_linedefs; }
    inline std::vector<WAD::SidedefEntry>& getSidedefs() { return m_sidedefs; }
    inline std::vector<WAD::NodeEntry>& getNodes() { return m_nodes; }
    inline std::vector<Seg>& getSegs() { return m_segs; }
    inline std::vector<WAD::SubsectorEntry>& getSubsectors() { return m_subsectors; }
    inline std::vector<WAD::SectorEntry>& getSectors() { return m_sectors; }

    Thing& findThingByType(Thing::ThingType t);

private:
    void loadThings(WADFile& wad, WAD::FileLump lump);
    void loadVertices(WADFile& wad, WAD::FileLump lump);
    void loadLinedefs(WADFile& wad, WAD::FileLump lump);
    void loadSidedefs(WADFile& wad, WAD::FileLump lump);
    void loadBSPNodes(WADFile& wad, WAD::FileLump lump);
    void loadSubsectors(WADFile& wad, WAD::FileLump lump);
    void loadSectors(WADFile& wad, WAD::FileLump lump);
    void loadSegs(WADFile& wad, WAD::FileLump lump);

    std::vector<Thing> m_things;
    std::vector<WAD::LinedefEntry> m_linedefs;
    std::vector<WAD::SidedefEntry> m_sidedefs;
    std::vector<WAD::NodeEntry> m_nodes;
    std::vector<Math::Vec2> m_verts;
    std::vector<WAD::SubsectorEntry> m_subsectors;
    std::vector<WAD::SectorEntry> m_sectors;
    std::vector<Seg> m_segs;
};