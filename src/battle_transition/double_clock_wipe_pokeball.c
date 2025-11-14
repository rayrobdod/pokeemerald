#include "global.h"
#include "battle_transition.h"
#include "battle_transition_shared.h"
#include "battle_transition_tasks/double_clock_wipe_pokeball.h"
#include "gba/syscall.h"
#include "decompress.h"
#include "gpu_regs.h"
#include "main.h"
#include "scanline_effect.h"
#include "task.h"
#include "trig.h"

#define EFFECTREG_WIN0H_OFFSET (0)
#define EFFECTREG_WIN1H_OFFSET (DISPLAY_HEIGHT + EFFECTREG_WIN0H_OFFSET)
#define EFFECTREG_MAXCOPY (DISPLAY_HEIGHT + EFFECTREG_WIN1H_OFFSET)

#define EFFECTREG_OUTER_EDGE (EFFECTREG_MAXCOPY)
#define EFFECTREG_INNER_EDGE (DISPLAY_HEIGHT / 2 + EFFECTREG_OUTER_EDGE)
#define EFFECTREG_BUTTON (DISPLAY_HEIGHT / 2 + EFFECTREG_INNER_EDGE)

#define RADIUS_OUTER_EDGE (76)
#define RADIUS_INNER_EDGE (64)
#define RADIUS_BUTTON (24)
#define HALFWIDTH_BELT (4)

// Special cases for horizontal frames means assume this is a multiple of 2
#define FRAMES_PER_ROTATION (44)

#define tAngle data[2]
#define SPRITE_ID_OFFSET (7)

#define TILETAG_POKEBALL_FRAGMENT_MASK (0xFA57)

static const u32 sSprite_Mask[] = INCBIN_U32("graphics/battle_transitions/double_clock_wipe_pokeball_mask.4bpp.lz");

static const struct CompressedSpriteSheet sSpriteSheet_Mask =
{
    .data = sSprite_Mask,
    .size = 8 * 4 * 4 * 4 * 4,
    .tag = TILETAG_POKEBALL_FRAGMENT_MASK,
};

static const struct OamData sOamData_Mask =
{
    .y = 0,
    .affineMode = ST_OAM_AFFINE_OFF,
    .objMode = ST_OAM_OBJ_WINDOW,
    .mosaic = FALSE,
    .bpp = ST_OAM_4BPP,
    .shape = SPRITE_SHAPE(32x32),
    .x = 0,
    .matrixNum = 0,
    .size = SPRITE_SIZE(32x32),
    .tileNum = 0,
    .priority = 1,
    .paletteNum = 0,
    .affineParam = 0,
};

static const union AnimCmd sAnim_Mask_0[] =
{
    ANIMCMD_FRAME(0, 1),
    ANIMCMD_END,
};

static const union AnimCmd sAnim_Mask_1[] =
{
    ANIMCMD_FRAME(16, 1),
    ANIMCMD_END,
};

static const union AnimCmd sAnim_Mask_2[] =
{
    ANIMCMD_FRAME(32, 1),
    ANIMCMD_END,
};

static const union AnimCmd sAnim_Mask_3[] =
{
    ANIMCMD_FRAME(48, 1),
    ANIMCMD_END,
};

static const union AnimCmd *const sAnimTable_Mask[] =
{
    sAnim_Mask_0,
    sAnim_Mask_1,
    sAnim_Mask_2,
    sAnim_Mask_3,
};

static const struct SpriteTemplate sSpriteTemplate_Mask =
{
    .tileTag = TILETAG_POKEBALL_FRAGMENT_MASK,
    .paletteTag = TAG_NONE,
    .oam = &sOamData_Mask,
    .anims = sAnimTable_Mask,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCallbackDummy,
};

static bool8 DoubleClockWipePokeball_Init(struct Task *);
static bool8 DoubleClockWipePokeball_NorthEast1(struct Task *);
static bool8 DoubleClockWipePokeball_East1(struct Task *);
static bool8 DoubleClockWipePokeball_SouthEast1(struct Task *);
static bool8 DoubleClockWipePokeball_South1(struct Task *);
static bool8 DoubleClockWipePokeball_SouthWest1(struct Task *);
static bool8 DoubleClockWipePokeball_West1(struct Task *);
static bool8 DoubleClockWipePokeball_NorthWest1(struct Task *);
static bool8 DoubleClockWipePokeball_North1(struct Task *);
static bool8 DoubleClockWipePokeball_NorthEast2(struct Task *);
static bool8 DoubleClockWipePokeball_East2(struct Task *);
static bool8 DoubleClockWipePokeball_SouthEast2(struct Task *);
static bool8 DoubleClockWipePokeball_South2(struct Task *);
static bool8 DoubleClockWipePokeball_SouthWest2(struct Task *);
static bool8 DoubleClockWipePokeball_West2(struct Task *);
static bool8 DoubleClockWipePokeball_NorthWest2(struct Task *);
static bool8 DoubleClockWipePokeball_End(struct Task *);

static const TransitionStateFunc sDoubleClockWipePokeball_Funcs[] = {
    DoubleClockWipePokeball_Init,
    DoubleClockWipePokeball_NorthEast1,
    DoubleClockWipePokeball_East1,
    DoubleClockWipePokeball_SouthEast1,
    DoubleClockWipePokeball_South1,
    DoubleClockWipePokeball_SouthWest1,
    DoubleClockWipePokeball_West1,
    DoubleClockWipePokeball_NorthWest1,
    DoubleClockWipePokeball_North1,
    DoubleClockWipePokeball_NorthEast2,
    DoubleClockWipePokeball_East2,
    DoubleClockWipePokeball_SouthEast2,
    DoubleClockWipePokeball_South2,
    DoubleClockWipePokeball_SouthWest2,
    DoubleClockWipePokeball_West2,
    DoubleClockWipePokeball_NorthWest2,
    DoubleClockWipePokeball_End,
};

void Task_DoubleClockWipePokeball(u8 taskId)
{
    while (sDoubleClockWipePokeball_Funcs[gTasks[taskId].tState](&gTasks[taskId]));
}

static void VBlankCB_DoubleClockWipe(void)
{
    VBlankCB_BattleTransition();

    if (sTransitionData->VBlank_DMA)
    {
        DmaCopy16(3, gScanlineEffectRegBuffers[0], gScanlineEffectRegBuffers[1], EFFECTREG_MAXCOPY * 2);
    }

    REG_WIN0H = gScanlineEffectRegBuffers[1][EFFECTREG_WIN0H_OFFSET];
    REG_WIN1H = gScanlineEffectRegBuffers[1][EFFECTREG_WIN1H_OFFSET];
}

