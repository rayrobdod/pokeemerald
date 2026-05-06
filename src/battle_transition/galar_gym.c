#include "global.h"
#include "battle_transition.h"
#include "battle_transition_shared.h"
#include "battle_transition_tasks/galar_gym.h"
#include "decompress.h"
#include "gpu_regs.h"
#include "palette.h"
#include "main.h"
#include "scanline_effect.h"
#include "task.h"

enum galar_gym_transition_variant
{
    GALAR_GYM_GRASS,
    GALAR_GYM_WATER,
    GALAR_GYM_ROCK,
};

enum effectreg_offset
{
    EFFECTREG_WIN0H_OFFSET = 0,
    EFFECTREG_WIN1H_OFFSET = DISPLAY_HEIGHT + EFFECTREG_WIN0H_OFFSET,
};
#define EFFECTREG_MAXCOPY (DISPLAY_HEIGHT + EFFECTREG_WIN1H_OFFSET)

enum is_done
{
    DONE,
    CONTINUE,
};

enum
{
    LETTERBOX_HEIGHT = 30,
    LETTERBOX_SLOPE_HEIGHT = 6,
    HOLD_ON_PATTERN_FRAMES = 20,
    WIPE_SPEED = 10,
    WIPE_SLOPE = 2,
    LOGO_DISAPPEAR_DELAY = 7,

    PALTAG_GALAR_GYM_LOGO = 0xFA59,
    TILETAG_GALAR_GYM_LOGO = 0xFA59,
};

#define tTopBarPosition data[2]
#define tTopBarVelocity data[3]
#define tBottomBarPosition data[5]
#define tBottomBarVelocity data[6]
#define tSpritePosition data[8]
#define tSpriteOpacity data[10]
#define tVariant data[13]
#define tSprite data[14]
#define tTimer data[15]

static bool8 GalarGym_Init(struct Task *);
static bool8 GalarGym_TerrainDisappear(struct Task *);
static bool8 GalarGym_LoadSprite(struct Task *);
static bool8 GalarGym_LoadPattern1(struct Task *);
static bool8 GalarGym_LoadPattern2(struct Task *);
static bool8 GalarGym_PatternAppear1(struct Task *);
static bool8 GalarGym_PatternAppear2(struct Task *);
static bool8 GalarGym_WaitOnPattern(struct Task *);
static bool8 GalarGym_PatternDisappear(struct Task *);
static bool8 GalarGym_End(struct Task *);

static const struct OamData sOamData_GymLogo =
{
    .y = 0,
    .affineMode = ST_OAM_AFFINE_DOUBLE,
    .objMode = ST_OAM_OBJ_BLEND,
    .mosaic = FALSE,
    .bpp = ST_OAM_4BPP,
    .shape = SPRITE_SHAPE(64x64),
    .x = 0,
    .matrixNum = 0,
    .size = SPRITE_SIZE(64x64),
    .tileNum = 0,
    .priority = 0,
    .paletteNum = 0,
    .affineParam = 0
};

static const union AffineAnimCmd sSpriteAffineAnimTable_GymLogo_Stay[] =
{
    AFFINEANIMCMD_FRAME(0x0, 0x0, 0, 1),
    AFFINEANIMCMD_END,
};

static const union AffineAnimCmd sSpriteAffineAnimTable_GymLogo_Expand[] =
{
    AFFINEANIMCMD_FRAME(6, 6, 0, 1),
    AFFINEANIMCMD_LOOP(12),
    AFFINEANIMCMD_END,
};

const union AffineAnimCmd *const sSpriteAffineAnimTable_GymLogo[] =
{
    sSpriteAffineAnimTable_GymLogo_Stay,
    sSpriteAffineAnimTable_GymLogo_Expand,
};

static const struct SpriteTemplate sSpriteTemplate_GymLogo =
{
    .tileTag = TILETAG_GALAR_GYM_LOGO,
    .paletteTag = PALTAG_GALAR_GYM_LOGO,
    .oam = &sOamData_GymLogo,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .affineAnims = sSpriteAffineAnimTable_GymLogo,
    .callback = SpriteCallbackDummy,
};

