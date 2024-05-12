#ifndef GUARD_BATTLE_TRANSITION_SHARED_H
#define GUARD_BATTLE_TRANSITION_SHARED_H

#include "task.h"

// Used by each transition task to determine which of its functions to call
#define tState          data[0]

#define SET_TILE(ptr, posY, posX, tile) \
{                                       \
    u32 index = (posY) * 32 + posX;     \
    ptr[index] = tile | (0xF0 << 8);    \
}

typedef bool8 (*TransitionStateFunc)(struct Task *task);
typedef bool8 (*TransitionSpriteCallback)(struct Sprite *sprite);

void InitTransitionData(void);
void FadeScreenBlack(void);


#endif // GUARD_BATTLE_TRANSITION_H
