	.include "asm/macros/battle_script.inc"

	.section script_data, "aw", %progbits

BattleScript_EffectMultistrikeTriAttack::
	attackcanceler
	setmultistriketriattacktype
	callnative BS_TryBlockMultistrikeTriAttack
	goto BattleScript_HitFromAccCheck