struct GalarGymPattern
{
    u8 pattern_width;
    u8 pattern_height;
    s8 pattern_x_offset;
    s8 pattern_y_offset;
    u8 pattern_tilemap_width;
    u8 pattern_tilemap_height;
    const u16 *pattern_palette;
    const u32 *pattern_tileset;
    const u16 *pattern_tilemap;
    const u16 *logo_palette;
    const u32 *logo_tiles;
};

static const u16 sGrassPattern_Palette[] = INCGFX_U16("graphics/battle_transitions/galar_gym_grass_pattern.png", ".gbapal");
static const u32 sGrassPattern_Tileset[] = INCGFX_U32("graphics/battle_transitions/galar_gym_grass_pattern.png", ".4bpp.lz");
static const u16 sGrassPattern_Tilemap[] = INCBIN_U16("graphics/battle_transitions/galar_gym_grass_pattern.bin");
static const u16 sGrassLogo_Palette[] = INCGFX_U16("graphics/battle_transitions/galar_gym_grass_logo.png", ".gbapal");
static const u32 sGrassLogo_Tileset[] = INCGFX_U32("graphics/battle_transitions/galar_gym_grass_logo.png", ".4bpp.lz");

static const u16 sWaterPattern_Palette[] = INCGFX_U16("graphics/battle_transitions/galar_gym_water_pattern.png", ".gbapal");
static const u32 sWaterPattern_Tileset[] = INCGFX_U32("graphics/battle_transitions/galar_gym_water_pattern.png", ".4bpp.lz", "-num_tiles 56");
static const u16 sWaterPattern_Tilemap[] = INCBIN_U16("graphics/battle_transitions/galar_gym_water_pattern.bin");
static const u16 sWaterLogo_Palette[] = INCGFX_U16("graphics/battle_transitions/galar_gym_water_logo.png", ".gbapal");
static const u32 sWaterLogo_Tileset[] = INCGFX_U32("graphics/battle_transitions/galar_gym_water_logo.png", ".4bpp.lz");

static const u16 sRockPattern_Palette[] = INCGFX_U16("graphics/battle_transitions/galar_gym_rock_pattern.png", ".gbapal");
static const u32 sRockPattern_Tileset[] = INCGFX_U32("graphics/battle_transitions/galar_gym_rock_pattern.png", ".4bpp.lz");
static const u16 sRockPattern_Tilemap[] = INCBIN_U16("graphics/battle_transitions/galar_gym_rock_pattern.bin");
static const u16 sRockLogo_Palette[] = INCGFX_U16("graphics/battle_transitions/galar_gym_rock_logo.png", ".gbapal");
static const u32 sRockLogo_Tileset[] = INCGFX_U32("graphics/battle_transitions/galar_gym_rock_logo.png", ".4bpp.lz");

static const struct GalarGymPattern sVariants[] =
{
    [GALAR_GYM_GRASS] =
    {
        .pattern_width = 5,
        .pattern_height = 21,
        .pattern_x_offset = -1,
        .pattern_y_offset = 2,
        .pattern_tilemap_width = 5,
        .pattern_tilemap_height = 23,
        .pattern_palette = sGrassPattern_Palette,
        .pattern_tileset = sGrassPattern_Tileset,
        .pattern_tilemap = sGrassPattern_Tilemap,
        .logo_palette = sGrassLogo_Palette,
        .logo_tiles = sGrassLogo_Tileset,
    },
    [GALAR_GYM_WATER] =
    {
        .pattern_width = 9,
        .pattern_height = 8,
        .pattern_x_offset = 4,
        .pattern_y_offset = -2,
        .pattern_tilemap_width = 9,
        .pattern_tilemap_height = 10,
        .pattern_palette = sWaterPattern_Palette,
        .pattern_tileset = sWaterPattern_Tileset,
        .pattern_tilemap = sWaterPattern_Tilemap,
        .logo_palette = sWaterLogo_Palette,
        .logo_tiles = sWaterLogo_Tileset,
    },
    [GALAR_GYM_ROCK] =
    {
        .pattern_width = 9,
        .pattern_height = 12,
        .pattern_x_offset = 3,
        .pattern_y_offset = 5,
        .pattern_tilemap_width = 12,
        .pattern_tilemap_height = 12,
        .pattern_palette = sRockPattern_Palette,
        .pattern_tileset = sRockPattern_Tileset,
        .pattern_tilemap = sRockPattern_Tilemap,
        .logo_palette = sRockLogo_Palette,
        .logo_tiles = sRockLogo_Tileset,
    },
};

