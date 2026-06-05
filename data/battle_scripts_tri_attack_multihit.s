#include "constants/battle.h"
	.include "asm/macros/battle_script.inc"

	.section script_data, "aw", %progbits

BattleScript_EffectMultistrikeTriAttack::
	clearmoveresultflags MOVE_RESULT_MISSED | MOVE_RESULT_AVOIDED_ATTACK
	attackcanceler
	setmultistriketriattacktype
	callnative BS_TriAttackStrikeCancelerPrologue
	callnative BS_TriAttackStrikeCanceler
	goto BattleScript_HitFromAccCheck
