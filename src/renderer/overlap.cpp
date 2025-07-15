#include "overlap.h"

// FIXME FIXME FIXME: Old algorithm might've been correct (but less memory efficient)
void OverlapManager::addWall(Interval newInterval, std::function<void(int32_t, int32_t)> callback) {
    Interval* tempstart = m_temporaryBuffer;
    Interval* start = m_mainBuffer;
    
    while (start->end < newInterval.start - 1) {
        *(tempstart++) = *(start++);
    }

    if(newInterval.start < start->start) {
        if (newInterval.end < start->start - 1) {
            callback(newInterval.start, newInterval.end);
            *(tempstart++) = Interval(newInterval.start, newInterval.end);
            size_t count = m_mainEnd - start;
            std::memmove(tempstart, start, count * sizeof(Interval));
            tempstart += count;
            swap(tempstart);
            return;
        }

        callback(newInterval.start, start->start - 1);
        start->start = newInterval.start;
    }

    if (newInterval.end <= start->end) { return; }

    Interval* next = start;
    bool broken = false;
    while (newInterval.end >= (next+1)->start - 1) {
        callback(next->end + 1, (next + 1)->start - 1);
        next++;
        if (newInterval.end <= next->end) {
            start->end = next->end;
            broken = true;
            break;
        }
    }

    if(!broken) {
        callback(next->end + 1, newInterval.end);
        start->end = newInterval.end;
    }

    if (next == start) { return; }

    *(tempstart++) = *start;
    next++;
    size_t count = m_mainEnd - next;
    std::memmove(tempstart, next, count * sizeof(Interval));
    tempstart += count + 1;
    swap(tempstart);
}