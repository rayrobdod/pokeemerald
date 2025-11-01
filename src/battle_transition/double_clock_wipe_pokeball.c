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

static inline s16 Cotan(s16 index, s16 y)
{
    s16 sin, cos;
    s32 retval;

    sin = gSineTable[index];
    if (0 == sin) {
        return DISPLAY_WIDTH / 2;
    }
    cos = gSineTable[index + 64];

    retval = (y * (cos << 8) / sin) >> 8;
    if (retval > DISPLAY_WIDTH / 2)
        return DISPLAY_WIDTH / 2;
    if (retval < -DISPLAY_WIDTH / 2)
        return -DISPLAY_WIDTH / 2;
    return retval;
}

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

#define FRAMES_PER_ROTATION (34)

#define tAngle data[2]
#define SPRITE_ID_OFFSET (7)

#define TILETAG_POKEBALL_FRAGMENT_MASK (0xFA57)

static const u32 sSprite_Mask[] = INCBIN_U32("graphics/battle_transitions/double_clock_wipe_pokeball_mask.4bpp.lz");

static const struct CompressedSpriteSheet sSpriteSheet_Mask =
{
    .data = sSprite_Mask,
    .size = 8 * 4 * 4 * 4 * 3,
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

static const union AnimCmd *const sAnimTable_Mask[] =
{
    sAnim_Mask_0,
    sAnim_Mask_1,
    sAnim_Mask_2,
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
static bool8 DoubleClockWipePokeball_North1(struct Task *);
static bool8 DoubleClockWipePokeball_NorthWest1(struct Task *);
static bool8 DoubleClockWipePokeball_West1(struct Task *);
static bool8 DoubleClockWipePokeball_South1(struct Task *);
static bool8 DoubleClockWipePokeball_East1(struct Task *);
static bool8 DoubleClockWipePokeball_NorthEast2(struct Task *);
static bool8 DoubleClockWipePokeball_North2(struct Task *);
static bool8 DoubleClockWipePokeball_NorthWest2(struct Task *);
static bool8 DoubleClockWipePokeball_West2(struct Task *);
static bool8 DoubleClockWipePokeball_SouthWest2(struct Task *);
static bool8 DoubleClockWipePokeball_South2(struct Task *);
static bool8 DoubleClockWipePokeball_SouthEast2(struct Task *);
static bool8 DoubleClockWipePokeball_East2(struct Task *);
static bool8 DoubleClockWipePokeball_End(struct Task *);

static const TransitionStateFunc sDoubleClockWipePokeball_Funcs[] = {
    DoubleClockWipePokeball_Init,
    DoubleClockWipePokeball_NorthEast1,
    DoubleClockWipePokeball_North1,
    DoubleClockWipePokeball_NorthWest1,
    DoubleClockWipePokeball_West1,
    DoubleClockWipePokeball_South1,
    DoubleClockWipePokeball_East1,
    DoubleClockWipePokeball_NorthEast2,
    DoubleClockWipePokeball_North2,
    DoubleClockWipePokeball_NorthWest2,
    DoubleClockWipePokeball_West2,
    DoubleClockWipePokeball_SouthWest2,
    DoubleClockWipePokeball_South2,
    DoubleClockWipePokeball_SouthEast2,
    DoubleClockWipePokeball_East2,
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
    unsigned i;
    s32 x;
    u32 y;
    unsigned angle2;

    angle2 = task->tAngle * 256 / FRAMES_PER_ROTATION;

    sTransitionData->VBlank_DMA = FALSE;
    for (y = 0; y < DISPLAY_HEIGHT / 2; y++)
    {
        x = max(
            Cotan(angle2, DISPLAY_HEIGHT / 2 - y),
            gScanlineEffectRegBuffers[0][EFFECTREG_BUTTON + y]);

        gScanlineEffectRegBuffers[0][EFFECTREG_WIN0H_OFFSET + y] = WIN_RANGE(0, DISPLAY_WIDTH / 2 + x);
    }
    sTransitionData->VBlank_DMA = TRUE;

    if (task->tAngle == 2)
    {
        LoadCompressedSpriteSheet(&sSpriteSheet_Mask);
    }
    if (task->tAngle == 3)
    {
        task->data[SPRITE_ID_OFFSET + 0] = CreateSprite(&sSpriteTemplate_Mask, 168 + 16, 48 + 16, 0);
        task->data[SPRITE_ID_OFFSET + 1] = CreateSprite(&sSpriteTemplate_Mask, 160 + 16, 16 + 16, 0);
        task->data[SPRITE_ID_OFFSET + 2] = CreateSprite(&sSpriteTemplate_Mask, 128 + 16, 16 + 16, 0);
        for (i = 0; i < 3; i++)
            if (MAX_SPRITES != task->data[SPRITE_ID_OFFSET + i])
                StartSpriteAnim(&gSprites[task->data[SPRITE_ID_OFFSET + i]], i);
    }
    if (task->tAngle == 4)
    {
        task->data[SPRITE_ID_OFFSET + 3] = CreateSprite(&sSpriteTemplate_Mask, DISPLAY_WIDTH - 168 - 16, 48 + 16, 0);
        task->data[SPRITE_ID_OFFSET + 4] = CreateSprite(&sSpriteTemplate_Mask, DISPLAY_WIDTH - 160 - 16, 16 + 16, 0);
        task->data[SPRITE_ID_OFFSET + 5] = CreateSprite(&sSpriteTemplate_Mask, DISPLAY_WIDTH - 128 - 16, 16 + 16, 0);
        for (i = 3; i < 6; i++)
            if (MAX_SPRITES != task->data[SPRITE_ID_OFFSET + i])
            {
                StartSpriteAnim(&gSprites[task->data[SPRITE_ID_OFFSET + i]], i - 3);
                gSprites[task->data[SPRITE_ID_OFFSET + i]].hFlip = TRUE;
            }
    }

    task->tAngle++;
    if (task->tAngle > FRAMES_PER_ROTATION / 4)
        task->tState++;
    return FALSE;
}

static bool8 DoubleClockWipePokeball_North1(struct Task *task)
{
    u32 y;

    sTransitionData->VBlank_DMA = FALSE;
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
            DISPLAY_WIDTH / 2 - gScanlineEffectRegBuffers[0][EFFECTREG_OUTER_EDGE + y],
            DISPLAY_WIDTH / 2 - gScanlineEffectRegBuffers[0][EFFECTREG_INNER_EDGE + y]);
    }
    for (; y < DISPLAY_HEIGHT / 2 - HALFWIDTH_BELT; y++)
    {
        gScanlineEffectRegBuffers[0][EFFECTREG_WIN1H_OFFSET + y] = WIN_RANGE(
            DISPLAY_WIDTH / 2 - gScanlineEffectRegBuffers[0][EFFECTREG_BUTTON + y],
            DISPLAY_WIDTH / 2 + gScanlineEffectRegBuffers[0][EFFECTREG_BUTTON + y]);
    }
    for (; y < DISPLAY_HEIGHT / 2; y++)
    {
        gScanlineEffectRegBuffers[0][EFFECTREG_WIN1H_OFFSET + y] = WIN_RANGE(
            DISPLAY_WIDTH / 2 - gScanlineEffectRegBuffers[0][EFFECTREG_OUTER_EDGE + y],
            DISPLAY_WIDTH / 2 + gScanlineEffectRegBuffers[0][EFFECTREG_OUTER_EDGE + y]);
    }

    task->tState++;
    return TRUE;
}