static void HBlankCB_DoubleClockWipe(void)
{
    if (REG_VCOUNT < DISPLAY_HEIGHT)
    {
        REG_WIN0H = gScanlineEffectRegBuffers[1][REG_VCOUNT + EFFECTREG_WIN0H_OFFSET];
        REG_WIN1H = gScanlineEffectRegBuffers[1][REG_VCOUNT + EFFECTREG_WIN1H_OFFSET];
    }
}

static inline s32 Tan(s16 index)
{
    s16 sin, cos;
    s32 retval;

    cos = gSineTable[index + 64];
    if (0 == cos) {
        return (DISPLAY_WIDTH / 2) << 8;
    }
    sin = gSineTable[index];

    retval = (sin << 8) / cos;
    retval = min((DISPLAY_WIDTH / 2) << 8, retval);
    retval = max(-(DISPLAY_WIDTH / 2) << 8, retval);
    return retval;
}

static inline s32 MultiplyBoundedToHalfwidth(u32 y, s32 cotan)
{
    s32 retval;
    retval = y * cotan;
    retval = retval >> 8;
    retval = min((DISPLAY_WIDTH / 2), retval);
    retval = max(-(DISPLAY_WIDTH / 2), retval);
    return retval;
}

inline static unsigned CircleXAt(unsigned radius, unsigned y)
{
    return (y > radius ? 0 : Sqrt(radius * radius - y * y));
}

static bool8 DoubleClockWipePokeball_Init(struct Task *task)
{
    unsigned y, y_from_center;

    InitTransitionData();
    ScanlineEffect_Clear();

    for (y = 0, y_from_center = DISPLAY_HEIGHT / 2 - 1; y < DISPLAY_HEIGHT / 2; y++, y_from_center--)
    {
        gScanlineEffectRegBuffers[0][EFFECTREG_OUTER_EDGE + y] = CircleXAt(RADIUS_OUTER_EDGE, y_from_center);
        gScanlineEffectRegBuffers[0][EFFECTREG_INNER_EDGE + y] = CircleXAt(RADIUS_INNER_EDGE, y_from_center);
        gScanlineEffectRegBuffers[0][EFFECTREG_BUTTON + y] = CircleXAt(RADIUS_BUTTON, y_from_center);
    }

    sTransitionData->VBlank_DMA = FALSE;
    for (y = 0; y < DISPLAY_HEIGHT / 2 - HALFWIDTH_BELT; y++)
    {
        gScanlineEffectRegBuffers[0][EFFECTREG_WIN0H_OFFSET + y] = DISPLAY_WIDTH;
        gScanlineEffectRegBuffers[1][EFFECTREG_WIN0H_OFFSET + y] = DISPLAY_WIDTH;
        gScanlineEffectRegBuffers[0][EFFECTREG_WIN1H_OFFSET + y] = WIN_RANGE(
            DISPLAY_WIDTH / 2 + gScanlineEffectRegBuffers[0][EFFECTREG_INNER_EDGE + y],
            DISPLAY_WIDTH / 2 + gScanlineEffectRegBuffers[0][EFFECTREG_OUTER_EDGE + y]);
    }
    for (; y < DISPLAY_HEIGHT / 2; y++)
    {
        gScanlineEffectRegBuffers[0][EFFECTREG_WIN0H_OFFSET + y] = DISPLAY_WIDTH;
        gScanlineEffectRegBuffers[1][EFFECTREG_WIN0H_OFFSET + y] = DISPLAY_WIDTH;
        gScanlineEffectRegBuffers[0][EFFECTREG_WIN1H_OFFSET + y] = WIN_RANGE(
            DISPLAY_WIDTH / 2,
            DISPLAY_WIDTH / 2 + gScanlineEffectRegBuffers[0][EFFECTREG_OUTER_EDGE + y]);
    }
    for (; y < DISPLAY_HEIGHT; y++)
    {
        gScanlineEffectRegBuffers[0][EFFECTREG_WIN0H_OFFSET + y] = DISPLAY_WIDTH;
        gScanlineEffectRegBuffers[1][EFFECTREG_WIN0H_OFFSET + y] = DISPLAY_WIDTH;
    }

    REG_WININ = WININ_WIN0_ALL | WININ_WIN1_ALL;
    REG_WINOUT = WINOUT_WINOBJ_ALL;
    REG_WIN0V = DISPLAY_HEIGHT;
    REG_WIN1V = DISPLAY_HEIGHT;
    SetGpuRegBits(REG_OFFSET_DISPCNT, DISPCNT_OBJWIN_ON);

    SetVBlankCallback(VBlankCB_DoubleClockWipe);
    SetHBlankCallback(HBlankCB_DoubleClockWipe);
    EnableInterrupts(INTR_FLAG_VBLANK | INTR_FLAG_HBLANK);

    task->tAngle = 1;
    task->tState++;
    return TRUE;
}

static bool8 DoubleClockWipePokeball_NorthEast1(struct Task *task)
{
    s32 x1, x2;
    u32 y;
    const s32 tan = Tan(task->tAngle * 256 / FRAMES_PER_ROTATION);

    sTransitionData->VBlank_DMA = FALSE;
    for (y = 0; y < DISPLAY_HEIGHT / 2; y++)
    {
        x1 = MultiplyBoundedToHalfwidth(DISPLAY_HEIGHT / 2 - y, tan);
        x2 = gScanlineEffectRegBuffers[0][EFFECTREG_BUTTON + y];

        gScanlineEffectRegBuffers[0][EFFECTREG_WIN0H_OFFSET + y] = (x1 > x2 ?
            WIN_RANGE(DISPLAY_WIDTH / 2 + x1, DISPLAY_WIDTH / 2 + x2) :
            WIN_RANGE(0, DISPLAY_WIDTH));
    }
    sTransitionData->VBlank_DMA = TRUE;

    if (task->tAngle == 2)
    {
        LoadCompressedSpriteSheet(&sSpriteSheet_Mask);
    }

    task->tAngle++;
    if (task->tAngle > FRAMES_PER_ROTATION / 4)
        task->tState++;
    return FALSE;
}

