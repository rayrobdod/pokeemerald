#ifndef GUARD_MUGSHOTS_H
#define GUARD_MUGSHOTS_H

#include "constants/mugshots.h"

struct Mugshot {
    const u32 *tiles;
    const u16 *palette;
};

extern const struct Mugshot gMugshots[];

#endif
