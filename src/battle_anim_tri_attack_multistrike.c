#include "global.h"
#include "battle_anim.h"

void AnimTask_TriAttack(u8 taskId)
{
    switch (gMultiHitCounter)
    {
    case 3:
        gBattleAnimArgs[0] = TYPE_FIRE;
        break;
    case 2:
        gBattleAnimArgs[0] = TYPE_ICE;
        break;
    case 1:
        gBattleAnimArgs[0] = TYPE_ELECTRIC;
        break;
    default:
        gBattleAnimArgs[0] = TYPE_NORMAL;
        break;
    }
    DestroyAnimVisualTask(taskId);
}