static bool8 DoubleClockWipePokeball_East1(struct Task *task)
{
    u32 y;

    sTransitionData->VBlank_DMA = FALSE;
    for (y = 0; y < DISPLAY_HEIGHT / 2 - RADIUS_BUTTON; y++)
    {
        gScanlineEffectRegBuffers[0][EFFECTREG_WIN0H_OFFSET + y] = WIN_RANGE(0, DISPLAY_WIDTH / 2);
    }
    for (; y < DISPLAY_HEIGHT / 2 - HALFWIDTH_BELT; y++)
    {
        gScanlineEffectRegBuffers[0][EFFECTREG_WIN0H_OFFSET + y] =
                WIN_RANGE(0, DISPLAY_WIDTH / 2 + gScanlineEffectRegBuffers[0][EFFECTREG_BUTTON + y]);
    }
    for (; y < DISPLAY_HEIGHT / 2; y++)
    {
        gScanlineEffectRegBuffers[0][EFFECTREG_WIN0H_OFFSET + y] = WIN_RANGE(0, DISPLAY_WIDTH / 2);
    }
    for (; y < DISPLAY_HEIGHT / 2 + HALFWIDTH_BELT; y++)
    {
        gScanlineEffectRegBuffers[0][EFFECTREG_WIN1H_OFFSET + y] = WIN_RANGE(
            DISPLAY_WIDTH / 2 - gScanlineEffectRegBuffers[0][EFFECTREG_OUTER_EDGE + DISPLAY_HEIGHT - y - 1],
            DISPLAY_WIDTH / 2 + gScanlineEffectRegBuffers[0][EFFECTREG_OUTER_EDGE + DISPLAY_HEIGHT - y - 1]);
    }
    for (; y < DISPLAY_HEIGHT; y++)
    {
        gScanlineEffectRegBuffers[0][EFFECTREG_WIN1H_OFFSET + y] = WIN_RANGE(
            DISPLAY_WIDTH / 2 + gScanlineEffectRegBuffers[0][EFFECTREG_INNER_EDGE + DISPLAY_HEIGHT - y - 1],
            DISPLAY_WIDTH / 2 + gScanlineEffectRegBuffers[0][EFFECTREG_OUTER_EDGE + DISPLAY_HEIGHT - y - 1]);
    }
    task->tState++;
    return TRUE;
}

static bool8 DoubleClockWipePokeball_SouthEast1(struct Task *task)
{
    s32 x1, x2;
    u32 y;
    const s32 tan = Tan(task->tAngle * 256 / FRAMES_PER_ROTATION);

    sTransitionData->VBlank_DMA = FALSE;
    for (y = DISPLAY_HEIGHT / 2; y < DISPLAY_HEIGHT; y++)
    {
        x1 = MultiplyBoundedToHalfwidth(DISPLAY_HEIGHT / 2 - y, tan);
        x2 = gScanlineEffectRegBuffers[0][EFFECTREG_BUTTON + DISPLAY_HEIGHT - y - 1];

        gScanlineEffectRegBuffers[0][EFFECTREG_WIN0H_OFFSET + y] =
            WIN_RANGE(0, DISPLAY_WIDTH / 2 + max(x1, x2));
    }
    sTransitionData->VBlank_DMA = TRUE;

    task->tAngle++;
    if (task->tAngle > FRAMES_PER_ROTATION * 2 / 4 - 1)
        task->tState++;
    return FALSE;
}

