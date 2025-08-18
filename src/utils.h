#pragma once
#include <fstream>

namespace Utils {
    template<typename T>
    inline void streamRead(T* buffer, size_t bufferSize, std::ifstream& stream) {
        stream.read(reinterpret_cast<char*>(buffer), bufferSize);
    }
    inline uint32_t ftou32_safe(float f) {
        return f <= 0.0f ? 0 : static_cast<uint32_t>(f);
    }
}