static bool8 DoubleClockWipePokeball_NorthWest1(struct Task *task)
{
    unsigned i;
    s32 x;
    u32 y;
    unsigned angle2;

    angle2 = task->tAngle * 256 / FRAMES_PER_ROTATION;

    sTransitionData->VBlank_DMA = FALSE;
    for (y = 0; y < DISPLAY_HEIGHT / 2; y++)
    {
        x = Cotan(angle2, DISPLAY_HEIGHT / 2 - y);
        gScanlineEffectRegBuffers[0][EFFECTREG_WIN0H_OFFSET + y] = WIN_RANGE(0, DISPLAY_WIDTH / 2 + x);
    }
    sTransitionData->VBlank_DMA = TRUE;

    task->tAngle++;
    if (task->tAngle > FRAMES_PER_ROTATION * 2 / 4 - 1)
        task->tState++;
    return FALSE;
}

static bool8 DoubleClockWipePokeball_West1(struct Task *task)
{
    unsigned i;
    u32 y;

    sTransitionData->VBlank_DMA = FALSE;
    for (y = DISPLAY_HEIGHT / 2 - RADIUS_OUTER_EDGE; y < DISPLAY_HEIGHT / 2 - RADIUS_INNER_EDGE; y++)
    {
        gScanlineEffectRegBuffers[0][EFFECTREG_WIN0H_OFFSET + y] = WIN_RANGE(0, 0);
        gScanlineEffectRegBuffers[0][EFFECTREG_WIN1H_OFFSET + y] = WIN_RANGE(
            DISPLAY_WIDTH / 2 - gScanlineEffectRegBuffers[0][EFFECTREG_OUTER_EDGE + y],
            DISPLAY_WIDTH / 2 + gScanlineEffectRegBuffers[0][EFFECTREG_OUTER_EDGE + y]);
    }
    for (; y < DISPLAY_HEIGHT / 2 - RADIUS_BUTTON; y++)
    {
        gScanlineEffectRegBuffers[0][EFFECTREG_WIN0H_OFFSET + y] = WIN_RANGE(
            DISPLAY_WIDTH / 2 + gScanlineEffectRegBuffers[0][EFFECTREG_INNER_EDGE + y],
            DISPLAY_WIDTH / 2 + gScanlineEffectRegBuffers[0][EFFECTREG_OUTER_EDGE + y]);
        gScanlineEffectRegBuffers[0][EFFECTREG_WIN1H_OFFSET + y] = WIN_RANGE(
            DISPLAY_WIDTH / 2 - gScanlineEffectRegBuffers[0][EFFECTREG_OUTER_EDGE + y],
            DISPLAY_WIDTH / 2 - gScanlineEffectRegBuffers[0][EFFECTREG_INNER_EDGE + y]);
    }
    for (; y < DISPLAY_HEIGHT / 2 - HALFWIDTH_BELT; y++)
    {
        gScanlineEffectRegBuffers[0][EFFECTREG_WIN0H_OFFSET + y] = WIN_RANGE(
            DISPLAY_WIDTH / 2 + gScanlineEffectRegBuffers[0][EFFECTREG_INNER_EDGE + y],
            DISPLAY_WIDTH / 2 + gScanlineEffectRegBuffers[0][EFFECTREG_OUTER_EDGE + y]);
        gScanlineEffectRegBuffers[0][EFFECTREG_WIN1H_OFFSET + y] = WIN_RANGE(
            DISPLAY_WIDTH / 2 - gScanlineEffectRegBuffers[0][EFFECTREG_BUTTON + y],
            DISPLAY_WIDTH / 2 + gScanlineEffectRegBuffers[0][EFFECTREG_BUTTON + y]);
    }
    for (; y < DISPLAY_HEIGHT / 2; y++)
    {
        gScanlineEffectRegBuffers[0][EFFECTREG_WIN0H_OFFSET + y] = WIN_RANGE(0, 0);
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
            DISPLAY_WIDTH / 2 - gScanlineEffectRegBuffers[0][EFFECTREG_OUTER_EDGE + DISPLAY_HEIGHT - y - 1],
            DISPLAY_WIDTH / 2 - gScanlineEffectRegBuffers[0][EFFECTREG_INNER_EDGE + DISPLAY_HEIGHT - y - 1]);
    }
    for (; y < DISPLAY_HEIGHT / 2 + RADIUS_OUTER_EDGE + 2; y++)
    {
        gScanlineEffectRegBuffers[0][EFFECTREG_WIN1H_OFFSET + y] = WIN_RANGE(
            DISPLAY_WIDTH / 2 - gScanlineEffectRegBuffers[0][EFFECTREG_OUTER_EDGE + DISPLAY_HEIGHT - y - 1],
            DISPLAY_WIDTH / 2 + gScanlineEffectRegBuffers[0][EFFECTREG_OUTER_EDGE + DISPLAY_HEIGHT - y - 1]);
    }
    sTransitionData->VBlank_DMA = TRUE;

    task->data[SPRITE_ID_OFFSET + 6] = CreateSprite(&sSpriteTemplate_Mask, DISPLAY_WIDTH - 168 - 16, DISPLAY_HEIGHT + 2 - 48 - 16, 0);
    task->data[SPRITE_ID_OFFSET + 7] = CreateSprite(&sSpriteTemplate_Mask, DISPLAY_WIDTH - 160 - 16, DISPLAY_HEIGHT + 2 - 16 - 16, 0);
    task->data[SPRITE_ID_OFFSET + 8] = CreateSprite(&sSpriteTemplate_Mask, DISPLAY_WIDTH - 128 - 16, DISPLAY_HEIGHT + 2 - 16 - 16, 0);
    for (i = 6; i < 9; i++)
        if (MAX_SPRITES != task->data[SPRITE_ID_OFFSET + i])
        {
            StartSpriteAnim(&gSprites[task->data[SPRITE_ID_OFFSET + i]], i - 6);
            gSprites[task->data[SPRITE_ID_OFFSET + i]].hFlip = TRUE;
            gSprites[task->data[SPRITE_ID_OFFSET + i]].vFlip = TRUE;
        }

    for (i = 0; i < 3; i++)
        if (MAX_SPRITES != task->data[SPRITE_ID_OFFSET + i])
        {
            StartSpriteAnim(&gSprites[task->data[SPRITE_ID_OFFSET + i]], i);
            gSprites[task->data[SPRITE_ID_OFFSET + i]].y = DISPLAY_HEIGHT + 2 - gSprites[task->data[SPRITE_ID_OFFSET + i]].y;
            gSprites[task->data[SPRITE_ID_OFFSET + i]].vFlip = TRUE;
        }

    task->tState++;
    task->tAngle++;
    return FALSE;
}