static bool8 DoubleClockWipePokeball_South1(struct Task *task)
{
    unsigned i;
    u32 y;

    sTransitionData->VBlank_DMA = FALSE;
    for (y = 0; y < DISPLAY_HEIGHT; y++)
    {
        gScanlineEffectRegBuffers[0][EFFECTREG_WIN0H_OFFSET + y] = WIN_RANGE(0, DISPLAY_WIDTH / 2);
    }

    for (y = 0; y < DISPLAY_HEIGHT / 2 - RADIUS_OUTER_EDGE; y++)
    {
        gScanlineEffectRegBuffers[0][EFFECTREG_WIN1H_OFFSET + y] = WIN_RANGE(0, 0);
    }
    for (; y < DISPLAY_HEIGHT / 2 - RADIUS_INNER_EDGE; y++)
    {
        gScanlineEffectRegBuffers[0][EFFECTREG_WIN1H_OFFSET + y] = WIN_RANGE(
            DISPLAY_WIDTH / 2 - gScanlineEffectRegBuffers[0][EFFECTREG_OUTER_EDGE + y],
            DISPLAY_WIDTH / 2 + gScanlineEffectRegBuffers[0][EFFECTREG_OUTER_EDGE + y]);
    }
    for (; y < DISPLAY_HEIGHT / 2 - RADIUS_BUTTON; y++)
    {
        gScanlineEffectRegBuffers[0][EFFECTREG_WIN1H_OFFSET + y] = WIN_RANGE(
            DISPLAY_WIDTH / 2 + gScanlineEffectRegBuffers[0][EFFECTREG_INNER_EDGE + y],
            DISPLAY_WIDTH / 2 + gScanlineEffectRegBuffers[0][EFFECTREG_OUTER_EDGE + y]);
    }
    for (; y < DISPLAY_HEIGHT / 2 - HALFWIDTH_BELT; y++)
    {
        gScanlineEffectRegBuffers[0][EFFECTREG_WIN0H_OFFSET + y] = WIN_RANGE(0,
            DISPLAY_WIDTH / 2 + gScanlineEffectRegBuffers[0][EFFECTREG_BUTTON + y]);
        gScanlineEffectRegBuffers[0][EFFECTREG_WIN1H_OFFSET + y] = WIN_RANGE(
            DISPLAY_WIDTH / 2 + gScanlineEffectRegBuffers[0][EFFECTREG_INNER_EDGE + y],
            DISPLAY_WIDTH / 2 + gScanlineEffectRegBuffers[0][EFFECTREG_OUTER_EDGE + y]);
    }
    for (; y < DISPLAY_HEIGHT / 2; y++)
    {
        gScanlineEffectRegBuffers[0][EFFECTREG_WIN1H_OFFSET + y] = WIN_RANGE(
            DISPLAY_WIDTH / 2 - gScanlineEffectRegBuffers[0][EFFECTREG_OUTER_EDGE + y],
            DISPLAY_WIDTH / 2 + gScanlineEffectRegBuffers[0][EFFECTREG_OUTER_EDGE + y]);
    }
    for (; y < DISPLAY_HEIGHT / 2 + HALFWIDTH_BELT; y++)
    {
        gScanlineEffectRegBuffers[0][EFFECTREG_WIN1H_OFFSET + y] = WIN_RANGE(
            DISPLAY_WIDTH / 2 - gScanlineEffectRegBuffers[0][EFFECTREG_OUTER_EDGE + DISPLAY_HEIGHT - y - 1],
            DISPLAY_WIDTH / 2 + gScanlineEffectRegBuffers[0][EFFECTREG_OUTER_EDGE + DISPLAY_HEIGHT - y - 1]);
    }
    for (; y < DISPLAY_HEIGHT / 2 + RADIUS_BUTTON; y++)
    {
        gScanlineEffectRegBuffers[0][EFFECTREG_WIN1H_OFFSET + y] = WIN_RANGE(
            DISPLAY_WIDTH / 2 - gScanlineEffectRegBuffers[0][EFFECTREG_BUTTON + DISPLAY_HEIGHT - y - 1],
            DISPLAY_WIDTH / 2 + gScanlineEffectRegBuffers[0][EFFECTREG_BUTTON + DISPLAY_HEIGHT - y - 1]);
    }
    for (; y < DISPLAY_HEIGHT / 2 + RADIUS_INNER_EDGE; y++)
    {
        gScanlineEffectRegBuffers[0][EFFECTREG_WIN1H_OFFSET + y] = WIN_RANGE(
            DISPLAY_WIDTH / 2 + gScanlineEffectRegBuffers[0][EFFECTREG_INNER_EDGE + DISPLAY_HEIGHT - y - 1],
            DISPLAY_WIDTH / 2 + gScanlineEffectRegBuffers[0][EFFECTREG_OUTER_EDGE + DISPLAY_HEIGHT - y - 1]);
    }
    for (; y < DISPLAY_HEIGHT / 2 + RADIUS_OUTER_EDGE + 2; y++)
    {
        gScanlineEffectRegBuffers[0][EFFECTREG_WIN1H_OFFSET + y] = WIN_RANGE(
            DISPLAY_WIDTH / 2 - gScanlineEffectRegBuffers[0][EFFECTREG_OUTER_EDGE + DISPLAY_HEIGHT - y - 1],
            DISPLAY_WIDTH / 2 + gScanlineEffectRegBuffers[0][EFFECTREG_OUTER_EDGE + DISPLAY_HEIGHT - y - 1]);
    }
    for (; y < DISPLAY_HEIGHT; y++)
    {
        gScanlineEffectRegBuffers[0][EFFECTREG_WIN1H_OFFSET + y] = WIN_RANGE(0, 0);
    }
    sTransitionData->VBlank_DMA = TRUE;

    task->data[SPRITE_ID_OFFSET + 0] = CreateSprite(&sSpriteTemplate_Mask, 168 + 16, DISPLAY_HEIGHT + 2 - 48 - 16, 0);
    task->data[SPRITE_ID_OFFSET + 1] = CreateSprite(&sSpriteTemplate_Mask, 160 + 16, DISPLAY_HEIGHT + 2 - 16 - 16, 0);
    task->data[SPRITE_ID_OFFSET + 2] = CreateSprite(&sSpriteTemplate_Mask, 128 + 16, DISPLAY_HEIGHT + 2 - 16 - 16, 0);
    for (i = 0; i < 3; i++)
        if (MAX_SPRITES != task->data[SPRITE_ID_OFFSET + i])
        {
            StartSpriteAnim(&gSprites[task->data[SPRITE_ID_OFFSET + i]], i);
            gSprites[task->data[SPRITE_ID_OFFSET + i]].vFlip = TRUE;
        }

    task->data[SPRITE_ID_OFFSET + 3] = CreateSprite(&sSpriteTemplate_Mask, DISPLAY_WIDTH - 168 - 16, DISPLAY_HEIGHT + 2 - 48 - 16, 0);
    task->data[SPRITE_ID_OFFSET + 4] = CreateSprite(&sSpriteTemplate_Mask, DISPLAY_WIDTH - 160 - 16, DISPLAY_HEIGHT + 2 - 16 - 16, 0);
    task->data[SPRITE_ID_OFFSET + 5] = CreateSprite(&sSpriteTemplate_Mask, DISPLAY_WIDTH - 128 - 16, DISPLAY_HEIGHT + 2 - 16 - 16, 0);
    for (i = 3; i < 6; i++)
        if (MAX_SPRITES != task->data[SPRITE_ID_OFFSET + i])
        {
            StartSpriteAnim(&gSprites[task->data[SPRITE_ID_OFFSET + i]], i - 3);
            gSprites[task->data[SPRITE_ID_OFFSET + i]].hFlip = TRUE;
            gSprites[task->data[SPRITE_ID_OFFSET + i]].vFlip = TRUE;
        }

    task->tState++;
    task->tAngle++;
    return FALSE;
}

static bool8 DoubleClockWipePokeball_SouthWest1(struct Task *task)
{
    s32 x;
    u32 y;
    const s32 tan = Tan(task->tAngle * 256 / FRAMES_PER_ROTATION);

    sTransitionData->VBlank_DMA = FALSE;
    for (y = DISPLAY_HEIGHT / 2; y < DISPLAY_HEIGHT; y++)
    {
        x = MultiplyBoundedToHalfwidth(DISPLAY_HEIGHT / 2 - y, tan);

        gScanlineEffectRegBuffers[0][EFFECTREG_WIN0H_OFFSET + y] =
            WIN_RANGE(0, DISPLAY_WIDTH / 2 + x);
    }
    sTransitionData->VBlank_DMA = TRUE;

    task->tAngle++;
    if (task->tAngle > FRAMES_PER_ROTATION * 3 / 4)
        task->tState++;
    return FALSE;
}

