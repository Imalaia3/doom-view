#pragma once
#include <stdio.h>
#include <array>
#include <functional>
#include <cassert>
#include <cstring>

/* Resolves wall overlaps and prevents out of order drawing by maintaining a list of already drawn walls (Interval) */
class OverlapManager {
public:
    static constexpr size_t BUFFER_MAX = 32;
    struct Interval {
        int32_t start, end;

        Interval() : start(0), end(0) {}
        Interval(int32_t _start, int32_t _end) : start(_start), end(_end) {}
    };

    OverlapManager() { clear(); }
    // Calculate overlaps for wall newInterval (inclusive) and call callback(start, end) for each new fragment
    void addWall(Interval newInterval, std::function<void(int32_t, int32_t)> callback);

    inline void print() {
        auto* ptr = m_mainBuffer;
        while (ptr != m_mainEnd){
            printf("[%i, %i], ", ptr->start, ptr->end);
            ptr++;
        }
        printf("\n");
    }

    void clear() {
        m_mainBuffer[0].start = -0x7fffffff;
        m_mainBuffer[0].end   = -1;
        m_mainBuffer[1].start = 1200;
        m_mainBuffer[1].end   = 0x7fffffff;
        m_mainEnd = m_mainBuffer+2;
    }
    
private:
    Interval m_mainBuffer_storage[BUFFER_MAX];
    Interval m_temporaryBuffer_storage[BUFFER_MAX];

    Interval* m_mainBuffer = m_mainBuffer_storage;
    Interval* m_temporaryBuffer = m_temporaryBuffer_storage;
    Interval* m_mainEnd;

    void swap(Interval* newend) {
        std::swap(m_mainBuffer, m_temporaryBuffer);
        m_mainEnd = newend;
    }
};