#include "global.h"
#include "battle_transition.h"
#include "battle_transition_shared.h"
#include "battle_transition_tasks/water_dpp.h"
#include "gpu_regs.h"
#include "main.h"
#include "scanline_effect.h"
#include "task.h"
#include "trig.h"

static bool8 WaterDpp_Init(struct Task *);
static bool8 WaterDpp_Main(struct Task *);
static void HBlankCB_WaterDpp(void);
static void VBlankCB_WaterDpp(void);

#define WIN_REG_BUFFER_OFFSET (DISPLAY_HEIGHT)

#define tTimer data[2]
#define tOffset data[3]
#define tPeriod data[4]
#define tAmplitude data[5]
#define tShutter data[6]

static const TransitionStateFunc sWaterDpp_Funcs[] = {
    WaterDpp_Init,
    WaterDpp_Main,
};

void Task_WaterDpp(u8 taskId)
{
    while (sWaterDpp_Funcs[gTasks[taskId].tState](&gTasks[taskId]));
}

static bool8 WaterDpp_Init(struct Task *task)
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

    SetVBlankCallback(VBlankCB_WaterDpp);
    SetHBlankCallback(HBlankCB_WaterDpp);

    EnableInterrupts(INTR_FLAG_HBLANK);

    task->tAmplitude = 0;
    task->tOffset = 0;
    task->tPeriod = 60;
    task->tState++;
    return TRUE;
}

static bool8 WaterDpp_Main(struct Task *task)
{
    int i;
    int index_line_part;
    int index_const_part;

    task->tTimer += 1;
    task->tOffset += 1;

    if (task->tAmplitude < 16)
        task->tAmplitude += 1;

    index_line_part = 256 / task->tPeriod;
    index_const_part = task->tOffset * index_line_part;

    sTransitionData->VBlank_DMA = FALSE;
    for (i = 0; i < DISPLAY_HEIGHT; i++)
    {
        int index = (index_const_part + i * index_line_part) % 256;
        int sin = Sin(index, task->tAmplitude);

        gScanlineEffectRegBuffers[0][i] = -sin;
        gScanlineEffectRegBuffers[0][WIN_REG_BUFFER_OFFSET + i] = (
            sin < 0 ?
                WIN_RANGE(0, DISPLAY_WIDTH + sin) :
                WIN_RANGE(sin, DISPLAY_WIDTH));
    }

    if (task->tTimer > 66)
    {
        task->tShutter += 2;

        for (i = 0; i < task->tShutter; i++)
        {
            gScanlineEffectRegBuffers[0][WIN_REG_BUFFER_OFFSET + i] = 0;
            gScanlineEffectRegBuffers[0][WIN_REG_BUFFER_OFFSET + i + 53] = 0;
            gScanlineEffectRegBuffers[0][WIN_REG_BUFFER_OFFSET + i + 107] = 0;
        }

        if (task->tShutter > 53)
        {
            SetVBlankCallback(NULL);
            SetHBlankCallback(NULL);
            DestroyTask(FindTaskIdByFunc(Task_WaterDpp));
        }
    }

    sTransitionData->VBlank_DMA = TRUE;
    return FALSE;
}

static void VBlankCB_WaterDpp(void)
{
    u16 offset0, offset;

    VBlankCB_BattleTransition();
    if (sTransitionData->VBlank_DMA)
        DmaCopy16(3, gScanlineEffectRegBuffers[0], gScanlineEffectRegBuffers[1], (WIN_REG_BUFFER_OFFSET + DISPLAY_HEIGHT) * 2);

    REG_WININ = sTransitionData->WININ;
    REG_WINOUT = sTransitionData->WINOUT;
    REG_WIN0V = sTransitionData->WIN0V;

    offset0 = gScanlineEffectRegBuffers[1][0];
    offset = offset0 + sTransitionData->cameraX;
    REG_BG0HOFS = offset0;
    REG_BG1HOFS = offset;
    REG_BG2HOFS = offset;
    REG_BG3HOFS = offset;
    REG_WIN0H = gScanlineEffectRegBuffers[1][WIN_REG_BUFFER_OFFSET + 0];
}

static void HBlankCB_WaterDpp(void)
{
    if (REG_VCOUNT < DISPLAY_HEIGHT)
    {
        u16 offset0 = gScanlineEffectRegBuffers[1][REG_VCOUNT];
        u16 offset = offset0 + sTransitionData->cameraX;
        REG_BG0HOFS = offset0;
        REG_BG1HOFS = offset;
        REG_BG2HOFS = offset;
        REG_BG3HOFS = offset;
        REG_WIN0H = gScanlineEffectRegBuffers[1][WIN_REG_BUFFER_OFFSET + REG_VCOUNT];
    }
}