static bool8 DoubleClockWipePokeball_South1(struct Task *task)
{
    s32 x;
    u32 y;
    unsigned angle2;

    angle2 = task->tAngle * 256 / FRAMES_PER_ROTATION;

    sTransitionData->VBlank_DMA = FALSE;
    for (y = DISPLAY_HEIGHT / 2; y < DISPLAY_HEIGHT; y++)
    {
        x = Cotan(angle2, y - DISPLAY_HEIGHT / 2);
        gScanlineEffectRegBuffers[0][EFFECTREG_WIN0H_OFFSET + y] = WIN_RANGE(DISPLAY_WIDTH / 2 - x, DISPLAY_WIDTH);
    }

    sTransitionData->VBlank_DMA = TRUE;

    task->tAngle++;
    if (task->tAngle > FRAMES_PER_ROTATION - 1)
    {
        task->tState++;
    }
    return FALSE;
}

static bool8 DoubleClockWipePokeball_East1(struct Task *task)
{
    s32 x;
    u32 y;
    unsigned angle2;

    angle2 = task->tAngle * 256 / FRAMES_PER_ROTATION;

    sTransitionData->VBlank_DMA = FALSE;
    for (y = DISPLAY_HEIGHT / 2; y < DISPLAY_HEIGHT; y++)
    {
        gScanlineEffectRegBuffers[0][EFFECTREG_WIN0H_OFFSET + y] = WIN_RANGE(0, 0);
    }

    sTransitionData->VBlank_DMA = TRUE;

    task->tAngle++;
    task->tAngle -= FRAMES_PER_ROTATION;
    task->tState++;
    return FALSE;
}