static bool8 DoubleClockWipePokeball_West1(struct Task *task)
{
    unsigned i;
    u32 y;

    sTransitionData->VBlank_DMA = FALSE;
    for (y = DISPLAY_HEIGHT / 2 - RADIUS_BUTTON; y < DISPLAY_HEIGHT / 2 - HALFWIDTH_BELT; y++)
    {
        gScanlineEffectRegBuffers[0][EFFECTREG_WIN1H_OFFSET + y] = WIN_RANGE(
            DISPLAY_WIDTH / 2 - gScanlineEffectRegBuffers[0][EFFECTREG_BUTTON + y],
            DISPLAY_WIDTH / 2 + gScanlineEffectRegBuffers[0][EFFECTREG_BUTTON + y]);
    }

    for (y = DISPLAY_HEIGHT / 2; y < DISPLAY_HEIGHT / 2 + HALFWIDTH_BELT; y++)
    {
        gScanlineEffectRegBuffers[0][EFFECTREG_WIN0H_OFFSET + y] = WIN_RANGE(
            DISPLAY_WIDTH / 2 - gScanlineEffectRegBuffers[0][EFFECTREG_OUTER_EDGE + DISPLAY_HEIGHT - y - 1],
            DISPLAY_WIDTH / 2);
        gScanlineEffectRegBuffers[0][EFFECTREG_WIN1H_OFFSET + y] = WIN_RANGE(
            DISPLAY_WIDTH / 2,
            DISPLAY_WIDTH / 2 + gScanlineEffectRegBuffers[0][EFFECTREG_OUTER_EDGE + DISPLAY_HEIGHT - y - 1]);
    }
    for (; y < DISPLAY_HEIGHT / 2 + RADIUS_BUTTON; y++)
    {
        gScanlineEffectRegBuffers[0][EFFECTREG_WIN0H_OFFSET + y] = WIN_RANGE(
            DISPLAY_WIDTH / 2 + gScanlineEffectRegBuffers[0][EFFECTREG_INNER_EDGE + DISPLAY_HEIGHT - y - 1],
            DISPLAY_WIDTH / 2 + gScanlineEffectRegBuffers[0][EFFECTREG_OUTER_EDGE + DISPLAY_HEIGHT - y - 1]);
        gScanlineEffectRegBuffers[0][EFFECTREG_WIN1H_OFFSET + y] = WIN_RANGE(
            DISPLAY_WIDTH / 2 - gScanlineEffectRegBuffers[0][EFFECTREG_BUTTON + DISPLAY_HEIGHT - y - 1],
            DISPLAY_WIDTH / 2 + gScanlineEffectRegBuffers[0][EFFECTREG_BUTTON + DISPLAY_HEIGHT - y - 1]);
    }
    for (; y < DISPLAY_HEIGHT / 2 + RADIUS_INNER_EDGE; y++)
    {
        gScanlineEffectRegBuffers[0][EFFECTREG_WIN0H_OFFSET + y] = WIN_RANGE(
            DISPLAY_WIDTH / 2 - gScanlineEffectRegBuffers[0][EFFECTREG_OUTER_EDGE + DISPLAY_HEIGHT - y - 1],
            DISPLAY_WIDTH / 2 - gScanlineEffectRegBuffers[0][EFFECTREG_INNER_EDGE + DISPLAY_HEIGHT - y - 1]);
        gScanlineEffectRegBuffers[0][EFFECTREG_WIN1H_OFFSET + y] = WIN_RANGE(
            DISPLAY_WIDTH / 2 + gScanlineEffectRegBuffers[0][EFFECTREG_INNER_EDGE + DISPLAY_HEIGHT - y - 1],
            DISPLAY_WIDTH / 2 + gScanlineEffectRegBuffers[0][EFFECTREG_OUTER_EDGE + DISPLAY_HEIGHT - y - 1]);
    }
    for (; y < DISPLAY_HEIGHT / 2 + RADIUS_OUTER_EDGE + 2; y++)
    {
        gScanlineEffectRegBuffers[0][EFFECTREG_WIN0H_OFFSET + y] = WIN_RANGE(
            DISPLAY_WIDTH / 2 - gScanlineEffectRegBuffers[0][EFFECTREG_OUTER_EDGE + DISPLAY_HEIGHT - y - 1],
            DISPLAY_WIDTH / 2);
        gScanlineEffectRegBuffers[0][EFFECTREG_WIN1H_OFFSET + y] = WIN_RANGE(
            DISPLAY_WIDTH / 2,
            DISPLAY_WIDTH / 2 + gScanlineEffectRegBuffers[0][EFFECTREG_OUTER_EDGE + DISPLAY_HEIGHT - y - 1]);
    }
    for (; y < DISPLAY_HEIGHT; y++)
    {
        gScanlineEffectRegBuffers[0][EFFECTREG_WIN0H_OFFSET + y] = WIN_RANGE(0, 0);
        gScanlineEffectRegBuffers[0][EFFECTREG_WIN1H_OFFSET + y] = WIN_RANGE(0, 0);
    }

    task->data[SPRITE_ID_OFFSET + 6] = CreateSprite(&sSpriteTemplate_Mask, DISPLAY_WIDTH - 168 - 16, 48 + 16, 0);
    task->data[SPRITE_ID_OFFSET + 7] = CreateSprite(&sSpriteTemplate_Mask, DISPLAY_WIDTH - 160 - 16, 16 + 16, 0);
    task->data[SPRITE_ID_OFFSET + 8] = CreateSprite(&sSpriteTemplate_Mask, DISPLAY_WIDTH - 128 - 16, 16 + 16, 0);
    for (i = 6; i < 9; i++)
        if (MAX_SPRITES != task->data[SPRITE_ID_OFFSET + i])
        {
            StartSpriteAnim(&gSprites[task->data[SPRITE_ID_OFFSET + i]], i - 6);
            gSprites[task->data[SPRITE_ID_OFFSET + i]].hFlip = TRUE;
            gSprites[task->data[SPRITE_ID_OFFSET + i]].vFlip = FALSE;
        }

    for (i = 0; i < 3; i++)
        if (MAX_SPRITES != task->data[SPRITE_ID_OFFSET + i])
        {
            StartSpriteAnim(&gSprites[task->data[SPRITE_ID_OFFSET + i]], i);
            gSprites[task->data[SPRITE_ID_OFFSET + i]].y = DISPLAY_HEIGHT + 2 - gSprites[task->data[SPRITE_ID_OFFSET + i]].y;
            gSprites[task->data[SPRITE_ID_OFFSET + i]].vFlip = FALSE;
        }

    task->tState++;
    return TRUE;
}

static bool8 DoubleClockWipePokeball_NorthWest1(struct Task *task)
{
    s32 x;
    u32 y;
    const s32 tan = Tan(task->tAngle * 256 / FRAMES_PER_ROTATION);

    sTransitionData->VBlank_DMA = FALSE;
    for (y = 0; y < DISPLAY_HEIGHT / 2; y++)
    {
        x = MultiplyBoundedToHalfwidth(DISPLAY_HEIGHT / 2 - y, tan);

        gScanlineEffectRegBuffers[0][EFFECTREG_WIN0H_OFFSET + y] =
            WIN_RANGE(DISPLAY_WIDTH / 2 + x, DISPLAY_WIDTH / 2);
    }
    sTransitionData->VBlank_DMA = TRUE;

    task->tAngle++;
    if (task->tAngle > FRAMES_PER_ROTATION - 1)
        task->tState++;
    return FALSE;
}