static const TransitionStateFunc sGalarGym_Funcs[] =
{
    GalarGym_Init,
    GalarGym_TerrainDisappear,
    GalarGym_LoadSprite,
    GalarGym_LoadPattern1,
    GalarGym_LoadPattern2,
    GalarGym_PatternAppear1,
    GalarGym_PatternAppear2,
    GalarGym_WaitOnPattern,
    GalarGym_PatternDisappear,
    GalarGym_End,
};

void Task_GalarGym(u8 taskId)
{
    while (sGalarGym_Funcs[gTasks[taskId].tState](&gTasks[taskId]));
}

static int clamp(int min, int value, int max)
{
    if (value < min) return min;
    if (value > max) return max;
    return value;
}

static inline uint16_t pattern_tile_at(const struct GalarGymPattern* pattern, signed x, signed y)
{
    while (1)
    {
        if (x < 0)
        {
            x += pattern->pattern_width;
            y += pattern->pattern_y_offset;
        }
        else if (y < 0)
        {
            y += pattern->pattern_height;
            x += pattern->pattern_x_offset;
        }
        else if (x >= pattern->pattern_tilemap_width)
        {
            x -= pattern->pattern_width;
            y -= pattern->pattern_y_offset;
        }
        else if (y >= pattern->pattern_tilemap_height)
        {
            y -= pattern->pattern_height;
            x -= pattern->pattern_x_offset;
        }
        else
        {
            if (pattern->pattern_tilemap)
                return pattern->pattern_tilemap[y * pattern->pattern_tilemap_width + x];
            else
                return 0xF000 | (y * pattern->pattern_tilemap_width + x);
        }
    }
}

static void GalarGym_AdvanceBottomBar(struct Task *task, enum effectreg_offset effectreg_offset)
{
    int row;
    const int prevBarPosition = task->tBottomBarPosition;
    task->tBottomBarPosition += task->tBottomBarVelocity;

    for (row = max(0, task->tBottomBarPosition + LETTERBOX_SLOPE_HEIGHT); row < prevBarPosition + LETTERBOX_SLOPE_HEIGHT; row++)
        gScanlineEffectRegBuffers[0][effectreg_offset + row] = 0;

    for (row = max(0, task->tBottomBarPosition); row < DISPLAY_HEIGHT && row < task->tBottomBarPosition + LETTERBOX_SLOPE_HEIGHT; row++)
    {
        gScanlineEffectRegBuffers[0][effectreg_offset + row] =
                ((LETTERBOX_SLOPE_HEIGHT - 1) - (row - task->tBottomBarPosition)) * DISPLAY_WIDTH / LETTERBOX_SLOPE_HEIGHT;
    }
}

static void GalarGym_AdvanceTopBar(struct Task *task)
{
    int row;
    const int prevBarPosition = task->tTopBarPosition;
    task->tTopBarPosition += task->tTopBarVelocity;

    for (row = max(0, task->tTopBarPosition + LETTERBOX_SLOPE_HEIGHT); row < prevBarPosition + LETTERBOX_SLOPE_HEIGHT; row++)
        gScanlineEffectRegBuffers[0][EFFECTREG_WIN1H_OFFSET + row] = DISPLAY_WIDTH;

    for (row = max(0, task->tTopBarPosition); row < DISPLAY_HEIGHT && row < task->tTopBarPosition + LETTERBOX_SLOPE_HEIGHT; row++)
    {
        gScanlineEffectRegBuffers[0][EFFECTREG_WIN1H_OFFSET + row] = WIN_RANGE(
                ((LETTERBOX_SLOPE_HEIGHT - 1) - (row - task->tTopBarPosition)) * DISPLAY_WIDTH / LETTERBOX_SLOPE_HEIGHT,
                DISPLAY_WIDTH);
    }
}

