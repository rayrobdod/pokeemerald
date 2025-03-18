#ifndef GUARD_BATTLE_TRANSITION_H
#define GUARD_BATTLE_TRANSITION_H

#include "constants/battle_transition.h"

void BattleTransition_StartOnField(u8 transitionId);
void BattleTransition_Start(u8 transitionId);
bool8 IsBattleTransitionDone(void);
bool8 FldEff_PokeballTrail(void);
void Task_BattleTransition_Intro(u8 taskId);
void GetBg0TilesDst(u16 **tilemap, u16 **tileset);

extern const struct SpritePalette gSpritePalette_Pokeball;

enum {
    MUGSHOT_COLOR_NONE,
    MUGSHOT_COLOR_PURPLE,
    MUGSHOT_COLOR_GREEN,
    MUGSHOT_COLOR_PINK,
    MUGSHOT_COLOR_BLUE,
    MUGSHOT_COLOR_YELLOW,
    MUGSHOT_COLOR_COUNT
};

// IDs for GetSpecialBattleTransition
enum {
    B_TRANSITION_GROUP_B_TOWER,
    B_TRANSITION_GROUP_B_DOME = 3,
    B_TRANSITION_GROUP_B_PALACE,
    B_TRANSITION_GROUP_B_ARENA,
    B_TRANSITION_GROUP_B_FACTORY,
    B_TRANSITION_GROUP_B_PIKE,
    B_TRANSITION_GROUP_B_PYRAMID = 10,
    B_TRANSITION_GROUP_TRAINER_HILL,
    B_TRANSITION_GROUP_SECRET_BASE,
    B_TRANSITION_GROUP_E_READER,
};

#endif // GUARD_BATTLE_TRANSITION_H
