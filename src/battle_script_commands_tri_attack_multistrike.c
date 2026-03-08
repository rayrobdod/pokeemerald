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
        gBattleStruct->battlerState[gBattlerAttacker].targetsDone[i] = FALSE;

    gBattlescriptCurrInstr += 5;
}

extern enum CancelerResult (*const sMoveSuccessOrderCancelers[])(struct BattleContext *ctx);

void BS_TriAttackStrikeCanceler(void)
{
    enum CancelerResult result = CANCELER_RESULT_SUCCESS;

    struct BattleContext ctx = {0};
    ctx.battlerAtk = gBattlerAttacker;
    ctx.battlerDef = gBattlerTarget;
    ctx.move = ctx.chosenMove = gCurrentMove;
    ctx.moveType = gBattleStruct->dynamicMoveType;
    ctx.abilityAtk = GetBattlerAbility(ctx.battlerAtk);
    ctx.abilityDef = GetBattlerAbility(ctx.battlerDef);
    ctx.holdEffectAtk = GetBattlerHoldEffect(ctx.battlerAtk);
    ctx.holdEffectDef = GetBattlerHoldEffect(ctx.battlerDef);

    while (gBattleStruct->eventState.atkStrikeCanceler < CANCELER_MULTIHIT_MOVES && result == CANCELER_RESULT_SUCCESS)
    {
        if (CANCELER_PPDEDUCTION == gBattleStruct->eventState.atkStrikeCanceler)
        {
            gBattleStruct->eventState.atkStrikeCanceler++;
            continue;
        }

        result = sMoveSuccessOrderCancelers[gBattleStruct->eventState.atkStrikeCanceler](&ctx);
        if (result != CANCELER_RESULT_PAUSE)
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
