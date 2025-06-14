#include "wad.h"
#include <cerrno>
#include <cstring>

WADFile::WADFile(const char *filename) {
    m_stream.open(filename, std::ios::binary);
    if(!m_stream.is_open()) {
        throw std::runtime_error("Failed to open WAD file");
    }
    char identifier[4];
    streamRead(identifier, 4, m_stream);

    if (std::memcmp(identifier, "IWAD", 4)) {
        throw std::runtime_error("File is missing the IWAD identifier");
    }
    streamRead(&m_lumpCount, sizeof(uint32_t), m_stream);
    streamRead(&m_infoTableOffset, sizeof(uint32_t), m_stream);

    // Read directory
    m_lumps.resize(m_lumpCount);
    m_stream.seekg(m_infoTableOffset, std::ios_base::beg);
    for (size_t i = 0; i < m_lumpCount; i++) {
        FileLump lump{};
        streamRead(&lump, sizeof(FileLump), m_stream);
        m_lumps.emplace_back(lump);
    }    
}