static bool8 DoubleClockWipePokeball_NorthEast2(struct Task *task)
{
    unsigned i;
    s32 x;
    u32 y;
    unsigned angle2;

    angle2 = task->tAngle * 256 / FRAMES_PER_ROTATION;

    sTransitionData->VBlank_DMA = FALSE;
    for (y = DISPLAY_HEIGHT / 2 - RADIUS_OUTER_EDGE; y < DISPLAY_HEIGHT / 2 - RADIUS_INNER_EDGE; y++)
    {
        x = Cotan(angle2, DISPLAY_HEIGHT / 2 - y);
        gScanlineEffectRegBuffers[0][EFFECTREG_WIN1H_OFFSET + y] = WIN_RANGE(
            DISPLAY_WIDTH / 2 - gScanlineEffectRegBuffers[0][EFFECTREG_OUTER_EDGE + y],
            DISPLAY_WIDTH / 2 + min(x, gScanlineEffectRegBuffers[0][EFFECTREG_OUTER_EDGE + y]));
    }
    for (; y < DISPLAY_HEIGHT / 2 - RADIUS_BUTTON; y++)
    {
        x = Cotan(angle2, DISPLAY_HEIGHT / 2 - y);
        gScanlineEffectRegBuffers[0][EFFECTREG_WIN0H_OFFSET + y] = WIN_RANGE(
            DISPLAY_WIDTH / 2 - gScanlineEffectRegBuffers[0][EFFECTREG_OUTER_EDGE + y],
            DISPLAY_WIDTH / 2 - gScanlineEffectRegBuffers[0][EFFECTREG_INNER_EDGE + y]);
        gScanlineEffectRegBuffers[0][EFFECTREG_WIN1H_OFFSET + y] = WIN_RANGE(
            DISPLAY_WIDTH / 2 + min(x, gScanlineEffectRegBuffers[0][EFFECTREG_INNER_EDGE + y]),
            DISPLAY_WIDTH / 2 + min(x, gScanlineEffectRegBuffers[0][EFFECTREG_OUTER_EDGE + y]));
    }
    for (; y < DISPLAY_HEIGHT / 2 - HALFWIDTH_BELT; y++)
    {
        x = Cotan(angle2, DISPLAY_HEIGHT / 2 - y);
        gScanlineEffectRegBuffers[0][EFFECTREG_WIN0H_OFFSET + y] = WIN_RANGE(
            DISPLAY_WIDTH / 2 - gScanlineEffectRegBuffers[0][EFFECTREG_BUTTON + y],
            DISPLAY_WIDTH / 2 + min(x, gScanlineEffectRegBuffers[0][EFFECTREG_BUTTON + y]));
        gScanlineEffectRegBuffers[0][EFFECTREG_WIN1H_OFFSET + y] = WIN_RANGE(
            DISPLAY_WIDTH / 2 + min(x, gScanlineEffectRegBuffers[0][EFFECTREG_INNER_EDGE + y]),
            DISPLAY_WIDTH / 2 + min(x, gScanlineEffectRegBuffers[0][EFFECTREG_OUTER_EDGE + y]));
    }
    for (; y < DISPLAY_HEIGHT / 2; y++)
    {
        x = Cotan(angle2, DISPLAY_HEIGHT / 2 - y);
        gScanlineEffectRegBuffers[0][EFFECTREG_WIN1H_OFFSET + y] = WIN_RANGE(
            DISPLAY_WIDTH / 2 - gScanlineEffectRegBuffers[0][EFFECTREG_OUTER_EDGE + y],
            DISPLAY_WIDTH / 2 + min(x, gScanlineEffectRegBuffers[0][EFFECTREG_OUTER_EDGE + y]));
    }
    sTransitionData->VBlank_DMA = TRUE;

    task->tAngle++;
    if (task->tAngle > FRAMES_PER_ROTATION * 1 / 4)
        task->tState++;
    return FALSE;
}

