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

void BS_TryBlockMultistrikeTriAttack(void)
{
    const u8 *nextInstr = gBattlescriptCurrInstr + 5;

    struct BattleContext ctx = {0};
    ctx.battlerAtk = gBattlerAttacker;
    ctx.battlerDef = gBattlerTarget;
    ctx.move = ctx.chosenMove = gCurrentMove;
    ctx.moveType = gBattleStruct->dynamicMoveType;
    ctx.abilityAtk = GetBattlerAbility(ctx.battlerAtk);
    ctx.abilityDef = GetBattlerAbility(ctx.battlerDef);
    ctx.holdEffectAtk = GetBattlerHoldEffect(ctx.battlerAtk);
    ctx.holdEffectDef = GetBattlerHoldEffect(ctx.battlerDef);
    ctx.runScript = TRUE;
    s32 movePriority = GetChosenMovePriority(ctx.battlerAtk, ctx.abilityAtk);

    gBattlescriptCurrInstr = BattleScript_MoveEnd;
    // the Instr to push onto the call stack must be set before
    // CanMoveBeBlockedByTarget calls the ability script
    if (! CanMoveBeBlockedByTarget(&ctx, movePriority))
    {
        gBattlescriptCurrInstr = nextInstr;
    }
}