static bool8 DoubleClockWipePokeball_North1(struct Task *task)
{
    unsigned i;
    u32 y;

    sTransitionData->VBlank_DMA = FALSE;
    for (y = 0; y < DISPLAY_HEIGHT / 2 - RADIUS_OUTER_EDGE; y++)
    {
        gScanlineEffectRegBuffers[0][EFFECTREG_WIN0H_OFFSET + y] = WIN_RANGE(0, 0);
        gScanlineEffectRegBuffers[0][EFFECTREG_WIN1H_OFFSET + y] = WIN_RANGE(0, 0);
    }
    for (; y < DISPLAY_HEIGHT / 2 - RADIUS_INNER_EDGE; y++)
    {
        gScanlineEffectRegBuffers[0][EFFECTREG_WIN0H_OFFSET + y] = WIN_RANGE(
            DISPLAY_WIDTH / 2 - gScanlineEffectRegBuffers[0][EFFECTREG_OUTER_EDGE + y],
            DISPLAY_WIDTH / 2);
        gScanlineEffectRegBuffers[0][EFFECTREG_WIN1H_OFFSET + y] = WIN_RANGE(
            DISPLAY_WIDTH / 2,
            DISPLAY_WIDTH / 2 + gScanlineEffectRegBuffers[0][EFFECTREG_OUTER_EDGE + y]);
    }
    for (; y < DISPLAY_HEIGHT / 2 - RADIUS_BUTTON; y++)
    {
        gScanlineEffectRegBuffers[0][EFFECTREG_WIN0H_OFFSET + y] = WIN_RANGE(
            DISPLAY_WIDTH / 2 - gScanlineEffectRegBuffers[0][EFFECTREG_OUTER_EDGE + y],
            DISPLAY_WIDTH / 2 - gScanlineEffectRegBuffers[0][EFFECTREG_INNER_EDGE + y]);
        gScanlineEffectRegBuffers[0][EFFECTREG_WIN1H_OFFSET + y] = WIN_RANGE(
            DISPLAY_WIDTH / 2 + gScanlineEffectRegBuffers[0][EFFECTREG_INNER_EDGE + y],
            DISPLAY_WIDTH / 2 + gScanlineEffectRegBuffers[0][EFFECTREG_OUTER_EDGE + y]);
    }
    for (; y < DISPLAY_HEIGHT / 2 - HALFWIDTH_BELT; y++)
    {
        gScanlineEffectRegBuffers[0][EFFECTREG_WIN0H_OFFSET + y] = WIN_RANGE(
            DISPLAY_WIDTH / 2,
            DISPLAY_WIDTH / 2 + gScanlineEffectRegBuffers[0][EFFECTREG_BUTTON + y]);
        gScanlineEffectRegBuffers[0][EFFECTREG_WIN1H_OFFSET + y] = WIN_RANGE(
            DISPLAY_WIDTH / 2 + gScanlineEffectRegBuffers[0][EFFECTREG_INNER_EDGE + y],
            DISPLAY_WIDTH / 2 + gScanlineEffectRegBuffers[0][EFFECTREG_OUTER_EDGE + y]);
    }
    for (; y < DISPLAY_HEIGHT / 2; y++)
    {
        gScanlineEffectRegBuffers[0][EFFECTREG_WIN0H_OFFSET + y] = WIN_RANGE(
            DISPLAY_WIDTH / 2 - gScanlineEffectRegBuffers[0][EFFECTREG_OUTER_EDGE + y],
            DISPLAY_WIDTH / 2);
        gScanlineEffectRegBuffers[0][EFFECTREG_WIN1H_OFFSET + y] = WIN_RANGE(
            DISPLAY_WIDTH / 2,
            DISPLAY_WIDTH / 2 + gScanlineEffectRegBuffers[0][EFFECTREG_OUTER_EDGE + y]);
    }
    sTransitionData->VBlank_DMA = TRUE;

    for (i = 1; i < 3; i++)
    {
        if (MAX_SPRITES != task->data[SPRITE_ID_OFFSET + i])
            DestroySprite(&gSprites[task->data[SPRITE_ID_OFFSET + i]]);
        task->data[SPRITE_ID_OFFSET + i] = MAX_SPRITES;
    }
    if (MAX_SPRITES != task->data[SPRITE_ID_OFFSET])
    {
        StartSpriteAnim(&gSprites[task->data[SPRITE_ID_OFFSET]], 3);
        gSprites[task->data[SPRITE_ID_OFFSET]].x = 120 - 16;
        gSprites[task->data[SPRITE_ID_OFFSET]].y = 80 - 16 + 1;
        gSprites[task->data[SPRITE_ID_OFFSET]].vFlip = FALSE;
        gSprites[task->data[SPRITE_ID_OFFSET]].hFlip = FALSE;
    }

    task->tState++;
    task->tAngle -= FRAMES_PER_ROTATION;
    task->tAngle++;
    return FALSE;
}

static bool8 DoubleClockWipePokeball_NorthEast2(struct Task *task)
{
    s32 x;
    u32 y;
    const s32 tan = Tan(task->tAngle * 256 / FRAMES_PER_ROTATION);

    sTransitionData->VBlank_DMA = FALSE;
    for (y = DISPLAY_HEIGHT / 2 - RADIUS_OUTER_EDGE; y < DISPLAY_HEIGHT / 2; y++)
    {
        x = MultiplyBoundedToHalfwidth(DISPLAY_HEIGHT / 2 - y, tan);

        if (y < DISPLAY_HEIGHT / 2 - RADIUS_INNER_EDGE || y >= DISPLAY_HEIGHT / 2 - HALFWIDTH_BELT)
        {
            gScanlineEffectRegBuffers[0][EFFECTREG_WIN1H_OFFSET + y] =
                (x > gScanlineEffectRegBuffers[0][EFFECTREG_OUTER_EDGE + y] ?
                    WIN_RANGE(0, 0) :
                    WIN_RANGE(
                        DISPLAY_WIDTH / 2 + x,
                        DISPLAY_WIDTH / 2 + gScanlineEffectRegBuffers[0][EFFECTREG_OUTER_EDGE + y])
                );
        }
        else
        {
            if (x < gScanlineEffectRegBuffers[0][EFFECTREG_INNER_EDGE + y])
                gScanlineEffectRegBuffers[0][EFFECTREG_WIN1H_OFFSET + y] = WIN_RANGE(
                    DISPLAY_WIDTH / 2 + gScanlineEffectRegBuffers[0][EFFECTREG_INNER_EDGE + y],
                    DISPLAY_WIDTH / 2 + gScanlineEffectRegBuffers[0][EFFECTREG_OUTER_EDGE + y]);
            else if (x < gScanlineEffectRegBuffers[0][EFFECTREG_OUTER_EDGE + y])
                gScanlineEffectRegBuffers[0][EFFECTREG_WIN1H_OFFSET + y] = WIN_RANGE(
                    DISPLAY_WIDTH / 2 + x,
                    DISPLAY_WIDTH / 2 + gScanlineEffectRegBuffers[0][EFFECTREG_OUTER_EDGE + y]);
            else
                gScanlineEffectRegBuffers[0][EFFECTREG_WIN1H_OFFSET + y] = WIN_RANGE(0, 0);

            if (y >= DISPLAY_HEIGHT / 2 - RADIUS_BUTTON)
            {
                if (x < gScanlineEffectRegBuffers[0][EFFECTREG_BUTTON + y])
                    gScanlineEffectRegBuffers[0][EFFECTREG_WIN0H_OFFSET + y] = WIN_RANGE(
                        DISPLAY_WIDTH / 2 + x,
                        DISPLAY_WIDTH / 2 + gScanlineEffectRegBuffers[0][EFFECTREG_BUTTON + y]);
                else
                    gScanlineEffectRegBuffers[0][EFFECTREG_WIN0H_OFFSET + y] = WIN_RANGE(0, 0);
            }
        }
    }
    sTransitionData->VBlank_DMA = TRUE;

    task->tAngle++;
    if (task->tAngle > FRAMES_PER_ROTATION / 4)
        task->tState++;
    return FALSE;
}

