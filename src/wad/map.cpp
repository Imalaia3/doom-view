#include "map.h"

WADMap::WADMap(WADFile &wad, WAD::FileLump mapLump) {
    auto base = wad.getLumpIndex(mapLump.name);
    auto thingsLump = wad.getLumpFromIndex(base + (size_t)WADFile::MapOffsets::THINGS);
    auto vertsLump = wad.getLumpFromIndex(base + (size_t)WADFile::MapOffsets::VERTEXES);
    auto linedefsLump = wad.getLumpFromIndex(base + (size_t)WADFile::MapOffsets::LINEDEFS);
    auto sidedefsLump = wad.getLumpFromIndex(base + (size_t)WADFile::MapOffsets::SIDEDEFS);
    auto nodesLump = wad.getLumpFromIndex(base + (size_t)WADFile::MapOffsets::NODES);
    auto ssectorsLump = wad.getLumpFromIndex(base + (size_t)WADFile::MapOffsets::SSECTORS);
    auto sectorsLump = wad.getLumpFromIndex(base + (size_t)WADFile::MapOffsets::SECTORS);
    auto segsLump = wad.getLumpFromIndex(base + (size_t)WADFile::MapOffsets::SEGS);
    loadThings(wad, thingsLump);
    loadVertices(wad, vertsLump);
    loadLinedefs(wad, linedefsLump);
    loadSidedefs(wad, sidedefsLump);
    loadBSPNodes(wad, nodesLump);
    loadSubsectors(wad, ssectorsLump);
    loadSectors(wad, sectorsLump);
    loadSegs(wad, segsLump);
}

Thing& WADMap::findThingByType(Thing::ThingType t) {
    auto iterator = std::find_if(m_things.begin(), m_things.end(), [&t](const Thing& thing) {
        return thing.getType() == (uint16_t)t;
    });
    if (iterator == m_things.end())
        throw std::runtime_error("Thing does not exist");
    
    return *iterator;
}

void WADMap::loadThings(WADFile &wad, WAD::FileLump lump) {
    auto& stream = wad.getStream();
    stream.seekg(lump.filepos, std::ios_base::beg);
    for (size_t i = 0; i < lump.size / sizeof(WAD::ThingEntry); i+=1) {
        WAD::ThingEntry entry{};
        Utils::streamRead(&entry, sizeof(WAD::ThingEntry), stream);
        m_things.push_back(Thing(entry));
    }
}

void WADMap::loadVertices(WADFile &wad, WAD::FileLump lump) {
    auto& stream = wad.getStream();
    stream.seekg(lump.filepos, std::ios_base::beg);
    for (size_t i = 0; i < lump.size / 4; i+=1) {
        int16_t x, y;
        Utils::streamRead(&x, 2, stream);
        Utils::streamRead(&y, 2, stream);
        m_verts.push_back(Math::Vec2(x, y));
    }
}

void WADMap::loadLinedefs(WADFile &wad, WAD::FileLump lump) {
    auto& stream = wad.getStream();
    stream.seekg(lump.filepos, std::ios_base::beg);
    m_linedefs.resize(lump.size / sizeof(WAD::LinedefEntry));
    Utils::streamRead(m_linedefs.data(), lump.size, stream);
}

void WADMap::loadSidedefs(WADFile &wad, WAD::FileLump lump) {
    auto& stream = wad.getStream();
    stream.seekg(lump.filepos, std::ios_base::beg);
    m_sidedefs.resize(lump.size / sizeof(WAD::SidedefEntry));
    Utils::streamRead(m_sidedefs.data(), lump.size, stream);
}

void WADMap::loadBSPNodes(WADFile &wad, WAD::FileLump lump) {
    auto& stream = wad.getStream();
    stream.seekg(lump.filepos, std::ios_base::beg);
    m_nodes.resize(lump.size / sizeof(WAD::NodeEntry));
    Utils::streamRead(m_nodes.data(), lump.size, stream);    
}

void WADMap::loadSubsectors(WADFile &wad, WAD::FileLump lump) {
    auto& stream = wad.getStream();
    stream.seekg(lump.filepos, std::ios_base::beg);
    m_subsectors.resize(lump.size / sizeof(WAD::SubsectorEntry));
    Utils::streamRead(m_subsectors.data(), lump.size, stream);
}

void WADMap::loadSectors(WADFile &wad, WAD::FileLump lump) {
    auto& stream = wad.getStream();
    stream.seekg(lump.filepos, std::ios_base::beg);
    m_sectors.resize(lump.size / sizeof(WAD::SectorEntry));
    Utils::streamRead(m_sectors.data(), lump.size, stream);
}

void WADMap::loadSegs(WADFile &wad, WAD::FileLump lump) {
    assert(m_sectors.size() > 0);
    assert(m_linedefs.size() > 0);
    assert(m_sidedefs.size() > 0);
    assert(m_verts.size() > 0);

    auto& stream = wad.getStream();
    stream.seekg(lump.filepos, std::ios_base::beg);
    for (size_t i = 0; i < lump.size / sizeof(WAD::SegEntry); i+=1) {
        WAD::SegEntry entry{};
        Utils::streamRead(&entry, sizeof(WAD::SegEntry), stream);
        m_segs.push_back(Seg(entry, m_verts, m_sectors, m_linedefs, m_sidedefs));
    }
}