#include "global.h"
#include "battle_transition.h"
#include "battle_transition_shared.h"
#include "battle_transition_tasks/horizontal_folds.h"
#include "gpu_regs.h"
#include "main.h"
#include "palette.h"
#include "scanline_effect.h"
#include "task.h"
#include "constants/rgb.h"

static bool8 HorizontalFolds_Init(struct Task *);
static bool8 HorizontalFolds_Main(struct Task *);
static bool8 HorizontalFolds_Main_Centered(struct Task *);

#define SEGMENT_TILE_WIDTH (2)
#define SEGMENT_WIDTH (TILE_WIDTH * SEGMENT_TILE_WIDTH)
#define FOLD_DURATION (20)
#define FOLD_START_OFFSET (5)
#define SEGMENT_COUNT (DISPLAY_TILE_WIDTH / SEGMENT_TILE_WIDTH + \
        (DISPLAY_TILE_WIDTH % SEGMENT_TILE_WIDTH ? 1 : 0))

#define tTimer data[2]

static const TransitionStateFunc sHorizontalFolds_Funcs[] = {
    HorizontalFolds_Init,
    HorizontalFolds_Main,
};

static const TransitionStateFunc sHorizontalFoldsCentered_Funcs[] = {
    HorizontalFolds_Init,
    HorizontalFolds_Main_Centered,
};

void Task_HorizontalFolds(u8 taskId)
{
    while (sHorizontalFolds_Funcs[gTasks[taskId].tState](&gTasks[taskId]));
}

void Task_HorizontalFoldsCentered(u8 taskId)
{
    while (sHorizontalFoldsCentered_Funcs[gTasks[taskId].tState](&gTasks[taskId]));
}

static void HorizontalFolds_BuildTileset(u16 *tileset) {
    u32 currentValue = 0;
    unsigned offset = 0;
    unsigned tile_i, y;

    for (tile_i = 0; tile_i <= TILE_WIDTH; tile_i++) {
        for (y = 0; y < TILE_HEIGHT; y++) {
            tileset[offset] = (u16) (currentValue & 0xFFFF);
            offset++;
            tileset[offset] = (u16) (currentValue >> 16);
            offset++;
        }
        currentValue |= (1 << ((TILE_WIDTH - 1 - tile_i) * 4));
    }
}

static bool8 HorizontalFolds_Init(struct Task *task)
{
    u16 *tilemap, *tileset;
    int tile_i, y, short_i;

    InitTransitionData();
    ScanlineEffect_Clear();

    sTransitionData->WININ = WININ_WIN0_ALL;
    sTransitionData->WINOUT = 0;
    sTransitionData->WIN0V = DISPLAY_HEIGHT;
    sTransitionData->WIN0H = DISPLAY_WIDTH;
    sTransitionData->VBlank_DMA = FALSE;

    GetBg0TilesDst(&tilemap, &tileset);
    FillPalette(RGB_BLACK, BG_PLTT_ID(15) + 1, PLTT_SIZEOF(1));
    HorizontalFolds_BuildTileset(tileset);

    task->tTimer = 0;

    task->tState++;
    return TRUE;
}

static bool8 HorizontalFolds_Main(struct Task *task)
{
    u16 *tilemap, *tileset;
    int fold_progress;
    u16 tile_id_left, tile_id_right;
    int i, j;

    GetBg0TilesDst(&tilemap, &tileset);

    for (i = 0; i < SEGMENT_COUNT; i++)
    {
        fold_progress = task->tTimer - FOLD_START_OFFSET * i;
        if (fold_progress < 0)
            fold_progress = 0;
        if (fold_progress > FOLD_DURATION)
            fold_progress = FOLD_DURATION;

        tile_id_left = max(fold_progress * SEGMENT_WIDTH / FOLD_DURATION, TILE_WIDTH) - TILE_WIDTH;
        tile_id_right = min(fold_progress * SEGMENT_WIDTH / FOLD_DURATION, TILE_WIDTH);

        for (j = 0; j < DISPLAY_TILE_HEIGHT; j++)
        {
            SET_TILE(tilemap, j, i * 2, tile_id_left);
            SET_TILE(tilemap, j, (i * 2 + 1), tile_id_right);
        }
    }

    if (++task->tTimer == SEGMENT_COUNT * FOLD_START_OFFSET + FOLD_DURATION)
    {
        BlendPalettes(PALETTES_ALL, 16, RGB_BLACK);
        DestroyTask(FindTaskIdByFunc(Task_HorizontalFolds));
    }

    return FALSE;
}

static bool8 HorizontalFolds_Main_Centered(struct Task *task)
{
    u16 *tilemap, *tileset;
    int fold_progress;
    u16 tile_id, flipped_tile_id;
    int i, j;

    GetBg0TilesDst(&tilemap, &tileset);

    for (i = 0; i < SEGMENT_COUNT; i++)
    {
        fold_progress = task->tTimer - FOLD_START_OFFSET * i;
        if (fold_progress < 0)
            fold_progress = 0;
        if (fold_progress > FOLD_DURATION)
            fold_progress = FOLD_DURATION;

        tile_id = (fold_progress * 8 / FOLD_DURATION);
        flipped_tile_id = 0x0400 | tile_id;

        for (j = 0; j < DISPLAY_TILE_HEIGHT; j++)
        {
            SET_TILE(tilemap, j, i * 2, flipped_tile_id);
            SET_TILE(tilemap, j, (i * 2 + 1), tile_id);
        }
    }

    if (++task->tTimer == SEGMENT_COUNT * FOLD_START_OFFSET + FOLD_DURATION)
    {
        BlendPalettes(PALETTES_ALL, 16, RGB_BLACK);
        DestroyTask(FindTaskIdByFunc(Task_HorizontalFoldsCentered));
    }

    return FALSE;
}
