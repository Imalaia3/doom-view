#include "wad.h"
#include <cerrno>
#include <cstring>
#include "../utils.h"

WADFile::WADFile(const char *filename) {
    m_stream.open(filename, std::ios::binary);
    if(!m_stream.is_open()) {
        throw std::runtime_error("Failed to open WAD file");
    }
    char identifier[4];
    Utils::streamRead(identifier, 4, m_stream);

    if (std::memcmp(identifier, "IWAD", 4)) {
        throw std::runtime_error("File is missing the IWAD identifier");
    }
    Utils::streamRead(&m_lumpCount, sizeof(uint32_t), m_stream);
    Utils::streamRead(&m_infoTableOffset, sizeof(uint32_t), m_stream);

    // Read directory
    m_lumps.resize(m_lumpCount);
    m_stream.seekg(m_infoTableOffset, std::ios_base::beg);
    for (size_t i = 0; i < m_lumpCount; i++) {
        WAD::FileLump lump{};
        Utils::streamRead(&lump, sizeof(WAD::FileLump), m_stream);
        m_lumps.emplace_back(lump);
    }    
}

std::vector<uint8_t> WADFile::getColorPalette(uint8_t paletteID) {
    auto playpalLump = getLumpByName("PLAYPAL");
    constexpr uint32_t paletteSize = 16*16*3; // 16x16 24bpp
    std::vector<uint8_t> colorValues(paletteSize);
    m_stream.seekg(playpalLump.filepos + paletteSize*paletteID, std::ios_base::beg);
    Utils::streamRead(colorValues.data(), paletteSize, m_stream);
    return colorValues;
}

WAD::FileLump WADFile::getLumpByName(std::string name) const {
    auto iterator = std::find_if(m_lumps.begin(), m_lumps.end(), [&name](const WAD::FileLump& lump) {
        return std::strncmp(lump.name, name.c_str(), 8) == 0;
    });
    if (iterator == m_lumps.end())
        throw std::runtime_error("Lump does not exist");
    
    return *iterator;
}
