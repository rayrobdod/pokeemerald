#include "global.h"
#include "test/battle.h"
#include "move.h"
#include "pokemon.h"
#include "technique_manual.h"

#define gSaveBlockTm gSaveBlock3Ptr->techniqueManual

SINGLE_BATTLE_TEST("Technique Manual: TM_TASK_SEEN: Seeing a move increments that move's counter")
{
    enum Move move = 0;
    u32 counter = 0;

    for (unsigned page = 0; page < TM_COUNT; page++)
    for (unsigned task = 0; task < TASKS_PER_PAGE; task++)
    {
        if (gTechniqueManualPages[page].tasks[task].type == TM_TASK_SEEN)
        {
            PARAMETRIZE {
                move = gTechniqueManualPages[page].move;
                counter = gTechniqueManualPages[page].tasks[task].storage_index;
            }
        }
    }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
        gSaveBlockTm.counters[counter] = 2;
    }
    WHEN {
        TURN { MOVE(player, move); }
    }
    SCENE {
        ANIMATION(ANIM_TYPE_MOVE, move, player);
    }
    THEN {
        EXPECT_EQ(gSaveBlockTm.counters[counter], 3);
    }
}

SINGLE_BATTLE_TEST("Technique Manual: TM_TASK_SEEN_DIFFERENT_SPECIES: Seeing a move records the species used")
{
    u32 move = 0;
    u32 counter = 0;

    for (unsigned page = 0; page < TM_COUNT; page++)
    for (unsigned task = 0; task < TASKS_PER_PAGE; task++)
    {
        if (gTechniqueManualPages[page].tasks[task].type == TM_TASK_SEEN_DIFFERENT_SPECIES)
        {
            PARAMETRIZE {
                move = gTechniqueManualPages[page].move;
                counter = gTechniqueManualPages[page].tasks[task].storage_index;
            }
        }
    }

    GIVEN {
        gSaveBlockTm.species[counter] = SPECIES_NONE;
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    }
    WHEN {
        TURN { MOVE(player, move); }
    }
    SCENE {
        ANIMATION(ANIM_TYPE_MOVE, move, player);
    }
    THEN {
        EXPECT_EQ(gSaveBlockTm.species[counter], SPECIES_WOBBUFFET);
    }
}

SINGLE_BATTLE_TEST("Technique Manual: TM_TASK_SEEN_DIFFERENT_SPECIES: Seeing a move with an already-recorded species does not record the species again")
{
    u32 move = 0;
    u32 counter = 0;

    for (unsigned page = 0; page < TM_COUNT; page++)
    for (unsigned task = 0; task < TASKS_PER_PAGE; task++)
    {
        if (gTechniqueManualPages[page].tasks[task].type == TM_TASK_SEEN_DIFFERENT_SPECIES &&
            gTechniqueManualPages[page].tasks[task].requirement >= 2)
        {
            PARAMETRIZE {
                move = gTechniqueManualPages[page].move;
                counter = gTechniqueManualPages[page].tasks[task].storage_index;
            }
        }
    }

    GIVEN {
        gSaveBlockTm.species[counter] = SPECIES_WOBBUFFET;
        gSaveBlockTm.species[counter + 1] = SPECIES_NONE;
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    }
    WHEN {
        TURN { MOVE(player, move); }
    }
    SCENE {
        ANIMATION(ANIM_TYPE_MOVE, move, player);
    }
    THEN {
        EXPECT_EQ(gSaveBlockTm.species[counter], SPECIES_WOBBUFFET);
        EXPECT_EQ(gSaveBlockTm.species[counter + 1], SPECIES_NONE);
    }
}

SINGLE_BATTLE_TEST("Technique Manual: TM_TASK_SEEN_DIFFERENT_SPECIES: Seeing a move with a second species records the species")
{
    u32 move = 0;
    u32 counter = 0;

    for (unsigned page = 0; page < TM_COUNT; page++)
    for (unsigned task = 0; task < TASKS_PER_PAGE; task++)
    {
        if (gTechniqueManualPages[page].tasks[task].type == TM_TASK_SEEN_DIFFERENT_SPECIES &&
            gTechniqueManualPages[page].tasks[task].requirement >= 2)
        {
            PARAMETRIZE {
                move = gTechniqueManualPages[page].move;
                counter = gTechniqueManualPages[page].tasks[task].storage_index;
            }
        }
    }

    GIVEN {
        gSaveBlockTm.species[counter] = SPECIES_WOBBUFFET;
        gSaveBlockTm.species[counter + 1] = SPECIES_NONE;
        PLAYER(SPECIES_WYNAUT);
        OPPONENT(SPECIES_WOBBUFFET);
    }
    WHEN {
        TURN { MOVE(player, move); }
    }
    SCENE {
        ANIMATION(ANIM_TYPE_MOVE, move, player);
    }
    THEN {
        EXPECT_EQ(gSaveBlockTm.species[counter], SPECIES_WOBBUFFET);
        EXPECT_EQ(gSaveBlockTm.species[counter + 1], SPECIES_WYNAUT);
    }
}