static void GalarGym_AdvanceLetterboxVelocity(struct Task *task, bool8 bottom_bar)
{
    const int TOP_BAR_STOP = LETTERBOX_HEIGHT - LETTERBOX_SLOPE_HEIGHT / 2;
    const int BOT_BAR_STOP = DISPLAY_HEIGHT - TOP_BAR_STOP;

    if (task->tTopBarPosition < TOP_BAR_STOP)
        task->tTopBarVelocity = 0;
    else if (task->tTopBarPosition < TOP_BAR_STOP + 1 * 2)
        task->tTopBarVelocity = -1;
    else if (task->tTopBarPosition < TOP_BAR_STOP + (1+2) * 2)
        task->tTopBarVelocity = -2;
    else if (task->tTopBarPosition < TOP_BAR_STOP + (1+2+3) * 2)
        task->tTopBarVelocity = -3;
    else if (task->tTopBarPosition < TOP_BAR_STOP + (1+2+3+4) * 2)
        task->tTopBarVelocity = -4;
    else if (task->tTopBarPosition < TOP_BAR_STOP + (1+2+3+4+5) * 2)
        task->tTopBarVelocity = -5;
    else if (task->tTopBarPosition < TOP_BAR_STOP + (1+2+3+4+5+6) * 2)
        task->tTopBarVelocity = -6;
    else if (task->tTopBarPosition < TOP_BAR_STOP + (1+2+3+4+5+6+7) * 2)
        task->tTopBarVelocity = -7;
    else
        task->tTopBarVelocity = -8;

    if (bottom_bar)
    {
        if (task->tBottomBarPosition < BOT_BAR_STOP)
            task->tBottomBarVelocity = 0;
        else if (task->tBottomBarPosition < BOT_BAR_STOP + 1 * 1)
            task->tBottomBarVelocity = -1;
        else if (task->tBottomBarPosition < BOT_BAR_STOP + 1*1 + 2*2)
            task->tBottomBarVelocity = -2;
        else if (task->tBottomBarPosition < BOT_BAR_STOP + 1*1 + 2*2 + 3*3)
            task->tBottomBarVelocity = -3;
        else if (task->tTopBarPosition < TOP_BAR_STOP + LETTERBOX_HEIGHT)
            task->tBottomBarVelocity = -4;
    }
}

static void GalarGym_AdvanceLogoAppear(struct Task *task)
{
    if (task->tSpriteOpacity * 2 / 3 < 16)
    {
        int hwvalue;
        ++task->tSpriteOpacity;

        hwvalue = clamp(0, task->tSpriteOpacity * 2 / 3, 16);

        REG_BLDALPHA = BLDALPHA_BLEND(hwvalue, 16 - hwvalue);
    }

    if (task->tSpritePosition < 80)
    {
        // do nothing
    }
    else
    {
        if (task->tSpritePosition < 80 + 1 * 2)
            task->tSpritePosition -= 1;
        else if (task->tSpritePosition < 80 + (1 + 2) * 2)
            task->tSpritePosition -= 2;
        else if (task->tSpritePosition < 80 + (1+2+3) * 2)
            task->tSpritePosition -= 3;
        else if (task->tSpritePosition < 80 + (1+2+3+4) * 2)
            task->tSpritePosition -= 4;
        else
            task->tSpritePosition -= 5;

        gSprites[task->tSprite].y = task->tSpritePosition;
    }
}