static bool8 DoubleClockWipePokeball_North2(struct Task *task)
{
    unsigned i;
    for (i = 3; i < 6; i++)
        if (MAX_SPRITES != task->data[SPRITE_ID_OFFSET + i])
            DestroySprite(&gSprites[task->data[SPRITE_ID_OFFSET + i]]);

    task->tState++;
    return TRUE;
}

static bool8 DoubleClockWipePokeball_NorthWest2(struct Task *task)
{
    unsigned i;
    s32 x;
    u32 y;
    unsigned angle2;

    angle2 = task->tAngle * 256 / FRAMES_PER_ROTATION;

    sTransitionData->VBlank_DMA = FALSE;
    for (y = DISPLAY_HEIGHT / 2 - RADIUS_OUTER_EDGE; y < DISPLAY_HEIGHT / 2 - RADIUS_INNER_EDGE; y++)
    {
        x = Cotan(angle2, DISPLAY_HEIGHT / 2 - y);
        gScanlineEffectRegBuffers[0][EFFECTREG_WIN1H_OFFSET + y] = WIN_RANGE(
            DISPLAY_WIDTH / 2 + min(x, -gScanlineEffectRegBuffers[0][EFFECTREG_OUTER_EDGE + y]),
            DISPLAY_WIDTH / 2 + min(x, gScanlineEffectRegBuffers[0][EFFECTREG_OUTER_EDGE + y]));
    }
    for (; y < DISPLAY_HEIGHT / 2 - RADIUS_BUTTON; y++)
    {
        x = Cotan(angle2, DISPLAY_HEIGHT / 2 - y);
        gScanlineEffectRegBuffers[0][EFFECTREG_WIN0H_OFFSET + y] = WIN_RANGE(
            DISPLAY_WIDTH / 2 + min(x, -gScanlineEffectRegBuffers[0][EFFECTREG_OUTER_EDGE + y]),
            DISPLAY_WIDTH / 2 + min(x, -gScanlineEffectRegBuffers[0][EFFECTREG_INNER_EDGE + y]));
    }
    for (; y < DISPLAY_HEIGHT / 2 - HALFWIDTH_BELT; y++)
    {
        x = Cotan(angle2, DISPLAY_HEIGHT / 2 - y);
        gScanlineEffectRegBuffers[0][EFFECTREG_WIN0H_OFFSET + y] = WIN_RANGE(
            DISPLAY_WIDTH / 2 + min(x, -gScanlineEffectRegBuffers[0][EFFECTREG_BUTTON + y]),
            DISPLAY_WIDTH / 2 + min(x, gScanlineEffectRegBuffers[0][EFFECTREG_BUTTON + y]));
        gScanlineEffectRegBuffers[0][EFFECTREG_WIN1H_OFFSET + y] = WIN_RANGE(
            DISPLAY_WIDTH / 2 + min(x, -gScanlineEffectRegBuffers[0][EFFECTREG_OUTER_EDGE + y]),
            DISPLAY_WIDTH / 2 + min(x, -gScanlineEffectRegBuffers[0][EFFECTREG_INNER_EDGE + y]));
    }
    for (; y < DISPLAY_HEIGHT / 2; y++)
    {
        x = Cotan(angle2, DISPLAY_HEIGHT / 2 - y);
        gScanlineEffectRegBuffers[0][EFFECTREG_WIN1H_OFFSET + y] = WIN_RANGE(
            DISPLAY_WIDTH / 2 + min(x, -gScanlineEffectRegBuffers[0][EFFECTREG_OUTER_EDGE + y]),
            DISPLAY_WIDTH / 2 + min(x, gScanlineEffectRegBuffers[0][EFFECTREG_OUTER_EDGE + y]));
    }
    sTransitionData->VBlank_DMA = TRUE;

    task->tAngle++;
    if (task->tAngle > FRAMES_PER_ROTATION * 2 / 4 - 1)
        task->tState++;
    return FALSE;
}

