#include "thing.h"

bool Thing::inDifficulty(uint8_t level) const {
    if (level > 5)                                      return false;
    if ((level == 1 || level == 2) && (m_flags & 0x0001)) return true;
    if ((level == 3) && (m_flags & 0x0002))               return true;
    if ((level == 4 || level == 5) && (m_flags & 0x0004)) return true;
    return false;
}