static void GalarGym_AdvanceLogoDisappear(struct Task *task)
{
    if (task->tTimer == LOGO_DISAPPEAR_DELAY)
    {
        StartSpriteAffineAnim(&gSprites[task->tSprite], 1);
    }

    if (task->tTimer > LOGO_DISAPPEAR_DELAY)
    {
        if (task->tSpriteOpacity * 2 / 3 < 16)
        {
            int hwvalue;
            ++task->tSpriteOpacity;

            hwvalue = clamp(0, task->tSpriteOpacity * 2 / 3, 16);

            REG_BLDALPHA = BLDALPHA_BLEND(16 - hwvalue, hwvalue);
        }

        if (task->tSpritePosition > 80 + (1 + 2) * 4)
            task->tSpritePosition += 3;
        else if (task->tSpritePosition > 80 + (1) * 4)
            task->tSpritePosition += 2;
        else
            task->tSpritePosition += 1;

        gSprites[task->tSprite].x = task->tSpritePosition;
    }
}

static void GalarGym_ScrollPattern(struct Task *task)
{
    static const int VOFS_VELOCITY = Q_8_8(1.0 / 3);
    static const int HOFS_VELOCITY = -Q_8_8(2.0 / 3);

    signed hofsmod = ((sTransitionData->BG0HOFS_Lower % Q_8_8(8)) + Q_8_8(8)) % Q_8_8(8);

    if (hofsmod >= 0 && 0 > hofsmod + HOFS_VELOCITY)
    {
        u16 *tilemap, *tileset;
        int y;
        int logical_x = Q_8_8_TO_INT(sTransitionData->BG0HOFS_Lower + HOFS_VELOCITY) / 8 - 1;
        int physical_x = ((logical_x % 32) + 32) % 32;

        GetBg0TilesDst(&tilemap, &tileset);
        for (y = 0; y < 32; y++)
        {
            tilemap[y * 32 + physical_x] = pattern_tile_at(&sVariants[task->tVariant], logical_x, y);
        }
    }

    sTransitionData->BG0HOFS_Lower += HOFS_VELOCITY;
    sTransitionData->BG0VOFS += VOFS_VELOCITY;
}

static enum is_done GalarGym_AdvanceDiagonalWipe(struct Task *task, int frame)
{
    unsigned row;
    int left, right, wipe;
    for (row = 0; row < DISPLAY_HEIGHT; row++)
    {
        right = gScanlineEffectRegBuffers[0][EFFECTREG_WIN1H_OFFSET + row];
        left = (right >> 8) & 0xFF;
        right &= 0xFF;

        wipe = frame * WIPE_SPEED - row / WIPE_SLOPE;

        left = clamp(left, wipe, DISPLAY_WIDTH);
        right = clamp(right, wipe, DISPLAY_WIDTH);

        gScanlineEffectRegBuffers[0][EFFECTREG_WIN1H_OFFSET + row] = WIN_RANGE(left, right);
    }

    return (frame >= (DISPLAY_WIDTH + (DISPLAY_HEIGHT / WIPE_SLOPE)) / WIPE_SPEED ? DONE : CONTINUE);
}

static void VBlankCB_GalarGym(void)
{
    VBlankCB_BattleTransition();

    if (sTransitionData->VBlank_DMA)
    {
        DmaCopy16(3, gScanlineEffectRegBuffers[0], gScanlineEffectRegBuffers[1], EFFECTREG_MAXCOPY * 2);
    }

    REG_BG0HOFS = Q_8_8_TO_INT(sTransitionData->BG0HOFS_Lower);
    REG_BG0VOFS = Q_8_8_TO_INT(sTransitionData->BG0VOFS);
    REG_WIN0H = gScanlineEffectRegBuffers[1][EFFECTREG_WIN0H_OFFSET];
    REG_WIN1H = gScanlineEffectRegBuffers[1][EFFECTREG_WIN1H_OFFSET];
}

static void HBlankCB_GalarGym(void)
{
    if (REG_VCOUNT < DISPLAY_HEIGHT)
    {
        REG_WIN0H = gScanlineEffectRegBuffers[1][REG_VCOUNT + EFFECTREG_WIN0H_OFFSET];
        REG_WIN1H = gScanlineEffectRegBuffers[1][REG_VCOUNT + EFFECTREG_WIN1H_OFFSET];
    }
}

