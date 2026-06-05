#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Protean/Libero changes the type of the user to the move used every time (Gen6-8)")
{
    enum Ability ability;
    u32 species;
    PARAMETRIZE { ability = ABILITY_PROTEAN; species = SPECIES_KECLEON; }
    PARAMETRIZE { ability = ABILITY_LIBERO;  species = SPECIES_RABOOT; }
    GIVEN {
        WITH_CONFIG(B_PROTEAN_LIBERO, GEN_6);
        PLAYER(SPECIES_REGIROCK);
        OPPONENT(species) { Ability(ability); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponent, MOVE_WATER_GUN); }
        TURN { MOVE(opponent, MOVE_SCRATCH); }
        TURN { SWITCH(opponent, 1); }
        TURN { SWITCH(opponent, 0); }
        TURN { MOVE(opponent, MOVE_WATER_GUN); }
    } SCENE {
        ABILITY_POPUP(opponent, ability);
        if (species == SPECIES_KECLEON)
            MESSAGE("The opposing Kecleon transformed into the Water type!");
        else
            MESSAGE("The opposing Raboot transformed into the Water type!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_WATER_GUN, opponent);
        ABILITY_POPUP(opponent, ability);
        if (species == SPECIES_KECLEON)
            MESSAGE("The opposing Kecleon transformed into the Normal type!");
        else
            MESSAGE("The opposing Raboot transformed into the Normal type!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SCRATCH, opponent);
        ABILITY_POPUP(opponent, ability);
        if (species == SPECIES_KECLEON)
            MESSAGE("The opposing Kecleon transformed into the Water type!");
        else
            MESSAGE("The opposing Raboot transformed into the Water type!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_WATER_GUN, opponent);
    }
}

SINGLE_BATTLE_TEST("Protean/Libero changes the type of the user only once per switch in (Gen9+)")
{
    enum Ability ability; u32 species;
    PARAMETRIZE { ability = ABILITY_PROTEAN; species = SPECIES_KECLEON; }
    PARAMETRIZE { ability = ABILITY_LIBERO;  species = SPECIES_RABOOT; }
    GIVEN {
        WITH_CONFIG(B_PROTEAN_LIBERO, GEN_9);
        PLAYER(SPECIES_REGIROCK);
        OPPONENT(species) { Ability(ability); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponent, MOVE_WATER_GUN); }
        TURN { MOVE(opponent, MOVE_SCRATCH); }
        TURN { SWITCH(opponent, 1); }
        TURN { SWITCH(opponent, 0); }
        TURN { MOVE(opponent, MOVE_WATER_GUN); }
    } SCENE {
        ABILITY_POPUP(opponent, ability);
        if (species == SPECIES_KECLEON)
            MESSAGE("The opposing Kecleon transformed into the Water type!");
        else
            MESSAGE("The opposing Raboot transformed into the Water type!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_WATER_GUN, opponent);
        NONE_OF {
            ABILITY_POPUP(opponent, ability);
            MESSAGE("The opposing Kecleon transformed into the Normal type!");
            MESSAGE("The opposing Raboot transformed into the Normal type!");
        }
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SCRATCH, opponent);
        ABILITY_POPUP(opponent, ability);
        if (species == SPECIES_KECLEON)
            MESSAGE("The opposing Kecleon transformed into the Water type!");
        else
            MESSAGE("The opposing Raboot transformed into the Water type!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_WATER_GUN, opponent);
    }
}

SINGLE_BATTLE_TEST("Protean/Libero changes the type of the user to each of Tri Attack's strikes (Gen6-8)")
{
    u32 ability, species;
    PARAMETRIZE { ability = ABILITY_PROTEAN; species = SPECIES_FROAKIE; }
    PARAMETRIZE { ability = ABILITY_LIBERO;  species = SPECIES_RABOOT; }
    GIVEN {
        ASSUME(gMovesInfo[MOVE_TRI_ATTACK].effect == EFFECT_MULTISTRIKE_TRI_ATTACK);
        ASSUME(gMovesInfo[MOVE_TRI_ATTACK].strikeCount == 3);
        ASSUME(gSpeciesInfo[SPECIES_RABOOT].types[0] == TYPE_FIRE);
        WITH_CONFIG(B_PROTEAN_LIBERO, GEN_6);
        PLAYER(SPECIES_REGIROCK);
        OPPONENT(species) { Ability(ability); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_TRI_ATTACK); }
    } SCENE {
        if (species == SPECIES_FROAKIE)
        {
            ABILITY_POPUP(opponent, ability);
            MESSAGE("The opposing Froakie transformed into the Fire type!");
        }
        else
        {
            // Raboot is already Fire, so will not become Fire type
            NONE_OF {
                ABILITY_POPUP(opponent, ability);
                MESSAGE("The opposing Raboot transformed into the Fire type!");
            }
        }
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TRI_ATTACK, opponent);
        ABILITY_POPUP(opponent, ability);
        if (species == SPECIES_FROAKIE)
            MESSAGE("The opposing Froakie transformed into the Ice type!");
        else
            MESSAGE("The opposing Raboot transformed into the Ice type!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TRI_ATTACK, opponent);
        ABILITY_POPUP(opponent, ability);
        if (species == SPECIES_FROAKIE)
            MESSAGE("The opposing Froakie transformed into the Electric type!");
        else
            MESSAGE("The opposing Raboot transformed into the Electric type!");
    }
}

SINGLE_BATTLE_TEST("Protean/Libero changes the type of the user to Tri Attack's first strike (Gen9+)")
{
    u32 ability, species;
    PARAMETRIZE { ability = ABILITY_PROTEAN; species = SPECIES_FROAKIE; }
    PARAMETRIZE { ability = ABILITY_LIBERO;  species = SPECIES_RABOOT; }
    GIVEN {
        ASSUME(gMovesInfo[MOVE_TRI_ATTACK].effect == EFFECT_MULTISTRIKE_TRI_ATTACK);
        ASSUME(gMovesInfo[MOVE_TRI_ATTACK].strikeCount == 3);
        ASSUME(gSpeciesInfo[SPECIES_RABOOT].types[0] == TYPE_FIRE);
        WITH_CONFIG(B_PROTEAN_LIBERO, GEN_9);
        PLAYER(SPECIES_REGIROCK);
        OPPONENT(species) { Ability(ability); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_TRI_ATTACK); }
    } SCENE {
        if (species == SPECIES_FROAKIE)
        {
            ABILITY_POPUP(opponent, ability);
            MESSAGE("The opposing Froakie transformed into the Fire type!");
        }
        else
        {
            // Raboot is already Fire, so will not become Fire type
            NONE_OF {
                ABILITY_POPUP(opponent, ability);
                MESSAGE("The opposing Raboot transformed into the Fire type!");
            }
        }
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TRI_ATTACK, opponent);
        if (species == SPECIES_FROAKIE)
        {
            NONE_OF {
                ABILITY_POPUP(opponent, ability);
                MESSAGE("The opposing Froakie transformed into the Ice type!");
            }
        }
        else
        {
            ABILITY_POPUP(opponent, ability);
            MESSAGE("The opposing Raboot transformed into the Ice type!");
        }
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TRI_ATTACK, opponent);
        NONE_OF {
            ABILITY_POPUP(opponent, ability);
            MESSAGE("The opposing Froakie transformed into the Electric type!");
            MESSAGE("The opposing Raboot transformed into the Electric type!");
        }
    }
}

SINGLE_BATTLE_TEST("Protean/Libero does not change the user's type when using Struggle")
{
    enum Ability ability; u32 species;
    PARAMETRIZE { ability = ABILITY_PROTEAN; species = SPECIES_GRENINJA; }
    PARAMETRIZE { ability = ABILITY_LIBERO;  species = SPECIES_RABOOT; }
    GIVEN {
        PLAYER(SPECIES_REGIROCK);
        OPPONENT(species) { Ability(ability); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_STRUGGLE); }
    } SCENE {
        NONE_OF {
            ABILITY_POPUP(opponent, ability);
            MESSAGE("The opposing Greninja transformed into the Normal type!");
            MESSAGE("The opposing Raboot transformed into the Normal type!");
        }
        ANIMATION(ANIM_TYPE_MOVE, MOVE_STRUGGLE, opponent);
    }
}
