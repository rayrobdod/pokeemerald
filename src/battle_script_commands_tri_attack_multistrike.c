#include "global.h"
#include "battle.h"

void BS_SetMultistrikeTriAttackType(void)
{
    NATIVE_ARGS();

    switch (gMultiHitCounter) {
    case 3:
        gBattleStruct->dynamicMoveType = TYPE_FIRE | F_DYNAMIC_TYPE_SET;
        break;
    case 2:
        gBattleStruct->dynamicMoveType = TYPE_ICE | F_DYNAMIC_TYPE_SET;
        break;
    case 1:
        gBattleStruct->dynamicMoveType = TYPE_ELECTRIC | F_DYNAMIC_TYPE_SET;
        break;
    default:
        gBattleStruct->dynamicMoveType = TYPE_NORMAL | F_DYNAMIC_TYPE_SET;
    }

    gBattlescriptCurrInstr = cmd->nextInstr;
}
