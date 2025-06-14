#pragma once
#include <fstream>

namespace Utils {
    template<typename T>
    inline void streamRead(T* buffer, size_t bufferSize, std::ifstream& stream) {
        stream.read(reinterpret_cast<char*>(buffer), bufferSize);
    }
}