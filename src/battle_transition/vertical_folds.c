#include "global.h"
#include "battle_transition.h"
#include "battle_transition_shared.h"
#include "battle_transition_tasks/vertical_folds.h"
#include "gpu_regs.h"
#include "main.h"
#include "scanline_effect.h"
#include "task.h"

static bool8 VerticalFolds_Init(struct Task *);
static bool8 VerticalFolds_Main(struct Task *);
static bool8 VerticalFolds_Main_Centered(struct Task *);
static void HBlankCB_VerticalFolds(void);
static void VBlankCB_VerticalFolds(void);
static void VBlankCB_VerticalFolds_First(void);

#define SEGMENT_HEIGHT (16)
#define FOLD_DURATION (32)
#define FOLD_START_OFFSET (6)
#define SEGMENT_COUNT (DISPLAY_HEIGHT / SEGMENT_HEIGHT + (DISPLAY_HEIGHT % SEGMENT_HEIGHT ? 1 : 0))

#define WIN_REG_BUFFER_OFFSET (DISPLAY_HEIGHT + SEGMENT_HEIGHT)

#define tTimer data[2]

static const TransitionStateFunc sVerticalFolds_Funcs[] = {
    VerticalFolds_Init,
    VerticalFolds_Main,
};

static const TransitionStateFunc sVerticalFoldsCentered_Funcs[] = {
    VerticalFolds_Init,
    VerticalFolds_Main_Centered,
};

void Task_VerticalFolds(u8 taskId)
{
    while (sVerticalFolds_Funcs[gTasks[taskId].tState](&gTasks[taskId]));
}

void Task_VerticalFoldsCentered(u8 taskId)
{
    while (sVerticalFoldsCentered_Funcs[gTasks[taskId].tState](&gTasks[taskId]));
}

static bool8 VerticalFolds_Init(struct Task *task)
{
    int i;

    InitTransitionData();
    ScanlineEffect_Clear();

    sTransitionData->WININ = WININ_WIN0_ALL;
    sTransitionData->WINOUT = 0;
    sTransitionData->WIN0V = DISPLAY_HEIGHT;
    sTransitionData->WIN0H = DISPLAY_WIDTH;
    sTransitionData->VBlank_DMA = FALSE;

    for (i = 0; i < DISPLAY_HEIGHT; i++)
    {
        gScanlineEffectRegBuffers[1][i] = 0;
        gScanlineEffectRegBuffers[1][WIN_REG_BUFFER_OFFSET + i] = DISPLAY_WIDTH;
    }

    SetVBlankCallback(VBlankCB_VerticalFolds_First);
    SetHBlankCallback(HBlankCB_VerticalFolds);

    EnableInterrupts(INTR_FLAG_HBLANK);

    task->tState++;
    return TRUE;
}

static bool8 VerticalFolds_Main(struct Task *task)
{
    int i, j;
    sTransitionData->VBlank_DMA = FALSE;

    for (i = 0; i < SEGMENT_COUNT; i++)
    {
        int fold_progress = task->tTimer - FOLD_START_OFFSET * i;
        if (fold_progress < 0)
            fold_progress = 0;
        if (fold_progress > FOLD_DURATION)
            fold_progress = FOLD_DURATION;

        for (j = 0; j < SEGMENT_HEIGHT; j++)
        {
            int row = i * SEGMENT_HEIGHT + j;

            bool32 winv = (SEGMENT_HEIGHT - 1 - j) * FOLD_DURATION >= fold_progress * SEGMENT_HEIGHT;
            int offy = j * fold_progress / (FOLD_DURATION / 2);

            gScanlineEffectRegBuffers[0][row] = offy;
            gScanlineEffectRegBuffers[0][WIN_REG_BUFFER_OFFSET + row] = (winv ? DISPLAY_WIDTH : 0);
        }
    }

    if (++task->tTimer == SEGMENT_COUNT * FOLD_START_OFFSET + FOLD_DURATION)
    {
        DestroyTask(FindTaskIdByFunc(Task_VerticalFolds));
    }

    sTransitionData->VBlank_DMA = TRUE;
    return FALSE;
}

static bool8 VerticalFolds_Main_Centered(struct Task *task)
{
    int i, j;
    sTransitionData->VBlank_DMA = FALSE;

    for (i = 0; i < SEGMENT_COUNT; i++)
    {
        int fold_progress = task->tTimer - FOLD_START_OFFSET * i;
        if (fold_progress < 0)
            fold_progress = 0;
        if (fold_progress > FOLD_DURATION)
            fold_progress = FOLD_DURATION;

        for (j = 0; j < SEGMENT_HEIGHT; j++)
        {
            int row = i * SEGMENT_HEIGHT + j;

            bool32 winv;
            int offy;

            if (j < SEGMENT_HEIGHT / 2)
                winv = j * FOLD_DURATION >= fold_progress * (SEGMENT_HEIGHT / 2);
            else
                winv = (SEGMENT_HEIGHT - 1 - j) * FOLD_DURATION >= fold_progress * (SEGMENT_HEIGHT / 2);

            offy = (j - (SEGMENT_HEIGHT / 2)) * fold_progress / (FOLD_DURATION / 2);

            gScanlineEffectRegBuffers[0][row] = offy;
            gScanlineEffectRegBuffers[0][WIN_REG_BUFFER_OFFSET + row] = (winv ? DISPLAY_WIDTH : 0);
        }
    }

    if (++task->tTimer == SEGMENT_COUNT * FOLD_START_OFFSET + FOLD_DURATION)
    {
        DestroyTask(FindTaskIdByFunc(Task_VerticalFoldsCentered));
    }

    sTransitionData->VBlank_DMA = TRUE;
    return FALSE;
}

static void VBlankCB_VerticalFolds_First(void)
{
    // setting these registers on later VBlanks seems to enable BG1, somehow
    REG_WININ = sTransitionData->WININ;
    REG_WINOUT = sTransitionData->WINOUT;
    REG_WIN0V = sTransitionData->WIN0V;
    SetVBlankCallback(VBlankCB_VerticalFolds);
    VBlankCB_VerticalFolds();
}

static void VBlankCB_VerticalFolds(void)
{
    u16 offset0, offset;

    VBlankCB_BattleTransition();
    if (sTransitionData->VBlank_DMA)
        DmaCopy16(3, gScanlineEffectRegBuffers[0], gScanlineEffectRegBuffers[1], (WIN_REG_BUFFER_OFFSET + DISPLAY_HEIGHT) * 2);

    offset0 = gScanlineEffectRegBuffers[1][0];
    offset = offset0 + sTransitionData->cameraY;
    REG_BG0VOFS = offset0;
    REG_BG1VOFS = offset;
    REG_BG2VOFS = offset;
    REG_BG3VOFS = offset;
    REG_WIN0H = gScanlineEffectRegBuffers[1][WIN_REG_BUFFER_OFFSET + 0];
}

static void HBlankCB_VerticalFolds(void)
{
    if (REG_VCOUNT < DISPLAY_HEIGHT)
    {
        u16 offset0 = gScanlineEffectRegBuffers[1][REG_VCOUNT];
        u16 offset = offset0 + sTransitionData->cameraY;
        REG_BG0VOFS = offset0;
        REG_BG1VOFS = offset;
        REG_BG2VOFS = offset;
        REG_BG3VOFS = offset;
        REG_WIN0H = gScanlineEffectRegBuffers[1][WIN_REG_BUFFER_OFFSET + REG_VCOUNT];
    }
}