static bool8 DoubleClockWipePokeball_East2(struct Task *task)
{
    u32 y;

    sTransitionData->VBlank_DMA = FALSE;
    for (y = DISPLAY_HEIGHT / 2 - RADIUS_BUTTON;
            y < DISPLAY_HEIGHT / 2 - HALFWIDTH_BELT; y++)
    {
        gScanlineEffectRegBuffers[0][EFFECTREG_WIN0H_OFFSET + y] = WIN_RANGE(
            DISPLAY_WIDTH / 2 - gScanlineEffectRegBuffers[0][EFFECTREG_BUTTON + y],
            DISPLAY_WIDTH / 2);
        gScanlineEffectRegBuffers[0][EFFECTREG_WIN1H_OFFSET + y] = WIN_RANGE(
            DISPLAY_WIDTH / 2 - gScanlineEffectRegBuffers[0][EFFECTREG_OUTER_EDGE + y],
            DISPLAY_WIDTH / 2 - gScanlineEffectRegBuffers[0][EFFECTREG_INNER_EDGE + y]);
    }

    if (MAX_SPRITES != task->data[SPRITE_ID_OFFSET])
    {
        StartSpriteAnim(&gSprites[task->data[SPRITE_ID_OFFSET]], 3);
        gSprites[task->data[SPRITE_ID_OFFSET]].x = 120 - 16;
        gSprites[task->data[SPRITE_ID_OFFSET]].y = 80 + 16 + 1;
        gSprites[task->data[SPRITE_ID_OFFSET]].vFlip = TRUE;
        gSprites[task->data[SPRITE_ID_OFFSET]].hFlip = FALSE;
    }

    task->tState++;
    return TRUE;
}

static bool8 DoubleClockWipePokeball_SouthEast2(struct Task *task)
{
    s32 x;
    u32 y;
    const s32 tan = Tan(task->tAngle * 256 / FRAMES_PER_ROTATION);

    sTransitionData->VBlank_DMA = FALSE;
    for (y = DISPLAY_HEIGHT / 2; y < DISPLAY_HEIGHT / 2 + RADIUS_OUTER_EDGE; y++)
    {
        x = MultiplyBoundedToHalfwidth(DISPLAY_HEIGHT / 2 - y, tan);

        if (y < DISPLAY_HEIGHT / 2 + HALFWIDTH_BELT || y >= DISPLAY_HEIGHT / 2 + RADIUS_INNER_EDGE)
        {
            gScanlineEffectRegBuffers[0][EFFECTREG_WIN1H_OFFSET + y] =
                WIN_RANGE(
                    DISPLAY_WIDTH / 2,
                    DISPLAY_WIDTH / 2 + min(x, gScanlineEffectRegBuffers[0][EFFECTREG_OUTER_EDGE + DISPLAY_HEIGHT - y - 1])
                );
        }
        else
        {
            if (x > gScanlineEffectRegBuffers[0][EFFECTREG_OUTER_EDGE + DISPLAY_HEIGHT - y - 1])
                gScanlineEffectRegBuffers[0][EFFECTREG_WIN1H_OFFSET + y] = WIN_RANGE(
                    DISPLAY_WIDTH / 2 + gScanlineEffectRegBuffers[0][EFFECTREG_INNER_EDGE + DISPLAY_HEIGHT - y - 1],
                    DISPLAY_WIDTH / 2 + gScanlineEffectRegBuffers[0][EFFECTREG_OUTER_EDGE + DISPLAY_HEIGHT - y - 1]);
            else if (x > gScanlineEffectRegBuffers[0][EFFECTREG_INNER_EDGE + DISPLAY_HEIGHT - y - 1])
                gScanlineEffectRegBuffers[0][EFFECTREG_WIN1H_OFFSET + y] = WIN_RANGE(
                    DISPLAY_WIDTH / 2 + gScanlineEffectRegBuffers[0][EFFECTREG_INNER_EDGE + DISPLAY_HEIGHT - y - 1],
                    DISPLAY_WIDTH / 2 + x);
            else
                gScanlineEffectRegBuffers[0][EFFECTREG_WIN1H_OFFSET + y] = WIN_RANGE(0, 0);

            if (y < DISPLAY_HEIGHT / 2 + RADIUS_BUTTON)
                gScanlineEffectRegBuffers[0][EFFECTREG_WIN0H_OFFSET + y] =
                    WIN_RANGE(
                        DISPLAY_WIDTH / 2,
                        DISPLAY_WIDTH / 2 + min(x, gScanlineEffectRegBuffers[0][EFFECTREG_BUTTON + DISPLAY_HEIGHT - y - 1])
                    );
        }
    }
    sTransitionData->VBlank_DMA = TRUE;

    task->tAngle++;
    if (task->tAngle > FRAMES_PER_ROTATION * 2 / 4)
        task->tState++;
    return FALSE;
}

static bool8 DoubleClockWipePokeball_South2(struct Task *task)
{
    unsigned i;

    for (i = 3; i < 9; i++)
    {
        if (MAX_SPRITES != task->data[SPRITE_ID_OFFSET + i])
            DestroySprite(&gSprites[task->data[SPRITE_ID_OFFSET + i]]);
        task->data[SPRITE_ID_OFFSET + i] = MAX_SPRITES;
    }
    if (MAX_SPRITES != task->data[SPRITE_ID_OFFSET + 0])
        DestroySprite(&gSprites[task->data[SPRITE_ID_OFFSET + 0]]);
    task->data[SPRITE_ID_OFFSET + 0] = MAX_SPRITES;

    task->tState++;
    return TRUE;
}

