#include "map.h"

WADMap::WADMap(WADFile &wad, WAD::FileLump mapLump) {
    auto base = wad.getLumpIndex(mapLump.name);
    auto thingsLump = wad.getLumpFromIndex(base + (size_t)WADFile::MapOffsets::THINGS);
    auto vertsLump = wad.getLumpFromIndex(base + (size_t)WADFile::MapOffsets::VERTEXES);
    auto linedefsLump = wad.getLumpFromIndex(base + (size_t)WADFile::MapOffsets::LINEDEFS);
    auto nodesLump = wad.getLumpFromIndex(base + (size_t)WADFile::MapOffsets::NODES);
    loadThings(wad, thingsLump);
    loadVertices(wad, vertsLump);
    loadLinedefs(wad, linedefsLump);
    loadBSPNodes(wad, nodesLump);
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

void WADMap::loadBSPNodes(WADFile &wad, WAD::FileLump lump) {
    auto& stream = wad.getStream();
    stream.seekg(lump.filepos, std::ios_base::beg);
    m_nodes.resize(lump.size / sizeof(WAD::NodeEntry));
    Utils::streamRead(m_nodes.data(), lump.size, stream);    
}