static bool8 GalarGym_Init(struct Task *task)
{
    InitTransitionData();
    ScanlineEffect_Clear();

    DmaFill16(3, DISPLAY_WIDTH, gScanlineEffectRegBuffers[0] + EFFECTREG_WIN0H_OFFSET, DISPLAY_HEIGHT * sizeof(u16));
    DmaFill16(3, 0, gScanlineEffectRegBuffers[0] + EFFECTREG_WIN1H_OFFSET, DISPLAY_HEIGHT * sizeof(u16));
    DmaCopy16(3, gScanlineEffectRegBuffers[0], gScanlineEffectRegBuffers[1], EFFECTREG_MAXCOPY * 2);

    REG_WININ = WININ_WIN0_ALL;
    REG_WINOUT = 0;
    REG_WIN0V = DISPLAY_HEIGHT;
    REG_WIN1V = DISPLAY_HEIGHT;

    SetVBlankCallback(VBlankCB_GalarGym);
    SetHBlankCallback(HBlankCB_GalarGym);
    EnableInterrupts(INTR_FLAG_VBLANK | INTR_FLAG_HBLANK);

    task->tVariant = GALAR_GYM_ROCK;

    task->tTopBarPosition = 0;
    task->tTopBarVelocity = 0;
    task->tBottomBarPosition = DISPLAY_HEIGHT;
    task->tBottomBarVelocity = -6;
    task->tTimer = 0;
    task->tState++;
    return TRUE;
}

static bool8 GalarGym_TerrainDisappear(struct Task *task)
{
    sTransitionData->VBlank_DMA = FALSE;
    GalarGym_AdvanceBottomBar(task, EFFECTREG_WIN0H_OFFSET);
    sTransitionData->VBlank_DMA = TRUE;

    if (task->tBottomBarPosition <= 90)
    {
        task->tState++;
    }
    return FALSE;
}

static bool8 GalarGym_LoadSprite(struct Task *task)
{
    const struct SpritePalette spritePalette =
    {
        .data = sVariants[task->tVariant].logo_palette,
        .tag = PALTAG_GALAR_GYM_LOGO,
    };
    const struct CompressedSpriteSheet spriteSheet =
    {
        .data = sVariants[task->tVariant].logo_tiles,
        .size = 64 * 64 * 4 / 8,
        .tag = TILETAG_GALAR_GYM_LOGO,
    };

    sTransitionData->VBlank_DMA = FALSE;
    GalarGym_AdvanceBottomBar(task, EFFECTREG_WIN0H_OFFSET);
    sTransitionData->VBlank_DMA = TRUE;

    LoadSpritePalette(&spritePalette);
    LoadCompressedSpriteSheet(&spriteSheet);

    task->tSpritePosition = 160;
    task->tSpriteOpacity = 0;

    task->tSprite = CreateSprite(&sSpriteTemplate_GymLogo, 120, 160, 0);
    REG_BLDCNT = BLDCNT_EFFECT_BLEND | BLDCNT_TGT1_OBJ | BLDCNT_TGT2_BG0;
    REG_BLDALPHA = BLDALPHA_BLEND(0, 16);

    task->tState++;
    return FALSE;
}

static bool8 GalarGym_LoadPattern1(struct Task *task)
{
    unsigned x, y;
    u16 *tilemap, *tileset;

    sTransitionData->VBlank_DMA = FALSE;
    GalarGym_AdvanceBottomBar(task, EFFECTREG_WIN0H_OFFSET);
    sTransitionData->VBlank_DMA = TRUE;

    REG_WININ = (WININ_WIN0_ALL & ~WININ_WIN0_BG0) | WININ_WIN1_BG0 | WININ_WIN1_OBJ;

    GetBg0TilesDst(&tilemap, &tileset);
    LZ77UnCompVram(sVariants[task->tVariant].pattern_tileset, tileset);

    for (y = 0; y < 16; y++)
    {
        for (x = 0; x < DISPLAY_TILE_WIDTH; x++)
        {
            tilemap[y * 32 + x] = pattern_tile_at(&sVariants[task->tVariant], x, y);
        }
    }

    task->tState++;
    return FALSE;
}

