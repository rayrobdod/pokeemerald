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

static bool32 IsCancellerPerStrike(enum CancelerState state)
{
    return
        state >= CANCELER_SET_TARGETS &&
        state != CANCELER_PPDEDUCTION &&
        state != CANCELER_MULTIHIT_MOVES &&
        state != CANCELER_ACCURACY_CHECK &&
        TRUE;
}

void BS_TriAttackMoveCanceler(void)
{
    if (gBattleStruct->battlerState[gBattlerAttacker].usedEjectItem)
    {
        gBattleStruct->battlerState[gBattlerAttacker].usedEjectItem = FALSE;
        gCurrentActionFuncId = B_ACTION_TRY_FINISH;
        return;
    }

    if (gBattleOutcome != 0)
    {
        gCurrentActionFuncId = B_ACTION_FINISHED;
        return;
    }


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

    while (gBattleStruct->eventState.atkCanceler < CANCELER_END && result == CANCELER_RESULT_SUCCESS)
    {
        if (IsCancellerPerStrike(gBattleStruct->eventState.atkCanceler))
        {
            gBattleStruct->eventState.atkCanceler++;
            continue;
        }

        result = sMoveSuccessOrderCancelers[gBattleStruct->eventState.atkCanceler](&cv);

        if (result != CANCELER_RESULT_RUN_SCRIPT)
            gBattleStruct->eventState.atkCanceler++;
    }

    if (CANCELER_RESULT_FAILURE == result)
    {
        gBattleStruct->unableToUseMove = TRUE;
        gBattleStruct->pledgeState = PLEDGE_COMBO_NONE;
    }
    else if (CANCELER_RESULT_SUCCESS == result)
    {
        if (gBattleStruct->moveResultFlags[gBattlerTarget] & MOVE_RESULT_AVOIDED_ATTACK)
            gBattlescriptCurrInstr = BattleScript_MoveEnd;
        else
            gBattlescriptCurrInstr += 5;
    }
}

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

    while (gBattleStruct->eventState.atkStrikeCanceler < CANCELER_END && result == CANCELER_RESULT_SUCCESS)
    {
        if (! IsCancellerPerStrike(gBattleStruct->eventState.atkStrikeCanceler))
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
