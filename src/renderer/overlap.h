#pragma once
#include <stdio.h>
#include <array>
#include <functional>
#include <cassert>

/* Resolves wall overlaps and prevents out of order drawing by maintaining a list of already drawn walls (Interval) */
class OverlapManager {
public:
    static constexpr size_t BUFFER_MAX = 1024;
    struct Interval {
        int32_t start, end;

        Interval() : start(0), end(0) {}
        Interval(int32_t _start, int32_t _end) : start(_start), end(_end) {}
    };

    OverlapManager() : m_size(0), m_end(m_intervals.begin()) {}
    // Calculate overlaps for wall newInterval (inclusive) and call callback(start, end) for each new fragment
    void addWall(Interval newInterval, std::function<void(int32_t, int32_t)> callback);

    inline void print() {
        for (size_t i = 0; i < m_size; i++) { printf("[%i, %i], ", m_intervals[i].start, m_intervals[i].end); }
        printf("\n");
    }
    
private:
    inline void push_back(Interval interval) {
        assert(m_size < BUFFER_MAX); // account for +1 that will be added later so no <=
        *(m_end++) = interval;
        m_size++;
    }

    size_t m_size = 0; // debugging
    std::array<Interval, BUFFER_MAX> m_intervals;
    std::array<Interval, BUFFER_MAX>::iterator m_end;
};