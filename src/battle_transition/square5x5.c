#include "global.h"
#include "battle_transition.h"
#include "battle_transition_shared.h"
#include "battle_transition_yoshord.h"
#include "gpu_regs.h"
#include "main.h"
#include "scanline_effect.h"
#include "task.h"

enum square5x5_transition_variant
{
    VARIANT_CHECKER_DOUBLE_CLOCK,
    VARIANT_LAYERED_QUARTERCLOCK,
};

enum
{
    EFFECTREG_OFFSET_WIN0H,
    EFFECTREG_OFFSET_WIN1H,
    EFFECTREG_OFFSET_COUNT
};
#define EFFECTREG_MAXCOPY (EFFECTREG_OFFSET_COUNT * 5)

static const u8 sSquare5X5_Patterns[][5][5] =
{
    [VARIANT_CHECKER_DOUBLE_CLOCK] =
    {
        {13, 1,14, 2,15},
        { 8,21, 9,22, 3},
        {20,12,25,10,16},
        { 7,24,11,23, 4},
        {19, 6,18, 5,17},
    },
    [VARIANT_LAYERED_QUARTERCLOCK] =
    {
        { 3, 2, 1, 2, 3},
        { 4, 7, 6, 7, 4},
        { 5, 8, 9, 8, 5},
        { 4, 7, 6, 7, 4},
        { 3, 2, 1, 2, 3},
    },
};

static bool8 Square5X5_Init(struct Task *);
static bool8 Square5X5_Main(struct Task *);
static bool8 Square5X5_End(struct Task *);
static void VBlankCB_Square5X5(void);
static void HBlankCB_Square5X5(void);

static const TransitionStateFunc sSquare5X5_Funcs[] = {
    Square5X5_Init,
    Square5X5_Main,
    Square5X5_End,
};

#define tStep data[1]
#define tTimer data[14]
#define tVariant data[15]

void Task_Square5X5CheckerDoubleClock(u8 taskId)
{
    gTasks[taskId].tVariant = VARIANT_CHECKER_DOUBLE_CLOCK;
    while (sSquare5X5_Funcs[gTasks[taskId].tState](&gTasks[taskId]));
}

void Task_Square5X5LayeredQuarterClock(u8 taskId)
{
    gTasks[taskId].tVariant = VARIANT_LAYERED_QUARTERCLOCK;
    while (sSquare5X5_Funcs[gTasks[taskId].tState](&gTasks[taskId]));
}

static bool8 Square5X5_Init(struct Task *task)
{
    u32 segment;
    InitTransitionData();
    ScanlineEffect_Clear();

    for (segment = 0; segment < 5; segment++)
    {
        gScanlineEffectRegBuffers[1][EFFECTREG_OFFSET_WIN0H + segment * EFFECTREG_OFFSET_COUNT] = DISPLAY_WIDTH;
        gScanlineEffectRegBuffers[1][EFFECTREG_OFFSET_WIN1H + segment * EFFECTREG_OFFSET_COUNT] = DISPLAY_WIDTH;
    }
    SetVBlankCallback(VBlankCB_Square5X5);
    SetHBlankCallback(HBlankCB_Square5X5);
    EnableInterrupts(INTR_FLAG_VBLANK | INTR_FLAG_HBLANK);

    task->tTimer = 1;
    task->tState++;
    return FALSE;
}

static bool8 Square5X5_Main(struct Task *task)
{
    u32 y, x;
    bool32 isDone = TRUE;

    if (0 == --task->tTimer)
    {
        ++task->tStep;
        sTransitionData->VBlank_DMA = FALSE;

        for (y = 0; y < 5; ++y)
        {
            // One line type, the `10101` type, requires a `WINxH` to wrap around the screen.
            // `didOverflow` is set to `TRUE` if this line type is encountered.
            // so that the closing of the open third segment can be skipped

            bool32 didOverflow = FALSE;
            u32 nextRegPart = 0;
            u32 regParts32 = 0;
            u16* const regParts16 = (u16 *) &regParts32;
            u8* const regParts8 = (u8 *) &regParts32;

            for (x = 0; x < 5; ++x)
            {
                bool32 segmentIsVisible = sSquare5X5_Patterns[task->tVariant][y][x] > task->tStep;

                if (segmentIsVisible && nextRegPart % 2 == 0)
                {
                    regParts8[nextRegPart + 1] = (DISPLAY_WIDTH / 5) * x;
                    ++nextRegPart;
                }
                else if (!segmentIsVisible && nextRegPart % 2 != 0)
                {
                    regParts8[nextRegPart - 1] = (DISPLAY_WIDTH / 5) * x;
                    ++nextRegPart;
                    if (4 == nextRegPart)
                    {
                        nextRegPart = 0;
                        didOverflow = TRUE;
                    }
                }
            }

            if (!didOverflow && nextRegPart % 2 != 0)
            {
                regParts8[nextRegPart - 1] = DISPLAY_WIDTH;
            }

            isDone = isDone && regParts16[0] == 0;

            gScanlineEffectRegBuffers[0][EFFECTREG_OFFSET_WIN0H + y * EFFECTREG_OFFSET_COUNT] = regParts16[0];
            gScanlineEffectRegBuffers[0][EFFECTREG_OFFSET_WIN1H + y * EFFECTREG_OFFSET_COUNT] = regParts16[1];
        }
        sTransitionData->VBlank_DMA = TRUE;

        task->tTimer = 4;

        if (TRUE == isDone)
        {
            task->tState++;
        }
    }

    return FALSE;
}

static bool8 Square5X5_End(struct Task *task)
{
    FadeScreenBlack();
    DestroyTask(FindTaskIdByFunc(task->func));
    return FALSE;
}

static void VBlankCB_Square5X5(void)
{
    VBlankCB_BattleTransition();

    if (sTransitionData->VBlank_DMA)
    {
        DmaCopy16(3, gScanlineEffectRegBuffers[0], gScanlineEffectRegBuffers[1], EFFECTREG_MAXCOPY * sizeof(u16));
    }

    REG_WIN0H = gScanlineEffectRegBuffers[1][EFFECTREG_OFFSET_WIN0H];
    REG_WIN1H = gScanlineEffectRegBuffers[1][EFFECTREG_OFFSET_WIN1H];
    REG_WIN0V = WIN_RANGE(0, 255);
    REG_WIN1V = WIN_RANGE(0, 255);
    REG_WININ = WININ_WIN0_ALL | WININ_WIN1_ALL;
    REG_WINOUT = 0;
}

static void HBlankCB_Square5X5(void)
{
    if (REG_VCOUNT < DISPLAY_HEIGHT)
    if (0 == REG_VCOUNT % (DISPLAY_HEIGHT / 5))
    {
        u32 segment = REG_VCOUNT / (DISPLAY_HEIGHT / 5);

        REG_WIN0H = gScanlineEffectRegBuffers[1][EFFECTREG_OFFSET_WIN0H + segment * EFFECTREG_OFFSET_COUNT];
        REG_WIN1H = gScanlineEffectRegBuffers[1][EFFECTREG_OFFSET_WIN1H + segment * EFFECTREG_OFFSET_COUNT];
    }
}