static bool8 DoubleClockWipePokeball_SouthWest2(struct Task *task)
{
    s32 x;
    u32 y;
    const s32 tan = Tan(task->tAngle * 256 / FRAMES_PER_ROTATION);

    sTransitionData->VBlank_DMA = FALSE;
    for (y = DISPLAY_HEIGHT / 2; y < DISPLAY_HEIGHT / 2 + RADIUS_OUTER_EDGE; y++)
    {
        x = MultiplyBoundedToHalfwidth(DISPLAY_HEIGHT / 2 - y, tan);

        if (y < DISPLAY_HEIGHT / 2 + HALFWIDTH_BELT || y >= DISPLAY_HEIGHT / 2 + RADIUS_INNER_EDGE)
        {
            gScanlineEffectRegBuffers[0][EFFECTREG_WIN0H_OFFSET + y] =
                WIN_RANGE(
                    DISPLAY_WIDTH / 2 - gScanlineEffectRegBuffers[0][EFFECTREG_OUTER_EDGE + DISPLAY_HEIGHT - y - 1],
                    DISPLAY_WIDTH / 2 + max(x, -gScanlineEffectRegBuffers[0][EFFECTREG_OUTER_EDGE + DISPLAY_HEIGHT - y - 1])
                );
        }
        else
        {
            if (x < -gScanlineEffectRegBuffers[0][EFFECTREG_OUTER_EDGE + DISPLAY_HEIGHT - y - 1])
                gScanlineEffectRegBuffers[0][EFFECTREG_WIN0H_OFFSET + y] = WIN_RANGE(0, 0);
            else if (x < -gScanlineEffectRegBuffers[0][EFFECTREG_INNER_EDGE + DISPLAY_HEIGHT - y - 1])
                gScanlineEffectRegBuffers[0][EFFECTREG_WIN0H_OFFSET + y] = WIN_RANGE(
                    DISPLAY_WIDTH / 2 - gScanlineEffectRegBuffers[0][EFFECTREG_OUTER_EDGE + DISPLAY_HEIGHT - y - 1],
                    DISPLAY_WIDTH / 2 + x);
            else
                gScanlineEffectRegBuffers[0][EFFECTREG_WIN0H_OFFSET + y] = WIN_RANGE(
                    DISPLAY_WIDTH / 2 - gScanlineEffectRegBuffers[0][EFFECTREG_OUTER_EDGE + DISPLAY_HEIGHT - y - 1],
                    DISPLAY_WIDTH / 2 - gScanlineEffectRegBuffers[0][EFFECTREG_INNER_EDGE + DISPLAY_HEIGHT - y - 1]);

            if (y < DISPLAY_HEIGHT / 2 + RADIUS_BUTTON)
                gScanlineEffectRegBuffers[0][EFFECTREG_WIN1H_OFFSET + y] =
                    WIN_RANGE(
                        DISPLAY_WIDTH / 2 - gScanlineEffectRegBuffers[0][EFFECTREG_BUTTON + DISPLAY_HEIGHT - y - 1],
                        DISPLAY_WIDTH / 2 + max(x, -gScanlineEffectRegBuffers[0][EFFECTREG_BUTTON + DISPLAY_HEIGHT - y - 1])
                    );
        }
    }
    sTransitionData->VBlank_DMA = TRUE;

    task->tAngle++;
    if (task->tAngle > FRAMES_PER_ROTATION * 3 / 4)
        task->tState++;
    return FALSE;
}

static bool8 DoubleClockWipePokeball_West2(struct Task *task)
{
    u32 y;

    sTransitionData->VBlank_DMA = FALSE;
    for (y = DISPLAY_HEIGHT / 2; y < DISPLAY_HEIGHT; y++)
    {
        gScanlineEffectRegBuffers[0][EFFECTREG_WIN0H_OFFSET + y] = WIN_RANGE(0, 0);
        gScanlineEffectRegBuffers[0][EFFECTREG_WIN1H_OFFSET + y] = WIN_RANGE(0, 0);
    }

    task->tState++;
    return TRUE;
}

static bool8 DoubleClockWipePokeball_NorthWest2(struct Task *task)
{
    s32 x;
    u32 y;
    const s32 tan = Tan(task->tAngle * 256 / FRAMES_PER_ROTATION);

    sTransitionData->VBlank_DMA = FALSE;
    for (y = DISPLAY_HEIGHT / 2 - RADIUS_OUTER_EDGE; y < DISPLAY_HEIGHT / 2; y++)
    {
        x = MultiplyBoundedToHalfwidth(DISPLAY_HEIGHT / 2 - y, tan);

        if (y < DISPLAY_HEIGHT / 2 - RADIUS_INNER_EDGE || y >= DISPLAY_HEIGHT / 2 - HALFWIDTH_BELT)
        {
            gScanlineEffectRegBuffers[0][EFFECTREG_WIN0H_OFFSET + y] =
                WIN_RANGE(
                    DISPLAY_WIDTH / 2 + max(x, -gScanlineEffectRegBuffers[0][EFFECTREG_OUTER_EDGE + y]),
                    DISPLAY_WIDTH / 2
                );
        }
        else
        {
            if (x > -gScanlineEffectRegBuffers[0][EFFECTREG_INNER_EDGE + y])
                gScanlineEffectRegBuffers[0][EFFECTREG_WIN0H_OFFSET + y] = WIN_RANGE(0, 0);
            else if (x > -gScanlineEffectRegBuffers[0][EFFECTREG_OUTER_EDGE + y])
                gScanlineEffectRegBuffers[0][EFFECTREG_WIN0H_OFFSET + y] = WIN_RANGE(
                    DISPLAY_WIDTH / 2 + x,
                    DISPLAY_WIDTH / 2 - gScanlineEffectRegBuffers[0][EFFECTREG_INNER_EDGE + y]);
            else
                gScanlineEffectRegBuffers[0][EFFECTREG_WIN0H_OFFSET + y] = WIN_RANGE(
                    DISPLAY_WIDTH / 2 - gScanlineEffectRegBuffers[0][EFFECTREG_OUTER_EDGE + y],
                    DISPLAY_WIDTH / 2 - gScanlineEffectRegBuffers[0][EFFECTREG_INNER_EDGE + y]);

            if (y >= DISPLAY_HEIGHT / 2 - RADIUS_BUTTON)
                gScanlineEffectRegBuffers[0][EFFECTREG_WIN1H_OFFSET + y] =
                    WIN_RANGE(
                        DISPLAY_WIDTH / 2 + max(x, -gScanlineEffectRegBuffers[0][EFFECTREG_BUTTON + y]),
                        DISPLAY_WIDTH / 2
                    );
        }
    }
    sTransitionData->VBlank_DMA = TRUE;

    task->tAngle++;
    if (task->tAngle > FRAMES_PER_ROTATION - 1)
        task->tState++;
    return FALSE;
}

static bool8 DoubleClockWipePokeball_End(struct Task *task)
{
    FreeSpriteTilesByTag(TILETAG_POKEBALL_FRAGMENT_MASK);
    FadeScreenBlack();
    DestroyTask(FindTaskIdByFunc(task->func));
    return FALSE;
}
