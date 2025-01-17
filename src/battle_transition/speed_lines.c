#include "global.h"
#include "battle_transition.h"
#include "battle_transition_shared.h"
#include "battle_transition_tasks/speed_lines.h"
#include "gpu_regs.h"
#include "palette.h"
#include "random.h"
#include "scanline_effect.h"
#include "task.h"
#include "constants/rgb.h"

static const u16 sSpeedLines_Palette[] = INCBIN_U16("graphics/battle_transitions/speedlines_bg.gbapal");
static const u32 sSpeedLines_Bg_Tileset[] = INCBIN_U32("graphics/battle_transitions/speedlines_bg.4bpp.lz");
static const u32 sSpeedLines_Bg_Small_Tileset[] = INCBIN_U32("graphics/battle_transitions/speedlines_bg_small.4bpp.lz");

static bool8 SpeedLines_Init(struct Task *);
static bool8 SpeedLines_ConstantVelocityUntilTimerZero(struct Task *);
static bool8 SpeedLines_ConstantVelocityUntilFadeComplete(struct Task *);
static bool8 SpeedLines_SwapToFullSize(struct Task *);
static bool8 SpeedLines_Accelerate(struct Task *);
static bool8 SpeedLines_FadeToWhite(struct Task *);
static bool8 SpeedLines_FadeWhiteToBlack(struct Task *);
static bool8 SpeedLines_End(struct Task *);

#define SPEED_LINE_VARIANT_COUNT (6)
static const u16 sSpeedLineVariantsBg[SPEED_LINE_VARIANT_COUNT][4] = {
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {1, BG_TILE_H_FLIP(1), 0, 0},
    {2, 3, BG_TILE_H_FLIP(3), BG_TILE_H_FLIP(2)},
    {4, 5, BG_TILE_H_FLIP(4), 0},
    {6, 7, BG_TILE_H_FLIP(7), BG_TILE_H_FLIP(6)},
};

static const TransitionStateFunc sSpeedLines_Funcs[] = {
    SpeedLines_Init,
    SpeedLines_ConstantVelocityUntilTimerZero,
    SpeedLines_SwapToFullSize,
    SpeedLines_ConstantVelocityUntilTimerZero,
    SpeedLines_Accelerate,
    SpeedLines_FadeToWhite,
    SpeedLines_ConstantVelocityUntilFadeComplete,
    SpeedLines_FadeWhiteToBlack,
    SpeedLines_End,
};

void Task_SpeedLines(u8 taskId)
{
    while (sSpeedLines_Funcs[gTasks[taskId].tState](&gTasks[taskId]));
}

#define Q_12_4(n) ((s16)(n * 16))
#define Q_12_4_TO_INT(n) ((s16)(n / 16))

#define FIXPOINT_SCALE (4)
#define MAX_VELOCITY (8 << FIXPOINT_SCALE)

#define tTimer data[13]
#define tFade data[14]
#define tPosition data[2]
#define tVelocity data[3]
#define tAcceleration data[4]
#define tJerk data[5]

static bool8 SpeedLines_Init(struct Task *task)
{
    u32 i, x, y;
    u32 variant;
    u16 vflip_factor;
    u16 *tilemap, *tileset;

    InitTransitionData();
    ScanlineEffect_Clear();

    GetBg0TilesDst(&tilemap, &tileset);
    LoadPalette(sSpeedLines_Palette, BG_PLTT_ID(15), sizeof(sSpeedLines_Palette));
    LZ77UnCompVram(sSpeedLines_Bg_Small_Tileset, tileset);

    x = 0;
    for (y = 0; y < DISPLAY_TILE_HEIGHT; y++)
    for (i = 0; i < 2; i++)
    {
        x += 4 + (Random() % 8);
        x %= 32;

        variant = Random() % (2 * SPEED_LINE_VARIANT_COUNT);
        if (variant >= SPEED_LINE_VARIANT_COUNT)
        {
            variant -= SPEED_LINE_VARIANT_COUNT;
            vflip_factor = BG_TILE_V_FLIP(0);
        }
        else
        {
            vflip_factor = 0;
        }

        SET_TILE(tilemap, y, x, vflip_factor | sSpeedLineVariantsBg[variant][0]);
        x += 1;
        x %= 32;
        SET_TILE(tilemap, y, x, vflip_factor | sSpeedLineVariantsBg[variant][1]);
        x += 1;
        x %= 32;
        SET_TILE(tilemap, y, x, vflip_factor | sSpeedLineVariantsBg[variant][2]);
        x += 1;
        x %= 32;
        SET_TILE(tilemap, y, x, vflip_factor | sSpeedLineVariantsBg[variant][3]);
    }

    task->tTimer = 8;
    task->tPosition = Q_12_4(0);
    task->tVelocity = Q_12_4(1);
    task->tAcceleration = 3;
    task->tJerk = Q_12_4(0);

    task->tState++;
    return TRUE;
}

static bool8 SpeedLines_SwapToFullSize(struct Task *task)
{
    u16 *tilemap, *tileset;
    GetBg0TilesDst(&tilemap, &tileset);
    LZ77UnCompVram(sSpeedLines_Bg_Tileset, tileset);
    task->tState++;
    task->tTimer = 8;
    return TRUE;
}

static bool8 SpeedLines_ConstantVelocityUntilTimerZero(struct Task *task)
{
    task->tPosition += task->tVelocity;
    SetGpuReg(REG_OFFSET_BG0HOFS, Q_12_4_TO_INT(task->tPosition));

    task->tTimer--;
    if (task->tTimer <= 0) {
        task->tState++;
    }
    return FALSE;
}

static bool8 SpeedLines_Accelerate(struct Task *task)
{
    task->tAcceleration += task->tJerk;
    task->tVelocity += task->tAcceleration;
    task->tPosition += task->tVelocity;
    SetGpuReg(REG_OFFSET_BG0HOFS, Q_12_4_TO_INT(task->tPosition));

    if (task->tVelocity > MAX_VELOCITY) {
        task->tState++;
    }
    return FALSE;
}

static bool8 SpeedLines_FadeToWhite(struct Task *task)
{
    BeginNormalPaletteFade(PALETTES_ALL, 2, 0, 16, RGB_WHITE);
    task->tState++;
    return TRUE;
}

static bool8 SpeedLines_ConstantVelocityUntilFadeComplete(struct Task *task)
{
    task->tPosition += task->tVelocity;
    SetGpuReg(REG_OFFSET_BG0HOFS, Q_12_4_TO_INT(task->tPosition));

    if (!gPaletteFade.active)
    {
        task->tFade = 16;
        task->tState++;
    }
    return FALSE;
}

// A normal palette fade from white to black induced tearing
// i.e. different palette entries were being updated on different frames,
// which was much more noticeable when the screen is supposed to fade from one solid color to a different solid color
static bool8 SpeedLines_FadeWhiteToBlack(struct Task *task)
{
    int color = RGB(task->tFade, task->tFade, task->tFade);

    CpuFill16(color, gPlttBufferFaded, PLTT_BUFFER_SIZE * sizeof(u16));

    task->tFade -= 1;
    if (task->tFade <= 0)
        task->tState++;

    return FALSE;
}

static bool8 SpeedLines_End(struct Task *task)
{
    if (!gPaletteFade.active)
    {
        FadeScreenBlack();
        DestroyTask(FindTaskIdByFunc(task->func));
    }
    return FALSE;
}
