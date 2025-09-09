#include "global.h"
#include "battle_transition.h"
#include "battle_transition_shared.h"
#include "battle_transition_tasks/regis_oras.h"
#include "constants/rgb.h"
#include "decompress.h"
#include "gpu_regs.h"
#include "main.h"
#include "palette.h"
#include "scanline_effect.h"
#include "task.h"

static const u32 sRegisEye_Tileset[] = INCBIN_U32("graphics/battle_transitions/regis_oras.4bpp.lz");
static const u32 sRegisGlow_Tileset[] = INCBIN_U32("graphics/battle_transitions/regis_oras_glow.4bpp.lz");

#define PALTAG_REGI_EYE (0xE7E5)
#define TILETAG_REGI_EYE (0xE7E5)
#define GLOW_TILES_START (40)
#define EYE_COUNT (7)

enum {
    REGI_REGIROCK,
    REGI_COUNT
};

struct eye_pattern {u8 x; u8 y;};
struct regi_params {
    u16 palette[16];
    struct eye_pattern eye_pattern[EYE_COUNT];
};
static const struct regi_params regi_params[REGI_COUNT] = {
    [REGI_REGIROCK] = {
        .palette = INCBIN_U16("graphics/battle_transitions/regirock_oras.gbapal"),
        .eye_pattern = {
            {.x = 13, .y = 8},
            {6, 8},
            {20, 8},
            {6, 2},
            {20, 14},
            {20, 2},
            {6, 14},
        }
    },
};

static const struct CompressedSpriteSheet sSpriteSheet_Regi =
{
    .data = sRegisEye_Tileset,
    .size = 8 * 4 * 8 * 4,
    .tag = TILETAG_REGI_EYE,
};

static const union AnimCmd sAnim_Eye_Gem[] =
{
    ANIMCMD_FRAME(0, 1),
    ANIMCMD_END,
};

static const union AnimCmd sAnim_Eye_Glow[] =
{
    ANIMCMD_FRAME(16, 1),
    ANIMCMD_END,
};

static const union AnimCmd *const sAnimTable_Eye[] =
{
    sAnim_Eye_Gem,
    sAnim_Eye_Glow,
};

