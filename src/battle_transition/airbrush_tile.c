#include "global.h"
#include "battle_transition.h"
#include "battle_transition_shared.h"
#include "battle_transition_tasks/airbrush_tile.h"
#include "palette.h"
#include "random.h"
#include "scanline_effect.h"
#include "task.h"
#include "constants/rgb.h"

/*
A linear-feedback shift register with an appropriate polynomial has a period of `2^^n - 1`,
selecting every value between `1` and `2^^n - 1` inclusive exactly once.
This means that until the full period is reached,
the lfsr is guaranteed to choose a tile that has not yet been blacked-out.
 */

#define tLfsr           data[2]
#define tInitialLfsr    data[3]

static bool8 AirbrushTile_Init(struct Task *);
static bool8 AirbrushTile_Main(struct Task *);

static const TransitionStateFunc sAirbrushTile_Funcs[] = {
    AirbrushTile_Init,
    AirbrushTile_Main,
};

void Task_AirbrushTile(u8 taskId)
{
    while (sAirbrushTile_Funcs[gTasks[taskId].tState](&gTasks[taskId]));
}

static bool8 AirbrushTile_Init(struct Task *task)
{
    u16 *tilemap, *tileset;
    unsigned i;
    InitTransitionData();
    ScanlineEffect_Clear();

    FillPalette(RGB_BLACK, BG_PLTT_ID(15) + 0xF, PLTT_SIZEOF(1));

    GetBg0TilesDst(&tilemap, &tileset);
    for (i = 0; i < TILE_SIZE_4BPP / sizeof(u16); i++)
    {
        tileset[i] = 0;
        tileset[i + TILE_SIZE_4BPP/ sizeof(u16)] = 0xFFFF;
    }

    do
    {
         // this lfsr is 10-bit, but the exit condition requires that tInitialLfsr be less than 600
        task->tLfsr = Random() & 0x1FF;
    }
    while (task->tLfsr == 0);

    task->tInitialLfsr = task->tLfsr;

    task->tState++;
    return TRUE;
}

static bool8 AirbrushTile_Main(struct Task *task)
{
    u16 *tilemap, *tileset;
    unsigned tile_i, x, y, j;

    GetBg0TilesDst(&tilemap, &tileset);

    for (tile_i = 0; tile_i < 7; tile_i++)
    {
        do
        {
            unsigned carry = task->tLfsr & 1;
            task->tLfsr >>= 1;
            task->tLfsr ^= carry * 0x240;
        }
        while (task->tLfsr > 600);

        j = (600 == task->tLfsr ? 0 : task->tLfsr);
        x = j % 30;
        y = j / 30;
        tilemap[x + 32 * y] = 0xF001;

        if (task->tLfsr == task->tInitialLfsr)
        {
            FadeScreenBlack();
            DestroyTask(FindTaskIdByFunc(Task_AirbrushTile));
        }
    }

    return FALSE;
}
