#include "global.h"
#include "battle_transition.h"
#include "battle_transition_shared.h"
#include "battle_transition_tasks/speed_lines.h"
#include "decompress.h"
#include "gpu_regs.h"
#include "palette.h"
#include "random.h"
#include "scanline_effect.h"
#include "task.h"
#include "constants/rgb.h"

#define Q_12_4(n) ((s16)(n * 16))
#define Q_12_4_TO_INT(n) ((s16)(n / 16))

#define PALTAG_SPEED_LINES (0xFA57)
#define TILETAG_SPEED_LINES (0xFA57)

#define SPEEDLINE_SMALL_FRAMES (4)
#define SPEEDLINE_INITIAL_SPEED_FRAMES (12)
#define INITIAL_VELOCITY (Q_12_4(4))
#define INITIAL_ACCELERATION (3)
#define JERK (Q_12_4(0))
#define START_FADE_VELOCITY (INITIAL_VELOCITY + Q_12_4(INITIAL_ACCELERATION * 3))

static const u16 sSpeedLines_Palette[] = INCBIN_U16("graphics/battle_transitions/speedlines_bg.gbapal");
static const u32 sSpeedLines_Bg_Tileset[] = INCBIN_U32("graphics/battle_transitions/speedlines_bg.4bpp.lz");
static const u32 sSpeedLines_Bg_Small_Tileset[] = INCBIN_U32("graphics/battle_transitions/speedlines_bg_small.4bpp.lz");
static const u32 sSpeedLines_Sprites[] = INCBIN_U32("graphics/battle_transitions/speedlines_sprites.4bpp.lz");

static bool8 SpeedLines_Init(struct Task *);
static bool8 SpeedLines_ConstantVelocityUntilTimerZero(struct Task *);
static bool8 SpeedLines_AccelerateUntilFadeComplete(struct Task *);
static bool8 SpeedLines_SwapToFullSize(struct Task *);
static bool8 SpeedLines_AccelerateUntilFadeStart(struct Task *);
static bool8 SpeedLines_FadeToWhite(struct Task *);
static bool8 SpeedLines_FadeWhiteToBlack(struct Task *);
static bool8 SpeedLines_End(struct Task *);
static void SpeedLines_SpriteCallback(struct Sprite* sprite);

static const struct CompressedSpriteSheet sSpriteSheet_SpeedLines =
{
    .data = sSpeedLines_Sprites,
    .size = 8 * 4 * 5 * 4,
    .tag = TILETAG_SPEED_LINES,
};

static const struct SpritePalette sSpritePalette_SpeedLines =
{
    .data = sSpeedLines_Palette,
    .tag = PALTAG_SPEED_LINES,
};

static const struct OamData sOamData_SpeedLines_16 =
{
    .y = 0,
    .affineMode = ST_OAM_AFFINE_OFF,
    .objMode = ST_OAM_OBJ_NORMAL,
    .mosaic = FALSE,
    .bpp = ST_OAM_4BPP,
    .shape = SPRITE_SHAPE(16x8),
    .x = 0,
    .matrixNum = 0,
    .size = SPRITE_SIZE(16x8),
    .tileNum = 0,
    .priority = 1,
    .paletteNum = 0,
    .affineParam = 0,
};

static const struct OamData sOamData_SpeedLines_32 =
{
    .y = 0,
    .affineMode = ST_OAM_AFFINE_OFF,
    .objMode = ST_OAM_OBJ_NORMAL,
    .mosaic = FALSE,
    .bpp = ST_OAM_4BPP,
    .shape = SPRITE_SHAPE(32x8),
    .x = 0,
    .matrixNum = 0,
    .size = SPRITE_SIZE(32x8),
    .tileNum = 0,
    .priority = 1,
    .paletteNum = 0,
    .affineParam = 0,
};

static const union AnimCmd sAnim_SpeedLine_0[] =
{
    ANIMCMD_FRAME(0, SPEEDLINE_SMALL_FRAMES),
    ANIMCMD_FRAME(2, 1),
    ANIMCMD_END,
};

static const union AnimCmd sAnim_SpeedLine_1[] =
{
    ANIMCMD_FRAME(4, SPEEDLINE_SMALL_FRAMES),
    ANIMCMD_FRAME(8, 1),
    ANIMCMD_END,
};

static const union AnimCmd sAnim_SpeedLine_2[] =
{
    ANIMCMD_FRAME(12, SPEEDLINE_SMALL_FRAMES),
    ANIMCMD_FRAME(16, 1),
    ANIMCMD_END,
};

#define SPEED_LINE_SPRITE_VARIANT_COUNT (4)
static const union AnimCmd *const sAnimTable_SpeedLine[SPEED_LINE_SPRITE_VARIANT_COUNT - 1] =
{
    sAnim_SpeedLine_0,
    sAnim_SpeedLine_1,
    sAnim_SpeedLine_2,
};

static const struct SpriteTemplate sSpriteTemplate_SpeedLine_16 =
{
    .tileTag = TILETAG_SPEED_LINES,
    .paletteTag = PALTAG_SPEED_LINES,
    .oam = &sOamData_SpeedLines_16,
    .anims = sAnimTable_SpeedLine,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpeedLines_SpriteCallback,
};

