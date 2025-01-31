#include "global.h"
#include "battle_transition.h"
#include "battle_transition_shared.h"
#include "battle_transition_tasks/star_iris.h"
#include "gpu_regs.h"
#include "main.h"
#include "scanline_effect.h"
#include "task.h"

#define Q_10_6(n) ((s16)((n) * 64))
#define Q_10_6_TO_INT(n) ((s16)((n) / 64))

static bool8 StarIris_Init(struct Task *);
static bool8 StarIris_FirstHalf(struct Task *);
static bool8 StarIris_SecondHalf(struct Task *);
static void HBlankCB_StarIris(void);
static void VBlankCB_StarIris(void);
static void VBlankCB_StarIris_First(void);
static void StarIris_UpdateSpeedsAccelerationsAndScanlineEffects(struct Task *task);
static void StarIris_SetScanlineEffectReflectionalSymmetryQuadrants(int row, int column);

#define FIRST_HALF_HEIGHT (60)

#define tEdgeX data[2]
#define tEdgeY data[3]
#define tPivotY data[4]

#define tEdgeDX data[5]
#define tEdgeDY data[6]
#define tPivotDY data[7]

#define tEdgeDDX data[8]
#define tEdgeDDY data[9]
#define tPivotDDY data[10]

static const TransitionStateFunc sStarIris_Funcs[] = {
    StarIris_Init,
    StarIris_FirstHalf,
    StarIris_SecondHalf,
};

void Task_StarIris(u8 taskId)
{
    while (sStarIris_Funcs[gTasks[taskId].tState](&gTasks[taskId]));
}

static bool8 StarIris_Init(struct Task *task)
{
    unsigned i;

    InitTransitionData();
    ScanlineEffect_Clear();

    sTransitionData->WININ = WININ_WIN0_ALL;
    sTransitionData->WINOUT = 0;
    sTransitionData->WIN0V = DISPLAY_HEIGHT;
    sTransitionData->WIN0H = DISPLAY_WIDTH;
    sTransitionData->VBlank_DMA = FALSE;

    for (i = 0; i < DISPLAY_HEIGHT; i++)
    {
        gScanlineEffectRegBuffers[1][i] = DISPLAY_WIDTH;
    }

    SetVBlankCallback(VBlankCB_StarIris_First);
    SetHBlankCallback(HBlankCB_StarIris);

    EnableInterrupts(INTR_FLAG_HBLANK);

    task->tEdgeX = Q_10_6(0);
    task->tEdgeY = Q_10_6(0);
    task->tPivotY = Q_10_6(0);
    task->tEdgeDX = Q_10_6(0);
    task->tEdgeDY = Q_10_6(0);
    task->tPivotDY = Q_10_6(1);
    task->tEdgeDDX = Q_10_6(0);
    task->tEdgeDDY = Q_10_6(0);
    task->tPivotDDY = Q_10_6(0);
    task->tState++;
    return TRUE;
}

static bool8 StarIris_FirstHalf(struct Task *task)
{
    StarIris_UpdateSpeedsAccelerationsAndScanlineEffects(task);

    if (task->tPivotY > Q_10_6(FIRST_HALF_HEIGHT))
    {
        task->tEdgeDX = Q_10_6(3);
        task->tEdgeDY = Q_10_6(2);
        task->tState++;
    }

    return FALSE;
}

static bool8 StarIris_SecondHalf(struct Task *task)
{
    StarIris_UpdateSpeedsAccelerationsAndScanlineEffects(task);

    if (task->tPivotY > Q_10_6(DISPLAY_HEIGHT / 2))
    {
        DestroyTask(FindTaskIdByFunc(Task_StarIris));
    }

    return FALSE;
}

static void StarIris_UpdateSpeedsAccelerationsAndScanlineEffects(struct Task *task)
{
    int row, pivotX, pivotY, edgeX, edgeY;
    u16 left;

    task->tEdgeDX += task->tEdgeDDX;
    task->tEdgeDY += task->tEdgeDDY;
    task->tPivotDY += task->tPivotDDY;
    task->tEdgeX += task->tEdgeDX;
    task->tEdgeY += task->tEdgeDY;
    task->tPivotY += task->tPivotDY;

    pivotY = Q_10_6_TO_INT(task->tPivotY);
    pivotX = pivotY + (pivotY / 2);
    edgeX = Q_10_6_TO_INT(task->tEdgeX);
    edgeY = Q_10_6_TO_INT(task->tEdgeY);

    sTransitionData->VBlank_DMA = FALSE;

    for (row = 0; row < edgeY; row++)
    {
        gScanlineEffectRegBuffers[0][row] = 0;
        gScanlineEffectRegBuffers[0][DISPLAY_HEIGHT - 1 - row] = 0;
    }
    for (; row < pivotY; row++)
    {
        left = (row - edgeY) * (pivotX - (DISPLAY_WIDTH / 2)) / (pivotY - edgeY) + (DISPLAY_WIDTH / 2);
        StarIris_SetScanlineEffectReflectionalSymmetryQuadrants(row, left);
    }
    for (; row < DISPLAY_HEIGHT / 2; row++)
    {
        left = (pivotX - edgeX) * (row - (DISPLAY_HEIGHT / 2)) / (pivotY - (DISPLAY_HEIGHT / 2)) + edgeX;
        StarIris_SetScanlineEffectReflectionalSymmetryQuadrants(row, left);
    }

    sTransitionData->VBlank_DMA = TRUE;
}

static void StarIris_SetScanlineEffectReflectionalSymmetryQuadrants(int row, int column)
{
    int right;
    u16 drawX;

    right = DISPLAY_WIDTH - column;
    drawX = (column << 8) | right;

    gScanlineEffectRegBuffers[0][row] = drawX;
    gScanlineEffectRegBuffers[0][DISPLAY_HEIGHT - 1 - row] = drawX;
}

static void VBlankCB_StarIris_First(void)
{
    // setting these registers on later VBlanks seems to enable BG1, somehow
    REG_WININ = sTransitionData->WININ;
    REG_WINOUT = sTransitionData->WINOUT;
    REG_WIN0V = sTransitionData->WIN0V;
    SetVBlankCallback(VBlankCB_StarIris);
    VBlankCB_StarIris();
}

static void VBlankCB_StarIris(void)
{
    u16 offset0, offset;

    VBlankCB_BattleTransition();
    if (sTransitionData->VBlank_DMA)
        DmaCopy16(3, gScanlineEffectRegBuffers[0], gScanlineEffectRegBuffers[1], DISPLAY_HEIGHT * sizeof(u16));

    REG_WIN0H = gScanlineEffectRegBuffers[1][0];
}

static void HBlankCB_StarIris(void)
{
    if (REG_VCOUNT < DISPLAY_HEIGHT)
    {
        REG_WIN0H = gScanlineEffectRegBuffers[1][REG_VCOUNT];
    }
}
