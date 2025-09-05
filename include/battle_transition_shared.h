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

struct TransitionData
{
    vu8 VBlank_DMA;
    u16 WININ;
    u16 WINOUT;
    u16 WIN0H;
    u16 WIN0V;
    u16 unused1;
    u16 unused2;
    u16 BLDCNT;
    u16 BLDALPHA;
    u16 BLDY;
    s16 cameraX;
    s16 cameraY;
    s16 BG0HOFS_Lower;
    s16 BG0HOFS_Upper;
    s16 BG0VOFS; // used but not set
    s16 unused3;
    s16 counter;
    s16 unused4;
    s16 data[11];
};

extern struct TransitionData *sTransitionData;

void InitTransitionData(void);
void VBlankCB_BattleTransition(void);
void GetBg0TilemapDst(u16 **);
void FadeScreenBlack(void);
void SetSinWave(s16 *, s16, s16, s16, s16, s16);
void SetCircularMask(u16 *, s16, s16, s16);
bool8 UpdateBlackWipe(s16 *, bool8, bool8);

#endif // GUARD_BATTLE_TRANSITION_H
