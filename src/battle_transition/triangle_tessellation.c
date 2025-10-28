#include "global.h"
#include "battle_transition.h"
#include "battle_transition_shared.h"
#include "battle_transition_tasks/triangle_tessellation.h"
#include "gpu_regs.h"
#include "main.h"
#include "scanline_effect.h"
#include "task.h"

#define EFFECTREG_WIN0H_OFFSET (0)
#define EFFECTREG_WIN1H_OFFSET (DISPLAY_HEIGHT + EFFECTREG_WIN0H_OFFSET)
#define EFFECTREG_MAX (DISPLAY_HEIGHT + EFFECTREG_WIN1H_OFFSET)

#define TRIANGLE_HEIGHT (40)
#define TRIANGLE_WIDTH (46)

#define tCycle data[1]

static bool8 TriangleTessellation_Init(struct Task *);
static bool8 TriangleTessellationCentered_DrawTriangle(struct Task *task);
static bool8 TriangleTessellationCentered_Darken(struct Task *);
static bool8 TriangleTessellationZARoyale_DrawTriangle(struct Task *task);
static bool8 TriangleTessellationZARoyale_Darken(struct Task *);
static bool8 TriangleTessellation_End(struct Task *);

static const TransitionStateFunc sTriangleTessellationCentered_Funcs[] = {
    TriangleTessellation_Init,
    TriangleTessellationCentered_DrawTriangle,
    TriangleTessellationCentered_Darken,
    TriangleTessellation_End,
};

void Task_TriangleTessellationCentered(u8 taskId)
{
    while (sTriangleTessellationCentered_Funcs[gTasks[taskId].tState](&gTasks[taskId]));
}

static const TransitionStateFunc sTriangleTessellationZARoyale_Funcs[] = {
    TriangleTessellation_Init,
    TriangleTessellationZARoyale_DrawTriangle,
    TriangleTessellationZARoyale_Darken,
    TriangleTessellation_End,
};

void Task_TriangleTessellationZARoyale(u8 taskId)
{
    while (sTriangleTessellationZARoyale_Funcs[gTasks[taskId].tState](&gTasks[taskId]));
}

static void VBlankCB_TriangleTessellation(void)
{
    VBlankCB_BattleTransition();

    REG_BLDY = sTransitionData->BLDY;

    if (sTransitionData->VBlank_DMA)
    {
        DmaCopy16(3, gScanlineEffectRegBuffers[0], gScanlineEffectRegBuffers[1], EFFECTREG_MAX * 2);
    }

    REG_WIN0H = gScanlineEffectRegBuffers[1][EFFECTREG_WIN0H_OFFSET];
    REG_WIN1H = gScanlineEffectRegBuffers[1][EFFECTREG_WIN1H_OFFSET];
}

static void HBlankCB_TriangleTessellation(void)
{
    if (REG_VCOUNT < DISPLAY_HEIGHT)
    {
        REG_WIN0H = gScanlineEffectRegBuffers[1][REG_VCOUNT + EFFECTREG_WIN0H_OFFSET];
        REG_WIN1H = gScanlineEffectRegBuffers[1][REG_VCOUNT + EFFECTREG_WIN1H_OFFSET];
    }
}

static bool8 TriangleTessellation_Init(struct Task *task)
{
    InitTransitionData();
    ScanlineEffect_Clear();

    task->tCycle = 0;

    REG_WININ = WININ_WIN1_ALL;
    REG_WINOUT = WINOUT_WIN01_BG_ALL | WINOUT_WIN01_OBJ;
    REG_WIN0V = DISPLAY_HEIGHT;
    REG_WIN1V = DISPLAY_HEIGHT;
    REG_BLDCNT = BLDCNT_TGT1_ALL | BLDCNT_EFFECT_DARKEN;
    sTransitionData->BLDY = 0;

    SetVBlankCallback(VBlankCB_TriangleTessellation);
    SetHBlankCallback(HBlankCB_TriangleTessellation);
    EnableInterrupts(INTR_FLAG_VBLANK | INTR_FLAG_HBLANK);

    task->tState++;
    return TRUE;
}

static signed TriangleTessellation_offh(signed column, unsigned y_in_row, bool32 flip)
{
    signed offh;

    if (column < 0)
        return 0;

    if (flip)
        y_in_row = TRIANGLE_HEIGHT - y_in_row;

    offh = y_in_row * TRIANGLE_WIDTH / 2 / TRIANGLE_HEIGHT + TRIANGLE_WIDTH / 2 * column;

    if (offh > DISPLAY_WIDTH / 2)
        offh = DISPLAY_WIDTH / 2;

    return offh;
}

