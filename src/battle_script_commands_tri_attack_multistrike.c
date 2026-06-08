#include "global.h"
#include "battle.h"
#include "battle_scripts.h"
#include "battle_util.h"

// SetTypeBeforeUsingMove runs before the move,
// so cannot be used to change type between strikes
void BS_SetMultistrikeTriAttackType(void)
{
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

    gBattlescriptCurrInstr += 5;
}

void BS_TriAttackStrikeCancelerPrologue(void)
{
    gBattleStruct->eventState.atkStrikeCanceler = CANCELER_SET_TARGETS;
    gBattleStruct->eventState.atkCancelerBattler = 0;
    for (unsigned i = 0; i < MAX_BATTLERS_COUNT; i++)
    {
        gBattleStruct->battlerState[gBattlerAttacker].targetsDone[i] = FALSE;
        gBattleStruct->moveResultFlags[i] = 0;
    }

    gBattlescriptCurrInstr += 5;
}

extern enum CancelerResult (*const sMoveSuccessOrderCancelers[])(struct BattleCalcValues *cv);

void BS_TriAttackStrikeCanceler(void)
{
    enum CancelerResult result = CANCELER_RESULT_SUCCESS;

    struct BattleCalcValues cv = {0};
    cv.battlerAtk = gBattlerAttacker;
    cv.battlerDef = gBattlerTarget;
    cv.move = gCurrentMove;
    cv.moveEffect = GetMoveEffect(cv.move);
    for (enum BattlerId battler = 0; battler < gBattlersCount; battler++)
    {
        cv.abilities[battler] = GetBattlerAbility(battler);
        cv.holdEffects[battler] = GetBattlerHoldEffect(battler);
    }

    while (gBattleStruct->eventState.atkStrikeCanceler < CANCELER_MULTIHIT_MOVES && result == CANCELER_RESULT_SUCCESS)
    {
        if (CANCELER_PPDEDUCTION == gBattleStruct->eventState.atkStrikeCanceler)
        {
            gBattleStruct->eventState.atkStrikeCanceler++;
            continue;
        }

        result = sMoveSuccessOrderCancelers[gBattleStruct->eventState.atkStrikeCanceler](&cv);
        if (result != CANCELER_RESULT_RUN_SCRIPT)
            gBattleStruct->eventState.atkStrikeCanceler++;
    }

    if (CANCELER_RESULT_SUCCESS == result)
    {
        if (gBattleStruct->moveResultFlags[gBattlerTarget] & MOVE_RESULT_AVOIDED_ATTACK)
            gBattlescriptCurrInstr = BattleScript_MoveEnd;
        else
            gBattlescriptCurrInstr += 5;
    }
}
