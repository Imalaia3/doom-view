#pragma once
#include <fstream>
#include <string>
#include <exception>
#include <stdexcept>
#include <stdint.h>
#include <vector>

#include "structures.h"


class WADFile {
public:
    WADFile(const char* filename);
    ~WADFile() {
        m_stream.close();
    }

    uint32_t getLumpCount() const { return m_lumpCount; }
    bool isMarker(const FileLump& lump) const { return lump.size == 0; }
    std::vector<FileLump>& getLumps() { return m_lumps; }

private:
    std::ifstream m_stream;
    std::vector<FileLump> m_lumps;
    uint32_t m_lumpCount;
    uint32_t m_infoTableOffset;

    template<typename T>
    inline void streamRead(T* buffer, size_t bufferSize, std::ifstream& stream) {
        stream.read(reinterpret_cast<char*>(buffer), bufferSize);
    }
};