SINGLE_BATTLE_TEST("Technique Manual: TM_TASK_SEEN_RAIN: using in rain increments counter")
{
    const u32 page = TM_THUNDER;
    const u32 task = 2;

    const enum Move move = gTechniqueManualPages[page].move;
    const u32 counter = gTechniqueManualPages[page].tasks[task].storage_index;

    bool32 with_rain = FALSE;
    PARAMETRIZE { with_rain = TRUE; }
    PARAMETRIZE { with_rain = FALSE; }

    GIVEN {
        ASSUME(TM_TASK_SEEN_RAIN == gTechniqueManualPages[page].tasks[task].type);
        gSaveBlockTm.counters[counter] = 2;

        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    }
    WHEN {
        if (with_rain)
            TURN { MOVE(player, MOVE_RAIN_DANCE); }
        TURN { MOVE(player, move); }
    }
    SCENE {
        ANIMATION(ANIM_TYPE_MOVE, move, player);
    }
    THEN {
        EXPECT_EQ(gSaveBlockTm.counters[counter], (with_rain ? 3 : 2));
    }
}

SINGLE_BATTLE_TEST("Technique Manual: TM_TASK_SEEN_SUN: using in sun increments counter")
{
    const u32 page = TM_SOLAR_BEAM;
    const u32 task = 2;

    const enum Move move = gTechniqueManualPages[page].move;
    const u32 counter = gTechniqueManualPages[page].tasks[task].storage_index;

    bool32 with_sun = FALSE;
    PARAMETRIZE { with_sun = TRUE; }
    PARAMETRIZE { with_sun = FALSE; }

    GIVEN {
        ASSUME(TM_TASK_SEEN_SUN == gTechniqueManualPages[page].tasks[task].type);
        gSaveBlockTm.counters[counter] = 2;

        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    }
    WHEN {
        if (with_sun)
            TURN { MOVE(player, MOVE_SUNNY_DAY); }
        TURN { MOVE(player, move); }
    }
    SCENE {
        ANIMATION(ANIM_TYPE_MOVE, move, player);
    }
    THEN {
        EXPECT_EQ(gSaveBlockTm.counters[counter], (with_sun ? 3 : 2));
    }
}

SINGLE_BATTLE_TEST("Technique Manual: TM_COUNTER_WATER_USING_ICE increments if a water type uses an ice move")
{
    enum Move move = MOVE_NONE;
    u32 species = SPECIES_NONE;

    PARAMETRIZE {species = SPECIES_VAPOREON; move = MOVE_ICE_BEAM;}
    PARAMETRIZE {species = SPECIES_VAPOREON; move = MOVE_FLAMETHROWER;}
    PARAMETRIZE {species = SPECIES_FLAREON; move = MOVE_ICE_BEAM;}

    GIVEN {
        ASSUME(IsSpeciesOfType(SPECIES_VAPOREON, TYPE_WATER));
        ASSUME(!IsSpeciesOfType(SPECIES_FLAREON, TYPE_WATER));
        ASSUME(GetMoveType(MOVE_ICE_BEAM) == TYPE_ICE);
        ASSUME(GetMoveType(MOVE_FLAMETHROWER) != TYPE_ICE);
        ASSUME(0 <= TM_COUNTER_WATER_USING_ICE);
        gSaveBlockTm.counters[TM_COUNTER_WATER_USING_ICE] = 2;

        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(species);
    }
    WHEN {
        TURN { MOVE(opponent, move); }
    }
    SCENE {
        ANIMATION(ANIM_TYPE_MOVE, move, opponent);
    }
    THEN {
        EXPECT_EQ(gSaveBlockTm.counters[TM_COUNTER_WATER_USING_ICE],
            (species == SPECIES_VAPOREON && move == MOVE_ICE_BEAM ? 3 : 2));
    }
}