static const struct OamData sOamData_Eye =
{
    .y = 0,
    .affineMode = ST_OAM_AFFINE_OFF,
    .objMode = ST_OAM_OBJ_NORMAL,
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

static void dummySpriteCB(struct Sprite *sprite)
{
}

static const struct SpriteTemplate sSpriteTemplate_EyeGem =
{
    .tileTag = TILETAG_REGI_EYE,
    .paletteTag = TILETAG_REGI_EYE,
    .oam = &sOamData_Eye,
    .anims = sAnimTable_Eye,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = dummySpriteCB,
};

static bool8 RegisOras_Init(struct Task *);
static bool8 RegisOras_FramesCountdown(struct Task *);
static bool8 RegisOras_DoBlendFade(struct Task *);
static bool8 RegisOras_SetupEyeFade0(struct Task *);
static bool8 RegisOras_EndEyeFade0(struct Task *);
static bool8 RegisOras_SetupEyeFade1(struct Task *);
static bool8 RegisOras_EndEyeFade1(struct Task *);
static bool8 RegisOras_SetupEyeFade2(struct Task *);
static bool8 RegisOras_EndEyeFade2(struct Task *);
static bool8 RegisOras_SetupEyeFade3(struct Task *);
static bool8 RegisOras_EndEyeFade3(struct Task *);
static bool8 RegisOras_SetupEyeFade4(struct Task *);
static bool8 RegisOras_EndEyeFade4(struct Task *);
static bool8 RegisOras_SetupEyeFade5(struct Task *);
static bool8 RegisOras_EndEyeFade5(struct Task *);
static bool8 RegisOras_SetupEyeFade6(struct Task *);
static bool8 RegisOras_EndEyeFade6(struct Task *);
static bool8 RegisOras_SetupEyeFade7(struct Task *);
static bool8 RegisOras_EndEyeFade7(struct Task *);
static bool8 RegisOras_SetupFadeToGlow(struct Task *);
static bool8 RegisOras_DoFadeToGlow(struct Task *);
static bool8 RegisOras_SetupFadeToBlack(struct Task *);
static bool8 RegisOras_DoFadeToBlack(struct Task *);

static const TransitionStateFunc sRegisOras_Funcs[] = {
    RegisOras_Init,
    RegisOras_SetupEyeFade0,
    RegisOras_DoBlendFade,
    RegisOras_EndEyeFade0,
    RegisOras_SetupEyeFade1,
    RegisOras_DoBlendFade,
    RegisOras_EndEyeFade1,
    RegisOras_SetupEyeFade2,
    RegisOras_DoBlendFade,
    RegisOras_EndEyeFade2,
    RegisOras_SetupEyeFade3,
    RegisOras_DoBlendFade,
    RegisOras_EndEyeFade3,
    RegisOras_SetupEyeFade4,
    RegisOras_DoBlendFade,
    RegisOras_EndEyeFade4,
    RegisOras_SetupEyeFade5,
    RegisOras_DoBlendFade,
    RegisOras_EndEyeFade5,
    RegisOras_SetupEyeFade6,
    RegisOras_DoBlendFade,
    RegisOras_EndEyeFade6,
    RegisOras_SetupEyeFade7,
    RegisOras_DoBlendFade,
    RegisOras_EndEyeFade7,
    RegisOras_FramesCountdown,
    RegisOras_SetupFadeToGlow,
    RegisOras_DoFadeToGlow,
    RegisOras_FramesCountdown,
    RegisOras_SetupFadeToBlack,
    RegisOras_DoFadeToBlack,
};

#define tBlendTarget1 data[1]
#define tBlendTarget2 data[2]
#define tBlendDelay   data[3]
#define tSpritePalette data[6]
#define tPatternIndex data[7]
#define tEndDelay     data[8]
#define tSpriteId(i)     data[9 + (i)]

void Task_RegirockOras(u8 taskId)
{
    gTasks[taskId].tPatternIndex = REGI_REGIROCK;
    while (sRegisOras_Funcs[gTasks[taskId].tState](&gTasks[taskId]));
}

static void VBlankCB_SetBlend(void)
{
    VBlankCB_BattleTransition();
    REG_BLDCNT = sTransitionData->BLDCNT;
    REG_BLDALPHA = sTransitionData->BLDALPHA;
    REG_BLDY = sTransitionData->BLDY;
}

static inline void RegisOras_ClearEye(u16 *tilemap, struct eye_pattern pos)
{
    unsigned i, j;

    tilemap += pos.x + pos.y * 32;

    for (j = 0; j < 4; j++)
    {
        for (i = 0; i < 4; i++)
        {
            *tilemap = 0;
            tilemap++;
        }
        tilemap += (32 - 4);
    }
}

static inline void RegisOras_DrawEye(u16 *tilemap, struct eye_pattern pos, u16 start_value)
{
    unsigned i, j;
    u16 value = start_value;

    tilemap += pos.x + pos.y * 32;

    for (j = 0; j < 4; j++)
    {
        for (i = 0; i < 4; i++)
        {
            *tilemap = value;
            tilemap++;
            value++;
        }
        tilemap += (32 - 4);
    }
}

static inline void RegisOras_DrawEyeGlow(u16 *tilemap, struct eye_pattern pos)
{
    unsigned i, j;
    u16 value = GLOW_TILES_START | (0xF << 12);

    tilemap += (pos.x - 1) + (pos.y - 1) * 32;

    for (j = 0; j < 6; j++)
    {
        for (i = 0; i < 6; i++)
        {
            *tilemap = value;
            tilemap++;
            value++;
        }
        tilemap += (32 - 6);
    }
}

static bool8 RegisOras_Init(struct Task *task)
{
    unsigned i;
    u16 *tilemap, *tileset;
    struct SpritePalette spritePalette;

    InitTransitionData();
    ScanlineEffect_Clear();
    ClearGpuRegBits(REG_OFFSET_DISPCNT, DISPCNT_WIN0_ON | DISPCNT_WIN1_ON);

    LoadPalette(regi_params[task->tPatternIndex].palette, BG_PLTT_ID(15), sizeof(regi_params[task->tPatternIndex].palette));
    GetBg0TilesDst(&tilemap, &tileset);
    CpuFill16(0, tilemap, BG_SCREEN_SIZE);
    CpuFill16(0, tileset, TILE_SIZE_4BPP / sizeof(u16));
    LZ77UnCompVram(sSpriteSheet_Regi.data, tileset + TILE_SIZE_4BPP / sizeof(u16));

    spritePalette.data = regi_params[task->tPatternIndex].palette;
    spritePalette.tag = PALTAG_REGI_EYE;
    task->tSpritePalette = LoadSpritePalette(&spritePalette);

    LoadCompressedSpriteSheet(&sSpriteSheet_Regi);

    task->tBlendTarget1 = 0;
    task->tBlendTarget2 = 16;
    for (i = 0; i < EYE_COUNT; i++)
    {
        task->tSpriteId(i) = SPRITE_NONE;
    }

    sTransitionData->BLDCNT = (BLDCNT_TGT1_BG0 | BLDCNT_EFFECT_BLEND | BLDCNT_TGT2_ALL) & ~(BLDCNT_TGT2_BG0);
    sTransitionData->BLDALPHA = BLDALPHA_BLEND(task->tBlendTarget1, task->tBlendTarget2);
    sTransitionData->BLDY = 0;
    REG_BLDCNT = sTransitionData->BLDCNT;
    REG_BLDALPHA = sTransitionData->BLDALPHA;
    REG_BLDY = sTransitionData->BLDY;

    SetVBlankCallback(VBlankCB_SetBlend);
    task->tState++;
    return FALSE;
}

static bool8 RegisOras_FramesCountdown(struct Task *task)
{
    if (--task->tEndDelay == 0)
        task->tState++;
    return FALSE;
}

static bool8 RegisOras_DoBlendFade(struct Task *task)
{
    task->tBlendTarget1 += 1;
    task->tBlendTarget2 -= 1;
    sTransitionData->BLDALPHA = BLDALPHA_BLEND(task->tBlendTarget1, task->tBlendTarget2);

    if (task->tBlendTarget2 == 0)
    {
        task->tState++;
        return TRUE;
    }

    return FALSE;
}

static bool8 RegisOras_SetupEyeFade0(struct Task *task)
{
    u16 *tilemap, *tileset;
    GetBg0TilesDst(&tilemap, &tileset);
    RegisOras_DrawEye(
        tilemap,
        regi_params[task->tPatternIndex].eye_pattern[0],
        1 | (0xF << 12));

    task->tBlendTarget1 = 0;
    task->tBlendTarget2 = 16;
    task->tState++;
    return FALSE;
}

static bool8 RegisOras_EndEyeFade0(struct Task *task)
{
    task->tSpriteId(0) = CreateSprite(
        &sSpriteTemplate_EyeGem,
        16 + 8 * regi_params[task->tPatternIndex].eye_pattern[0].x,
        16 + 8 * regi_params[task->tPatternIndex].eye_pattern[0].y,
        1);
    StartSpriteAnim(&gSprites[task->tSpriteId(0)], 0);

    task->tBlendTarget1 = 0;
    task->tBlendTarget2 = 16;
    sTransitionData->BLDALPHA = BLDALPHA_BLEND(task->tBlendTarget1, task->tBlendTarget2);
    task->tState++;

    return FALSE;
}

static bool8 RegisOras_SetupEyeFade1(struct Task *task)
{
    u16 *tilemap, *tileset;

    GetBg0TilesDst(&tilemap, &tileset);
    RegisOras_DrawEye(
        tilemap,
        regi_params[task->tPatternIndex].eye_pattern[0],
        (1+16) | (0xF << 12));

    task->tState++;
    return FALSE;
}

static bool8 RegisOras_EndEyeFade1(struct Task *task)
{
    StartSpriteAnim(&gSprites[task->tSpriteId(0)], 1);

    task->tBlendTarget1 = 0;
    task->tBlendTarget2 = 16;
    sTransitionData->BLDALPHA = BLDALPHA_BLEND(task->tBlendTarget1, task->tBlendTarget2);
    task->tState++;

    return FALSE;
}

static bool8 RegisOras_SetupEyeFade2(struct Task *task)
{
    u16 *tilemap, *tileset;

    GetBg0TilesDst(&tilemap, &tileset);
    RegisOras_ClearEye(
        tilemap,
        regi_params[task->tPatternIndex].eye_pattern[0]);
    RegisOras_DrawEye(
        tilemap,
        regi_params[task->tPatternIndex].eye_pattern[1],
        1 | (0xF << 12));

    task->tState++;
    return FALSE;
}

static bool8 RegisOras_EndEyeFade2(struct Task *task)
{
    task->tSpriteId(1) = CreateSprite(
        &sSpriteTemplate_EyeGem,
        16 + 8 * regi_params[task->tPatternIndex].eye_pattern[1].x,
        16 + 8 * regi_params[task->tPatternIndex].eye_pattern[1].y,
        1);

    task->tBlendTarget1 = 0;
    task->tBlendTarget2 = 16;
    sTransitionData->BLDALPHA = BLDALPHA_BLEND(task->tBlendTarget1, task->tBlendTarget2);
    task->tState++;

    return FALSE;
}

static bool8 RegisOras_SetupEyeFade3(struct Task *task)
{
    u16 *tilemap, *tileset;

    GetBg0TilesDst(&tilemap, &tileset);
    RegisOras_DrawEye(
        tilemap,
        regi_params[task->tPatternIndex].eye_pattern[1],
        (1+16) | (0xF << 12));
    RegisOras_DrawEye(
        tilemap,
        regi_params[task->tPatternIndex].eye_pattern[2],
        1 | (0xF << 12));

    task->tState++;
    return FALSE;
}

static bool8 RegisOras_EndEyeFade3(struct Task *task)
{
    task->tSpriteId(2) = CreateSprite(
        &sSpriteTemplate_EyeGem,
        16 + 8 * regi_params[task->tPatternIndex].eye_pattern[2].x,
        16 + 8 * regi_params[task->tPatternIndex].eye_pattern[2].y,
        1);
    StartSpriteAnim(&gSprites[task->tSpriteId(1)], 1);
    StartSpriteAnim(&gSprites[task->tSpriteId(2)], 0);

    task->tBlendTarget1 = 0;
    task->tBlendTarget2 = 16;
    sTransitionData->BLDALPHA = BLDALPHA_BLEND(task->tBlendTarget1, task->tBlendTarget2);
    task->tState++;

    return FALSE;
}

static bool8 RegisOras_SetupEyeFade4(struct Task *task)
{
    u16 *tilemap, *tileset;

    GetBg0TilesDst(&tilemap, &tileset);
    RegisOras_ClearEye(
        tilemap,
        regi_params[task->tPatternIndex].eye_pattern[1]);
    RegisOras_DrawEye(
        tilemap,
        regi_params[task->tPatternIndex].eye_pattern[2],
        (1+16) | (0xF << 12));
    RegisOras_DrawEye(
        tilemap,
        regi_params[task->tPatternIndex].eye_pattern[3],
        1 | (0xF << 12));
    RegisOras_DrawEye(
        tilemap,
        regi_params[task->tPatternIndex].eye_pattern[4],
        1 | (0xF << 12));

    task->tState++;
    return FALSE;
}

static bool8 RegisOras_EndEyeFade4(struct Task *task)
{
    task->tSpriteId(3) = CreateSprite(
        &sSpriteTemplate_EyeGem,
        16 + 8 * regi_params[task->tPatternIndex].eye_pattern[3].x,
        16 + 8 * regi_params[task->tPatternIndex].eye_pattern[3].y,
        1);
    task->tSpriteId(4) = CreateSprite(
        &sSpriteTemplate_EyeGem,
        16 + 8 * regi_params[task->tPatternIndex].eye_pattern[4].x,
        16 + 8 * regi_params[task->tPatternIndex].eye_pattern[4].y,
        1);
    StartSpriteAnim(&gSprites[task->tSpriteId(2)], 1);
    StartSpriteAnim(&gSprites[task->tSpriteId(3)], 0);
    StartSpriteAnim(&gSprites[task->tSpriteId(4)], 0);

    task->tBlendTarget1 = 0;
    task->tBlendTarget2 = 16;
    sTransitionData->BLDALPHA = BLDALPHA_BLEND(task->tBlendTarget1, task->tBlendTarget2);
    task->tState++;

    return FALSE;
}

static bool8 RegisOras_SetupEyeFade5(struct Task *task)
{
    u16 *tilemap, *tileset;

    GetBg0TilesDst(&tilemap, &tileset);
    RegisOras_ClearEye(
        tilemap,
        regi_params[task->tPatternIndex].eye_pattern[2]);
    RegisOras_DrawEye(
        tilemap,
        regi_params[task->tPatternIndex].eye_pattern[3],
        (1+16) | (0xF << 12));
    RegisOras_DrawEye(
        tilemap,
        regi_params[task->tPatternIndex].eye_pattern[4],
        (1+16) | (0xF << 12));
    RegisOras_DrawEye(
        tilemap,
        regi_params[task->tPatternIndex].eye_pattern[5],
        1 | (0xF << 12));
    RegisOras_DrawEye(
        tilemap,
        regi_params[task->tPatternIndex].eye_pattern[6],
        1 | (0xF << 12));

    task->tState++;
    return FALSE;
}

static bool8 RegisOras_EndEyeFade5(struct Task *task)
{
    task->tSpriteId(5) = CreateSprite(
        &sSpriteTemplate_EyeGem,
        16 + 8 * regi_params[task->tPatternIndex].eye_pattern[5].x,
        16 + 8 * regi_params[task->tPatternIndex].eye_pattern[5].y,
        1);
    task->tSpriteId(6) = CreateSprite(
        &sSpriteTemplate_EyeGem,
        16 + 8 * regi_params[task->tPatternIndex].eye_pattern[6].x,
        16 + 8 * regi_params[task->tPatternIndex].eye_pattern[6].y,
        1);
    StartSpriteAnim(&gSprites[task->tSpriteId(3)], 1);
    StartSpriteAnim(&gSprites[task->tSpriteId(4)], 1);
    StartSpriteAnim(&gSprites[task->tSpriteId(5)], 0);
    StartSpriteAnim(&gSprites[task->tSpriteId(6)], 0);

    task->tBlendTarget1 = 0;
    task->tBlendTarget2 = 16;
    sTransitionData->BLDALPHA = BLDALPHA_BLEND(task->tBlendTarget1, task->tBlendTarget2);
    task->tState++;

    return FALSE;
}

static bool8 RegisOras_SetupEyeFade6(struct Task *task)
{
    u16 *tilemap, *tileset;

    GetBg0TilesDst(&tilemap, &tileset);
    RegisOras_ClearEye(
        tilemap,
        regi_params[task->tPatternIndex].eye_pattern[3]);
    RegisOras_ClearEye(
        tilemap,
        regi_params[task->tPatternIndex].eye_pattern[4]);
    RegisOras_DrawEye(
        tilemap,
        regi_params[task->tPatternIndex].eye_pattern[5],
        (1+16) | (0xF << 12));
    RegisOras_DrawEye(
        tilemap,
        regi_params[task->tPatternIndex].eye_pattern[6],
        (1+16) | (0xF << 12));

    task->tState++;
    return FALSE;
}

static bool8 RegisOras_EndEyeFade6(struct Task *task)
{
    StartSpriteAnim(&gSprites[task->tSpriteId(5)], 1);
    StartSpriteAnim(&gSprites[task->tSpriteId(6)], 1);

    task->tBlendTarget1 = 0;
    task->tBlendTarget2 = 16;
    sTransitionData->BLDALPHA = BLDALPHA_BLEND(task->tBlendTarget1, task->tBlendTarget2);
    task->tState++;

    return FALSE;
}

static bool8 RegisOras_SetupEyeFade7(struct Task *task)
{
    u16 *tilemap, *tileset;

    GetBg0TilesDst(&tilemap, &tileset);
    RegisOras_ClearEye(
        tilemap,
        regi_params[task->tPatternIndex].eye_pattern[5]);
    RegisOras_ClearEye(
        tilemap,
        regi_params[task->tPatternIndex].eye_pattern[6]);

    task->tState++;
    return FALSE;
}

static bool8 RegisOras_EndEyeFade7(struct Task *task)
{
    task->tEndDelay = 8;

    task->tBlendTarget1 = 0;
    task->tBlendTarget2 = 16;
    sTransitionData->BLDALPHA = BLDALPHA_BLEND(task->tBlendTarget1, task->tBlendTarget2);
    task->tState++;

    return FALSE;
}

static bool8 RegisOras_SetupFadeToGlow(struct Task *task)
{
    u16 *tilemap, *tileset;
    unsigned i, palettes_to_fade;

    GetBg0TilesDst(&tilemap, &tileset);
    LZ77UnCompVram(sRegisGlow_Tileset, tileset + GLOW_TILES_START * TILE_SIZE_4BPP / sizeof(u16));
    for (i = 0; i < 7; i++)
        RegisOras_DrawEyeGlow(tilemap, regi_params[task->tPatternIndex].eye_pattern[i]);

    palettes_to_fade = PALETTES_ALL & ~(1 << 15) & ~(1 << (16 + task->tSpritePalette));
    BeginNormalPaletteFade(palettes_to_fade, 1, 0, 16, RGB_BLACK);
    task->tState++;
    return FALSE;
}

static bool8 RegisOras_DoFadeToGlow(struct Task *task)
{
    if (task->tBlendTarget1 < 16)
    {
        if (task->tBlendDelay)
            task->tBlendDelay -= 1;
        else
        {
            task->tBlendTarget1 += 1;
            task->tBlendDelay = 1;
            sTransitionData->BLDALPHA = BLDALPHA_BLEND(task->tBlendTarget1, task->tBlendTarget2);
        }
    }
    else
    {
        if (!gPaletteFade.active)
        {
            task->tEndDelay = 6;
            task->tState++;
        }
    }
    return FALSE;
}

static bool8 RegisOras_SetupFadeToBlack(struct Task *task)
{
    unsigned palettes_to_fade = (1 << 15) | (1 << (16 + task->tSpritePalette));
    BeginNormalPaletteFade(palettes_to_fade, 1, 0, 16, RGB_BLACK);
    task->tState++;
    return FALSE;
}

static bool8 RegisOras_DoFadeToBlack(struct Task *task)
{
    unsigned i;
    if (!gPaletteFade.active)
    {
        for (i = 0; i < 7; i++)
        {
            DestroySprite(gSprites + task->tSpriteId(i));
        }
        FadeScreenBlack();
        DestroyTask(FindTaskIdByFunc(task->func));
    }

    return FALSE;
}