static const struct SpriteTemplate sSpriteTemplate_SpeedLine_32 =
{
    .tileTag = TILETAG_SPEED_LINES,
    .paletteTag = PALTAG_SPEED_LINES,
    .oam = &sOamData_SpeedLines_32,
    .anims = sAnimTable_SpeedLine,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpeedLines_SpriteCallback,
};

#define SPEED_LINE_BG_VARIANT_COUNT (6)
static const u16 sSpeedLineVariantsBg[SPEED_LINE_BG_VARIANT_COUNT][4] = {
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
    SpeedLines_AccelerateUntilFadeStart,
    SpeedLines_FadeToWhite,
    SpeedLines_AccelerateUntilFadeComplete,
    SpeedLines_FadeWhiteToBlack,
    SpeedLines_End,
};

void Task_SpeedLines(u8 taskId)
{
    while (sSpeedLines_Funcs[gTasks[taskId].tState](&gTasks[taskId]));
}

#define tTimer data[7]
#define tFade data[14]
#define tPosition data[2]
#define tVelocity data[3]
#define tAcceleration data[4]

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

    CpuFill16(0, tilemap, BG_SCREEN_SIZE);

    x = 0;
    for (y = 0; y < DISPLAY_TILE_HEIGHT; y++)
    for (i = 0; i < 2; i++)
    {
        x += 4 + (Random() % 8);
        x %= 32;

        variant = Random() % (2 * SPEED_LINE_BG_VARIANT_COUNT);
        if (variant >= SPEED_LINE_BG_VARIANT_COUNT)
        {
            variant -= SPEED_LINE_BG_VARIANT_COUNT;
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


    LoadSpritePalette(&sSpritePalette_SpeedLines);
    LoadCompressedSpriteSheet(&sSpriteSheet_SpeedLines);
    x = 6 * 8;
    for (y = 0; y < DISPLAY_HEIGHT; y += 8)
    for (i = 0; i < 2; i++)
    {
        x += (4 + (Random() % 8)) * 8;
        x %= 256;

        variant = Random() % (2 * SPEED_LINE_SPRITE_VARIANT_COUNT);
        if (variant >= SPEED_LINE_SPRITE_VARIANT_COUNT)
        {
            variant -= SPEED_LINE_SPRITE_VARIANT_COUNT;
            vflip_factor = 4;
        }
        else
        {
            vflip_factor = 0;
        }

        if (variant)
        {
            u8 spriteId;
            if (1 == variant)
                spriteId = CreateSprite(&sSpriteTemplate_SpeedLine_16, x, y + vflip_factor, 0);
            else
                spriteId = CreateSprite(&sSpriteTemplate_SpeedLine_32, x, y + vflip_factor, 0);

            if (spriteId != MAX_SPRITES)
            {
                gSprites[spriteId].tTimer = 0;
                gSprites[spriteId].tPosition = Q_12_4(x);
                gSprites[spriteId].tVelocity = INITIAL_VELOCITY;
                gSprites[spriteId].tAcceleration = INITIAL_ACCELERATION;
                StartSpriteAnim(&gSprites[spriteId], variant - 1);
            }
        }
    }

    task->tTimer = SPEEDLINE_SMALL_FRAMES;
    task->tPosition = Q_12_4(0);
    task->tVelocity = INITIAL_VELOCITY;
    task->tAcceleration = INITIAL_ACCELERATION;

    task->tState++;
    return TRUE;
}

static bool8 SpeedLines_SwapToFullSize(struct Task *task)
{
    u16 *tilemap, *tileset;
    GetBg0TilesDst(&tilemap, &tileset);
    LZ77UnCompVram(sSpeedLines_Bg_Tileset, tileset);
    task->tState++;
    task->tTimer = SPEEDLINE_INITIAL_SPEED_FRAMES;
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

static bool8 SpeedLines_AccelerateUntilFadeStart(struct Task *task)
{
    task->tAcceleration += JERK;
    task->tVelocity += task->tAcceleration;
    task->tPosition += task->tVelocity;
    SetGpuReg(REG_OFFSET_BG0HOFS, Q_12_4_TO_INT(task->tPosition));

    if (task->tVelocity > START_FADE_VELOCITY) {
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

static bool8 SpeedLines_AccelerateUntilFadeComplete(struct Task *task)
{
    task->tAcceleration += JERK;
    task->tVelocity += task->tAcceleration;
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
        FreeSpritePaletteByTag(PALTAG_SPEED_LINES);
        FreeSpriteTilesByTag(TILETAG_SPEED_LINES);
        FadeScreenBlack();
        DestroyTask(FindTaskIdByFunc(task->func));
    }
    return FALSE;
}

static void SpeedLines_SpriteCallback(struct Sprite* sprite)
{
    if (sprite->tTimer > SPEEDLINE_INITIAL_SPEED_FRAMES + SPEEDLINE_SMALL_FRAMES)
    {
        sprite->tAcceleration += JERK;
        sprite->tVelocity += sprite->tAcceleration;
    }
    sprite->tPosition += sprite->tVelocity;
    sprite->x = Q_12_4_TO_INT(sprite->tPosition) & 0xFF;

    sprite->tTimer++;
}