static bool8 DoubleClockWipePokeball_West2(struct Task *task)
{
    unsigned i;
    u32 y;

    for (i = 6; i < 9; i++)
        if (MAX_SPRITES != task->data[SPRITE_ID_OFFSET + i])
            DestroySprite(&gSprites[task->data[SPRITE_ID_OFFSET + i]]);

    sTransitionData->VBlank_DMA = FALSE;
    for (y = 0; y < DISPLAY_HEIGHT / 2; y++)
    {
        gScanlineEffectRegBuffers[0][EFFECTREG_WIN0H_OFFSET + y] = WIN_RANGE(0, 0);
        gScanlineEffectRegBuffers[0][EFFECTREG_WIN1H_OFFSET + y] = WIN_RANGE(0, 0);
    }

    for (; y < DISPLAY_HEIGHT / 2 + HALFWIDTH_BELT; y++)
    {
        gScanlineEffectRegBuffers[0][EFFECTREG_WIN0H_OFFSET + y] = WIN_RANGE(0, 0);
        gScanlineEffectRegBuffers[0][EFFECTREG_WIN1H_OFFSET + y] = WIN_RANGE(
            DISPLAY_WIDTH / 2 - gScanlineEffectRegBuffers[0][EFFECTREG_OUTER_EDGE + DISPLAY_HEIGHT - y - 1],
            DISPLAY_WIDTH / 2 + gScanlineEffectRegBuffers[0][EFFECTREG_OUTER_EDGE + DISPLAY_HEIGHT - y - 1]);
    }
    for (; y < DISPLAY_HEIGHT / 2 + RADIUS_BUTTON; y++)
    {
        gScanlineEffectRegBuffers[0][EFFECTREG_WIN0H_OFFSET + y] = WIN_RANGE(
            DISPLAY_WIDTH / 2 - gScanlineEffectRegBuffers[0][EFFECTREG_BUTTON + DISPLAY_HEIGHT - y - 1],
            DISPLAY_WIDTH / 2 + gScanlineEffectRegBuffers[0][EFFECTREG_BUTTON + DISPLAY_HEIGHT - y - 1]);
        gScanlineEffectRegBuffers[0][EFFECTREG_WIN1H_OFFSET + y] = WIN_RANGE(
            DISPLAY_WIDTH / 2 - gScanlineEffectRegBuffers[0][EFFECTREG_OUTER_EDGE + DISPLAY_HEIGHT - y - 1],
            DISPLAY_WIDTH / 2 - gScanlineEffectRegBuffers[0][EFFECTREG_INNER_EDGE + DISPLAY_HEIGHT - y - 1]);
    }
    for (; y < DISPLAY_HEIGHT / 2 + RADIUS_INNER_EDGE; y++)
    {
        gScanlineEffectRegBuffers[0][EFFECTREG_WIN0H_OFFSET + y] = WIN_RANGE(
            DISPLAY_WIDTH / 2 + gScanlineEffectRegBuffers[0][EFFECTREG_INNER_EDGE + DISPLAY_HEIGHT - y - 1],
            DISPLAY_WIDTH / 2 + gScanlineEffectRegBuffers[0][EFFECTREG_OUTER_EDGE + DISPLAY_HEIGHT - y - 1]);
        gScanlineEffectRegBuffers[0][EFFECTREG_WIN1H_OFFSET + y] = WIN_RANGE(
            DISPLAY_WIDTH / 2 - gScanlineEffectRegBuffers[0][EFFECTREG_OUTER_EDGE + DISPLAY_HEIGHT - y - 1],
            DISPLAY_WIDTH / 2 - gScanlineEffectRegBuffers[0][EFFECTREG_INNER_EDGE + DISPLAY_HEIGHT - y - 1]);
    }
    for (; y < DISPLAY_HEIGHT / 2 + RADIUS_OUTER_EDGE + 2; y++)
    {
        gScanlineEffectRegBuffers[0][EFFECTREG_WIN0H_OFFSET + y] = WIN_RANGE(0, 0);
        gScanlineEffectRegBuffers[0][EFFECTREG_WIN1H_OFFSET + y] = WIN_RANGE(
            DISPLAY_WIDTH / 2 - gScanlineEffectRegBuffers[0][EFFECTREG_OUTER_EDGE + DISPLAY_HEIGHT - y - 1],
            DISPLAY_WIDTH / 2 + gScanlineEffectRegBuffers[0][EFFECTREG_OUTER_EDGE + DISPLAY_HEIGHT - y - 1]);
    }
    sTransitionData->VBlank_DMA = TRUE;

    task->tState++;
    task->tAngle++;
    return FALSE;
}

