#include "global.h"
#include "battle_transition.h"
#include "battle_transition_shared.h"
#include "battle_transition_tasks/tpp_host.h"
#include "palette.h"
#include "random.h"
#include "scanline_effect.h"
#include "task.h"
#include "constants/rgb.h"

static bool8 TppHost_Init(struct Task *);
static bool8 TppHost_WipeToBlack(struct Task *);
static bool8 TppHost_Main1(struct Task *);
static bool8 TppHost_Main2(struct Task *);
static bool8 TppHost_Main3(struct Task *);
static bool8 TppHost_FadeOut(struct Task *);
static bool8 TppHost_End(struct Task *);

static const u16 sTppHost_Palette[] = INCBIN_U16("graphics/battle_transitions/tpp_host.gbapal");
static const u32 sTppHost_Tileset[] = INCBIN_U32("graphics/battle_transitions/tpp_host.4bpp.lz");

#define INPUT_TYPES (8)
#define WINDOW_SPEED_DOWN (3)
#define SCROLL_ACCELERATE_FIRST (24)
#define SCROLL_ACCELERATE_REST (12)
#define MAX_SCROLL_SPEED (20)

#define tWindowBottom data[2]
#define tPlacedRow data[3]
#define tScrollPosition data[4]
#define tScrollSpeed data[5]
#define tScrollAccelerateCounter data[6]

static const TransitionStateFunc sTppHost_Funcs[] = {
    TppHost_Init,
    TppHost_Main1,
    TppHost_Main2,
    TppHost_Main3,
    TppHost_FadeOut,
    TppHost_End
};

void Task_TppHost(u8 taskId)
{
    while (sTppHost_Funcs[gTasks[taskId].tState](&gTasks[taskId]));
}

static bool8 TppHost_Init(struct Task *task)
{
    u16 *tilemap, *tileset;
    InitTransitionData();
    ScanlineEffect_Clear();

    REG_WIN0V = 0;
    REG_WIN0H = DISPLAY_WIDTH;
    REG_WININ = WININ_WIN0_OBJ | WININ_WIN0_BG0;
    REG_WINOUT = WININ_WIN0_OBJ | WININ_WIN0_BG1 | WININ_WIN0_BG2 | WININ_WIN0_BG3;
    // Place BG0 below the OAMs, and don't smash the data used by the other BGs
    // BG0 being below the other BGs doesn't matter because BG0 and the other BGs aren't enabled at the same time
    REG_BG0CNT = 0x180B;

    task->tWindowBottom = 0;
    task->tPlacedRow = 0;
    task->tScrollPosition = 0;
    task->tScrollSpeed = 2;
    task->tScrollAccelerateCounter = SCROLL_ACCELERATE_FIRST;

    GetBg0TilesDst(&tilemap, &tileset);
    LoadPalette(sTppHost_Palette, BG_PLTT_ID(15), sizeof(sTppHost_Palette));
    LZ77UnCompVram(sTppHost_Tileset, tileset);

    task->tState++;
    return FALSE;
}

static void TppHost_PlaceInputs(struct Task *task)
{
    s16 i;
    u16 *tilemap, *tileset;
    GetBg0TilesDst(&tilemap, &tileset);

    for (i = 0; i < DISPLAY_TILE_WIDTH; i++)
    {
        SET_TILE(tilemap, task->tPlacedRow, i, Random() % INPUT_TYPES);
    }

    task->tPlacedRow += 1;
}

static void TppHost_RevealInputs(struct Task *task)
{
    task->tWindowBottom += WINDOW_SPEED_DOWN;
    REG_WIN0V = task->tWindowBottom;
}

static void TppHost_ScrollInputs(struct Task *task)
{
    task->tScrollAccelerateCounter--;
    task->tScrollPosition = (task->tScrollPosition + task->tScrollSpeed) & 0xFF;
    REG_BG0VOFS = task->tScrollPosition;

    if (task->tScrollAccelerateCounter <= 0) {
        task->tScrollSpeed += task->tScrollSpeed >> 1;
        task->tScrollAccelerateCounter = SCROLL_ACCELERATE_REST;
    }
}

static bool8 TppHost_Main1(struct Task *task)
{
    TppHost_PlaceInputs(task);
    TppHost_RevealInputs(task);
    TppHost_ScrollInputs(task);

    if (task->tPlacedRow >= 32)
    {
        task->tState++;
    }
    return FALSE;
}

static bool8 TppHost_Main2(struct Task *task)
{
    TppHost_RevealInputs(task);
    TppHost_ScrollInputs(task);

    if (task->tWindowBottom >= DISPLAY_HEIGHT)
    {
        task->tState++;
    }
    return FALSE;
}

static bool8 TppHost_Main3(struct Task *task)
{
    TppHost_ScrollInputs(task);

    if (task->tScrollSpeed > MAX_SCROLL_SPEED) {
        task->tState++;
    }
    return FALSE;
}

static bool8 TppHost_FadeOut(struct Task *task)
{
    BeginNormalPaletteFade(PALETTES_OBJECTS | (1 << 15), 1, 0, 16, RGB_BLACK);
    task->tState++;
    return FALSE;
}

static bool8 TppHost_End(struct Task *task)
{
    if (!gPaletteFade.active)
    {
        DmaStop(0);
        FadeScreenBlack();
        DestroyTask(FindTaskIdByFunc(task->func));
    }
    return FALSE;
}
