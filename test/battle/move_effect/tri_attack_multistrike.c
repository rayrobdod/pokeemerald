#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(MoveHasAdditionalEffect(MOVE_TRI_ATTACK, MOVE_EFFECT_TRI_ATTACK) == TRUE);
    ASSUME(gMovesInfo[MOVE_TRI_ATTACK].effect == EFFECT_MULTISTRIKE_TRI_ATTACK);
    ASSUME(gMovesInfo[MOVE_TRI_ATTACK].strikeCount == 3);
}

SINGLE_BATTLE_TEST("Tri Attack hits 3 times")
{
    s16 firstHit;
    s16 secondHit;
    s16 thirdHit;

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_TRI_ATTACK); }
        TURN {}
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TRI_ATTACK, player);
        HP_BAR(opponent, captureDamage: &firstHit);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TRI_ATTACK, player);
        HP_BAR(opponent, captureDamage: &secondHit);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TRI_ATTACK, player);
        HP_BAR(opponent, captureDamage: &thirdHit);
        MESSAGE("The Pokémon was hit 3 time(s)!");
    } THEN {
        EXPECT_MUL_EQ(firstHit, Q_4_12(1.0), secondHit);
        EXPECT_MUL_EQ(firstHit, Q_4_12(1.0), thirdHit);
    }
}

SINGLE_BATTLE_TEST("Tri Attack reports the effectiveness of each hit individually (Grass)")
{
    s16 firstHit;
    s16 secondHit;
    s16 thirdHit;

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_MEGANIUM);
    } WHEN {
        TURN { MOVE(player, MOVE_TRI_ATTACK); }
        TURN {}
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TRI_ATTACK, player);
        HP_BAR(opponent, captureDamage: &firstHit);
        MESSAGE("It's super effective!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TRI_ATTACK, player);
        HP_BAR(opponent, captureDamage: &secondHit);
        MESSAGE("It's super effective!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TRI_ATTACK, player);
        HP_BAR(opponent, captureDamage: &thirdHit);
        MESSAGE("It's not very effective…");
        // TODO: check that the not very effective message is not printed a second time
        MESSAGE("The Pokémon was hit 3 time(s)!");
    } THEN {
        EXPECT_MUL_EQ(firstHit, Q_4_12(1.0), secondHit);
        EXPECT_MUL_EQ(firstHit, Q_4_12(0.25), thirdHit);
    }
}

SINGLE_BATTLE_TEST("Tri Attack reports the effectiveness of each hit individually (Ice)")
{
    s16 firstHit;
    s16 secondHit;
    s16 thirdHit;

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_GLALIE);
    } WHEN {
        TURN { MOVE(player, MOVE_TRI_ATTACK); }
        TURN {}
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TRI_ATTACK, player);
        HP_BAR(opponent, captureDamage: &firstHit);
        MESSAGE("It's super effective!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TRI_ATTACK, player);
        HP_BAR(opponent, captureDamage: &secondHit);
        MESSAGE("It's not very effective…");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TRI_ATTACK, player);
        HP_BAR(opponent, captureDamage: &thirdHit);
        MESSAGE("The Pokémon was hit 3 time(s)!");
    } THEN {
        EXPECT_MUL_EQ(firstHit, Q_4_12(0.25), secondHit);
        EXPECT_MUL_EQ(firstHit, Q_4_12(0.5), thirdHit);
    }
}

SINGLE_BATTLE_TEST("Tri Attack reports the effectiveness of each hit individually (Ground)")
{
    s16 firstHit;
    s16 secondHit;

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_MUDSDALE);
    } WHEN {
        TURN { MOVE(player, MOVE_TRI_ATTACK); }
        TURN {}
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TRI_ATTACK, player);
        HP_BAR(opponent, captureDamage: &firstHit);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TRI_ATTACK, player);
        HP_BAR(opponent, captureDamage: &secondHit);
        MESSAGE("It's super effective!");
        MESSAGE("It doesn't affect the\nopposing Mudsdale…");
        MESSAGE("The Pokémon was hit 3 time(s)!");
    } THEN {
        // Ground is neutral to fire, weak to ice and immune to electric
        EXPECT_MUL_EQ(firstHit, Q_4_12(2.0), secondHit);
    }
}

SINGLE_BATTLE_TEST("Tri Attack's first hit can inflict burn")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
        PASSES_RANDOMLY(10, 100, RNG_SECONDARY_EFFECT);
    } WHEN {
        TURN { MOVE(player, MOVE_TRI_ATTACK); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TRI_ATTACK, player);
        HP_BAR(opponent);
        ANIMATION(ANIM_TYPE_STATUS, B_ANIM_STATUS_BRN, opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TRI_ATTACK, player);
        HP_BAR(opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TRI_ATTACK, player);
        HP_BAR(opponent);
        MESSAGE("The Pokémon was hit 3 time(s)!");
    }
}

SINGLE_BATTLE_TEST("Tri Attack's second hit can inflict freeze")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        // Ideally, the rng could be used to make the first hit's status check fail
        // Failing that, we make the opponent immune to burn
        OPPONENT(SPECIES_SLUGMA) { Ability(ABILITY_FLAME_BODY); };
        PASSES_RANDOMLY(10, 100, RNG_SECONDARY_EFFECT);
    } WHEN {
        TURN { MOVE(player, MOVE_TRI_ATTACK); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TRI_ATTACK, player);
        HP_BAR(opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TRI_ATTACK, player);
        HP_BAR(opponent);
        FREEZE_OR_FROSTBURN_STATUS(opponent, TRUE);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TRI_ATTACK, player);
        HP_BAR(opponent);
        MESSAGE("The Pokémon was hit 3 time(s)!");
    }
}

SINGLE_BATTLE_TEST("Tri Attack's third hit can inflict paralysis")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        // Ideally, the rng could be used to make the first two hits' status checks fail
        // Failing that, we make the opponent immune to burn and freeze
        OPPONENT(SPECIES_SLUGMA) { Ability(ABILITY_MAGMA_ARMOR); };
        PASSES_RANDOMLY(10, 100, RNG_SECONDARY_EFFECT);
    } WHEN {
        TURN { MOVE(player, MOVE_TRI_ATTACK); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TRI_ATTACK, player);
        HP_BAR(opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TRI_ATTACK, player);
        HP_BAR(opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TRI_ATTACK, player);
        HP_BAR(opponent);
        ANIMATION(ANIM_TYPE_STATUS, B_ANIM_STATUS_PRZ, opponent);
        MESSAGE("The Pokémon was hit 3 time(s)!");
    }
}

SINGLE_BATTLE_TEST("Tri Attack continues even if the defender is immune to the first hit")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_ARCANINE) { Ability(ABILITY_FLASH_FIRE); };
    } WHEN {
        TURN { MOVE(player, MOVE_TRI_ATTACK); }
        TURN {}
    } SCENE {
        ABILITY_POPUP(opponent, ABILITY_FLASH_FIRE);
        MESSAGE("The opposing Arcanine's Flash Fire\nraised the power of Fire-type moves!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TRI_ATTACK, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TRI_ATTACK, player);
        MESSAGE("The Pokémon was hit 3 time(s)!");
    }
}