static bool8 DoubleClockWipePokeball_SouthWest2(struct Task *task)
{
    unsigned i;
    s32 x;
    u32 y;
    unsigned angle2;

    angle2 = task->tAngle * 256 / FRAMES_PER_ROTATION;

    sTransitionData->VBlank_DMA = FALSE;
    for (y = DISPLAY_HEIGHT / 2; y < DISPLAY_HEIGHT / 2 + HALFWIDTH_BELT; y++)
    {
        x = Cotan(angle2, y - DISPLAY_HEIGHT / 2);
        gScanlineEffectRegBuffers[0][EFFECTREG_WIN1H_OFFSET + y] = WIN_RANGE(
            DISPLAY_WIDTH / 2 - min(x, gScanlineEffectRegBuffers[0][EFFECTREG_OUTER_EDGE + DISPLAY_HEIGHT - y - 1]),
            DISPLAY_WIDTH / 2 + gScanlineEffectRegBuffers[0][EFFECTREG_OUTER_EDGE + DISPLAY_HEIGHT - y - 1]);
    }
    for (; y < DISPLAY_HEIGHT / 2 + RADIUS_BUTTON; y++)
    {
        x = Cotan(angle2, y - DISPLAY_HEIGHT / 2);
        gScanlineEffectRegBuffers[0][EFFECTREG_WIN0H_OFFSET + y] = WIN_RANGE(
            DISPLAY_WIDTH / 2 - min(x, gScanlineEffectRegBuffers[0][EFFECTREG_BUTTON + DISPLAY_HEIGHT - y - 1]),
            DISPLAY_WIDTH / 2 + gScanlineEffectRegBuffers[0][EFFECTREG_BUTTON + DISPLAY_HEIGHT - y - 1]);
        gScanlineEffectRegBuffers[0][EFFECTREG_WIN1H_OFFSET + y] = WIN_RANGE(
            DISPLAY_WIDTH / 2 - min(x, gScanlineEffectRegBuffers[0][EFFECTREG_OUTER_EDGE + DISPLAY_HEIGHT - y - 1]),
            DISPLAY_WIDTH / 2 - min(x, gScanlineEffectRegBuffers[0][EFFECTREG_INNER_EDGE + DISPLAY_HEIGHT - y - 1]));
    }
    for (; y < DISPLAY_HEIGHT / 2 + RADIUS_INNER_EDGE; y++)
    {
        x = Cotan(angle2, y - DISPLAY_HEIGHT / 2);
        gScanlineEffectRegBuffers[0][EFFECTREG_WIN1H_OFFSET + y] = WIN_RANGE(
            DISPLAY_WIDTH / 2 - min(x, gScanlineEffectRegBuffers[0][EFFECTREG_OUTER_EDGE + DISPLAY_HEIGHT - y - 1]),
            DISPLAY_WIDTH / 2 - min(x, gScanlineEffectRegBuffers[0][EFFECTREG_INNER_EDGE + DISPLAY_HEIGHT - y - 1]));
    }
    for (; y < DISPLAY_HEIGHT / 2 + RADIUS_OUTER_EDGE + 2; y++)
    {
        x = Cotan(angle2, y - DISPLAY_HEIGHT / 2);
        gScanlineEffectRegBuffers[0][EFFECTREG_WIN1H_OFFSET + y] = WIN_RANGE(
            DISPLAY_WIDTH / 2 - min(x, gScanlineEffectRegBuffers[0][EFFECTREG_OUTER_EDGE + DISPLAY_HEIGHT - y - 1]),
            DISPLAY_WIDTH / 2 + gScanlineEffectRegBuffers[0][EFFECTREG_OUTER_EDGE + DISPLAY_HEIGHT - y - 1]);
    }
    sTransitionData->VBlank_DMA = TRUE;

    task->tAngle++;
    if (task->tAngle > FRAMES_PER_ROTATION * 3 / 4)
        task->tState++;
    return FALSE;
}

static bool8 DoubleClockWipePokeball_South2(struct Task *task)
{
    unsigned i;
    for (i = 0; i < 3; i++)
        if (MAX_SPRITES != task->data[SPRITE_ID_OFFSET + i])
            DestroySprite(&gSprites[task->data[SPRITE_ID_OFFSET + i]]);

    task->tState++;
    return TRUE;
}

