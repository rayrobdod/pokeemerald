#include "constants/battle.h"
	.include "asm/macros/battle_script.inc"

	.section script_data, "aw", %progbits

BattleScript_EffectMultistrikeTriAttack::
	clearmoveresultflags MOVE_RESULT_MISSED
	attackcanceler
	setmultistriketriattacktype
	callnative BS_TriAttackStrikeCancelerPrologue
	callnative BS_TriAttackStrikeCanceler
	goto BattleScript_HitFromAccCheck