static bool8 GalarGym_LoadPattern2(struct Task *task)
{
    unsigned x, y;
    u16 *tilemap, *tileset;

    sTransitionData->VBlank_DMA = FALSE;
    GalarGym_AdvanceBottomBar(task, EFFECTREG_WIN0H_OFFSET);
    sTransitionData->VBlank_DMA = TRUE;

    GetBg0TilesDst(&tilemap, &tileset);
    LoadPalette(sVariants[task->tVariant].pattern_palette, BG_PLTT_ID(15), PLTT_SIZE_4BPP);

    for (y = 16; y < 32; y++)
    {
        for (x = 0; x < DISPLAY_TILE_WIDTH; x++)
        {
            tilemap[y * 32 + x] = pattern_tile_at(&sVariants[task->tVariant], x, y);
        }
    }

    task->tTopBarPosition = DISPLAY_HEIGHT;
    task->tTopBarVelocity = -6;

    task->tState++;
    return FALSE;
}

static bool8 GalarGym_PatternAppear1(struct Task *task)
{
    sTransitionData->VBlank_DMA = FALSE;
    GalarGym_AdvanceBottomBar(task, EFFECTREG_WIN0H_OFFSET);
    GalarGym_AdvanceTopBar(task);
    GalarGym_ScrollPattern(task);
    GalarGym_AdvanceLogoAppear(task);
    sTransitionData->VBlank_DMA = TRUE;

    GalarGym_AdvanceLetterboxVelocity(task, FALSE);

    if (task->tBottomBarPosition < -LETTERBOX_SLOPE_HEIGHT)
    {
        task->tBottomBarPosition = DISPLAY_HEIGHT;
        task->tBottomBarVelocity = 0;
        task->tState++;
    }
    return FALSE;
}

static bool8 GalarGym_PatternAppear2(struct Task *task)
{
    sTransitionData->VBlank_DMA = FALSE;
    GalarGym_AdvanceBottomBar(task, EFFECTREG_WIN1H_OFFSET);
    GalarGym_AdvanceTopBar(task);
    GalarGym_ScrollPattern(task);
    GalarGym_AdvanceLogoAppear(task);
    sTransitionData->VBlank_DMA = TRUE;

    GalarGym_AdvanceLetterboxVelocity(task, TRUE);

    if (task->tTopBarVelocity == 0 && task->tBottomBarVelocity == 0) {
        task->tTimer = -HOLD_ON_PATTERN_FRAMES;
        task->tState++;
    }
    return FALSE;
}

static bool8 GalarGym_WaitOnPattern(struct Task *task)
{
    sTransitionData->VBlank_DMA = FALSE;
    GalarGym_ScrollPattern(task);
    GalarGym_AdvanceLogoAppear(task);
    sTransitionData->VBlank_DMA = TRUE;
    ++task->tTimer;

    if (task->tTimer >= 0) {
        task->tSpritePosition = 120;
        task->tSpriteOpacity = 0;
        task->tState++;
    }
    return FALSE;
}

static bool8 GalarGym_PatternDisappear(struct Task *task)
{
    enum is_done is_done;
    sTransitionData->VBlank_DMA = FALSE;
    GalarGym_ScrollPattern(task);
    GalarGym_AdvanceLogoDisappear(task);
    is_done = GalarGym_AdvanceDiagonalWipe(task, task->tTimer);
    sTransitionData->VBlank_DMA = TRUE;
    ++task->tTimer;

    if (is_done == DONE) {
        task->tState++;
    }
    return FALSE;
}

static bool8 GalarGym_End(struct Task *task)
{
    FadeScreenBlack();
    DestroyTask(FindTaskIdByFunc(task->func));
    return FALSE;
}
