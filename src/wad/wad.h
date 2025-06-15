#pragma once
#include <fstream>
#include <string>
#include <exception>
#include <stdexcept>
#include <stdint.h>
#include <vector>
#include <algorithm>
#include <cstring>
#include <assert.h>

#include "structures.h"


class WADFile {
public:
    WADFile(const char* filename);
    ~WADFile() {
        m_stream.close();
    }

    std::ifstream& getStream() { return m_stream; }
    uint32_t getLumpCount() const { return m_lumpCount; }
    bool isMarker(const WAD::FileLump& lump) const { return lump.size == 0; }
    std::vector<WAD::FileLump>& getLumps() { return m_lumps; }
    std::vector<uint8_t> getColorPalette(uint8_t paletteID);
    
    WAD::FileLump getLumpByName(std::string name) const;
    size_t getLumpIndex(std::string name) const;
    
    WAD::FileLump getLumpFromIndex(size_t index) {
        assert(index < m_lumpCount);
        return m_lumps[index];
    }

    // Offset from Map Marker
    enum class MapOffsets {
        THINGS = 1,
        LINEDEFS,
        SIDEDEFS,
        VERTEXES,
        SEGS,
        SSECTORS,
        NODES,
        SECTORS,
        REJECT,
        BLOCKMAP,
        BEHAVIOR
    };
    
private:
    std::ifstream m_stream;
    std::vector<WAD::FileLump> m_lumps;
    uint32_t m_lumpCount;
    uint32_t m_infoTableOffset;
};