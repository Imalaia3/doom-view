#include "overlap.h"

void OverlapManager::addWall(Interval newInterval, std::function<void(int32_t, int32_t)> callback) {
    auto interval = m_intervals.begin();
    while (interval != m_end and interval->end < newInterval.start) { interval++; }

    //FIXME: Can this be inside the while loop to skip 2 ifs?
    if (interval != m_end  && newInterval.start >= interval->start && newInterval.end <= interval->end) {
        return;
    }
    
    int32_t start = newInterval.start;
    while (start < newInterval.end && interval != m_end) {
        if (start >= interval->start) {
            start = std::min(newInterval.end, interval->end);
        } else {
            int32_t newStart = std::min(newInterval.end, interval->start);
            //FIXME: Optimize min to [(b < a ? b : a) = b] interval->start < newInterval.end below?;
            if (newStart == interval->start) {
                callback(start, newStart - 1);
                interval->start = start;
            } else if (interval > m_intervals.begin()) {
                callback(start + 1, newStart);
                (interval - 1)->end = newInterval.end;
            }
            start = newStart;
        }
        interval++;
    }
    if (start < newInterval.end) {
        if (interval != m_intervals.begin() && (start == (m_end-1)->end)) {
            callback(start + 1, newInterval.end);
            (m_end-1)->end = newInterval.end;
        } else {
            callback(start, newInterval.end);
            push_back(Interval(start, newInterval.end));
        }
    }
}