static bool8 TriangleTessellationCentered_DrawTriangle(struct Task *task)
{
    unsigned y;
    signed off0h, off1h;
    signed column0, column1;

    signed row = 0;
    unsigned y_in_row = 20;

    sTransitionData->VBlank_DMA = FALSE;
    for (y = 0; y < DISPLAY_HEIGHT; y++)
    {
        y_in_row++;
        if (y_in_row >= TRIANGLE_HEIGHT)
        {
            y_in_row = 0;
            row++;
        }
        if (y_in_row == 0 || y == 0)
        {
            column0 = task->tCycle - abs(2 - row) - 1;
            column1 = column0 + 1 + (task->tCycle == 7 ? 1 : 0);
        }

        off0h = TriangleTessellation_offh(column0, y_in_row, (column0 + row) % 2);
        gScanlineEffectRegBuffers[0][y + EFFECTREG_WIN0H_OFFSET] = WIN_RANGE(DISPLAY_WIDTH / 2 - off0h, DISPLAY_WIDTH / 2 + off0h);

        off1h = TriangleTessellation_offh(column1, y_in_row, (column1 + row) % 2);
        gScanlineEffectRegBuffers[0][y + EFFECTREG_WIN1H_OFFSET] = WIN_RANGE(DISPLAY_WIDTH / 2 - off1h, DISPLAY_WIDTH / 2 + off1h);
    }
    sTransitionData->BLDY = 1;
    sTransitionData->VBlank_DMA = TRUE;
    task->tState++;
    return FALSE;
}

static bool8 TriangleTessellationZARoyale_DrawTriangle(struct Task *task)
{
    unsigned y;
    signed off0h, off1h;
    signed column0, column1;

    signed row = 0;
    unsigned y_in_row = 0;

    sTransitionData->VBlank_DMA = FALSE;
    for (y = 0; y < DISPLAY_HEIGHT; y++)
    {
        y_in_row++;
        if (y_in_row >= TRIANGLE_HEIGHT)
        {
            y_in_row = 0;
            row++;
        }
        if (y_in_row == 0 || y == 0)
        {
            if (row < 3)
            {
                if (task->tCycle < 4)
                    column0 = task->tCycle - abs(2 - row) - 1;
                else
                    column0 = 4 + (task->tCycle - 4) * 2 - abs(2 - row) - 1;

                if (task->tCycle < 4)
                    column1 = column0 + 1;
                else if (task->tCycle < 5)
                    column1 = column0 + 2;
                else
                    column1 = column0 + 3;
            }
            else
            {
                column0 = (task->tCycle - 2) * 2;
                column1 = column0 + 2;
            }
        }

        off0h = TriangleTessellation_offh(column0, y_in_row, (column0 + row) % 2);
        gScanlineEffectRegBuffers[0][y + EFFECTREG_WIN0H_OFFSET] = WIN_RANGE(DISPLAY_WIDTH / 2 - off0h, DISPLAY_WIDTH / 2 + off0h);

        off1h = TriangleTessellation_offh(column1, y_in_row, (column1 + row) % 2);
        gScanlineEffectRegBuffers[0][y + EFFECTREG_WIN1H_OFFSET] = WIN_RANGE(DISPLAY_WIDTH / 2 - off1h, DISPLAY_WIDTH / 2 + off1h);
    }
    sTransitionData->BLDY = 1;
    sTransitionData->VBlank_DMA = TRUE;
    task->tState++;
    return FALSE;
}

static bool8 TriangleTessellationCentered_Darken(struct Task *task)
{
    if (sTransitionData->BLDY < 16)
    {
        sTransitionData->BLDY += 2;
        return FALSE;
    }
    else if (task->tCycle > 6)
    {
        task->tState++;
        return TRUE;
    }
    else
    {
        task->tCycle++;
        task->tState--;
        return TRUE;
    }
}

static bool8 TriangleTessellationZARoyale_Darken(struct Task *task)
{
    if (sTransitionData->BLDY < 16)
    {
        sTransitionData->BLDY += 1;
        return FALSE;
    }
    else if (task->tCycle > 4)
    {
        task->tState++;
        return TRUE;
    }
    else
    {
        task->tCycle++;
        task->tState--;
        return TRUE;
    }
}

static bool8 TriangleTessellation_End(struct Task *task)
{
    FadeScreenBlack();
    DestroyTask(FindTaskIdByFunc(task->func));
    return FALSE;
}