static bool8 DoubleClockWipePokeball_SouthEast2(struct Task *task)
{
    unsigned i;
    s32 x;
    u32 y;
    unsigned angle2;

    angle2 = task->tAngle * 256 / FRAMES_PER_ROTATION;

    sTransitionData->VBlank_DMA = FALSE;
    for (y = DISPLAY_HEIGHT / 2; y < DISPLAY_HEIGHT / 2 + HALFWIDTH_BELT; y++)
    {
        x = Cotan(angle2, y - DISPLAY_HEIGHT / 2);
        gScanlineEffectRegBuffers[0][EFFECTREG_WIN1H_OFFSET + y] = WIN_RANGE(
            DISPLAY_WIDTH / 2 - min(x, gScanlineEffectRegBuffers[0][EFFECTREG_OUTER_EDGE + DISPLAY_HEIGHT - y - 1]),
            DISPLAY_WIDTH / 2 - min(x, -gScanlineEffectRegBuffers[0][EFFECTREG_OUTER_EDGE + DISPLAY_HEIGHT - y - 1]));
    }
    for (; y < DISPLAY_HEIGHT / 2 + RADIUS_BUTTON; y++)
    {
        x = Cotan(angle2, y - DISPLAY_HEIGHT / 2);
        gScanlineEffectRegBuffers[0][EFFECTREG_WIN0H_OFFSET + y] = WIN_RANGE(
            DISPLAY_WIDTH / 2 - min(x, gScanlineEffectRegBuffers[0][EFFECTREG_BUTTON + DISPLAY_HEIGHT - y - 1]),
            DISPLAY_WIDTH / 2 - min(x, -gScanlineEffectRegBuffers[0][EFFECTREG_BUTTON + DISPLAY_HEIGHT - y - 1]));
        gScanlineEffectRegBuffers[0][EFFECTREG_WIN1H_OFFSET + y] = WIN_RANGE(
            DISPLAY_WIDTH / 2 - min(x, -gScanlineEffectRegBuffers[0][EFFECTREG_INNER_EDGE + DISPLAY_HEIGHT - y - 1]),
            DISPLAY_WIDTH / 2 - min(x, -gScanlineEffectRegBuffers[0][EFFECTREG_OUTER_EDGE + DISPLAY_HEIGHT - y - 1]));
    }
    for (; y < DISPLAY_HEIGHT / 2 + RADIUS_INNER_EDGE; y++)
    {
        x = Cotan(angle2, y - DISPLAY_HEIGHT / 2);
        gScanlineEffectRegBuffers[0][EFFECTREG_WIN0H_OFFSET + y] = WIN_RANGE(
            DISPLAY_WIDTH / 2 - min(x, -gScanlineEffectRegBuffers[0][EFFECTREG_INNER_EDGE + DISPLAY_HEIGHT - y - 1]),
            DISPLAY_WIDTH / 2 - min(x, -gScanlineEffectRegBuffers[0][EFFECTREG_OUTER_EDGE + DISPLAY_HEIGHT - y - 1]));
    }
    for (; y < DISPLAY_HEIGHT / 2 + RADIUS_OUTER_EDGE + 2; y++)
    {
        x = Cotan(angle2, y - DISPLAY_HEIGHT / 2);
        gScanlineEffectRegBuffers[0][EFFECTREG_WIN1H_OFFSET + y] = WIN_RANGE(
            DISPLAY_WIDTH / 2 - min(x, gScanlineEffectRegBuffers[0][EFFECTREG_OUTER_EDGE + DISPLAY_HEIGHT - y - 1]),
            DISPLAY_WIDTH / 2 - min(x, -gScanlineEffectRegBuffers[0][EFFECTREG_OUTER_EDGE + DISPLAY_HEIGHT - y - 1]));
    }
    sTransitionData->VBlank_DMA = TRUE;

    task->tAngle++;
    if (task->tAngle > FRAMES_PER_ROTATION - 1)
        task->tState++;
    return FALSE;
}

static bool8 DoubleClockWipePokeball_East2(struct Task *task)
{
    s32 x;
    u32 y;
    unsigned angle2;

    angle2 = task->tAngle * 256 / FRAMES_PER_ROTATION;

    sTransitionData->VBlank_DMA = FALSE;
    for (y = DISPLAY_HEIGHT / 2; y < DISPLAY_HEIGHT; y++)
    {
        gScanlineEffectRegBuffers[0][EFFECTREG_WIN0H_OFFSET + y] = WIN_RANGE(0, 0);
        gScanlineEffectRegBuffers[0][EFFECTREG_WIN1H_OFFSET + y] = WIN_RANGE(0, 0);
    }

    sTransitionData->VBlank_DMA = TRUE;

    task->tAngle++;
    task->tAngle -= FRAMES_PER_ROTATION;
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
