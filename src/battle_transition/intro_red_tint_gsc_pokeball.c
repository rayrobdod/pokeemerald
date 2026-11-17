#include "global.h"
#include "battle_transition.h"
#include "battle_transition_shared.h"
#include "battle_transition_yoshord.h"
#include "field_camera.h"
#include "field_weather.h"
#include "gpu_regs.h"
#include "palette.h"
#include "scanline_effect.h"
#include "task.h"
#include "constants/rgb.h"

#define FADE_INCREMENT (8)
#define FADE_DARKEST (-32)
#define FADE_LIGHTEST (32)
#define tBlend data[1]

static bool8 IntroWithRedTintAndGscPokeball_LoadTileset(struct Task *);
static bool8 IntroWithRedTintAndGscPokeball_LoadTilemap(struct Task *);
static bool8 IntroWithRedTintAndGscPokeball_FadeJustTint(struct Task *);
static bool8 IntroWithRedTintAndGscPokeball_FadeDarken(struct Task *);
static bool8 IntroWithRedTintAndGscPokeball_FadeLighten(struct Task *);
static bool8 IntroWithRedTintAndGscPokeball_FadeDarkenToZero(struct Task *);
static bool8 IntroWithRedTintAndGscPokeball_CopyPlttBufferFadedToUnfaded(struct Task *);

static const u16 sGscPokeball_Palette[] = INCGFX_U16("graphics/battle_transitions/gsc_pokeball_tile.png", ".gbapal");
static const ALIGNED(4) u16 sGscPokeball_Tileset[] = INCGFX_U16("graphics/battle_transitions/gsc_pokeball_tile.png", ".4bpp");

#define TILEMAP_OP(x, dy, width) (x << 3 | dy << 2 | (width / 2 - 1))
#define TILEMAP_OP_X(op) (op >> 3)
#define TILEMAP_OP_DY(op) (op & 4 ? 1 : 0)
#define TILEMAP_OP_WIDTH(op) (((op & 3) + 1) * 2)
static const u8 sGscPokeball_Tilemap[] = {
    TILEMAP_OP(13, 0, 4),
    TILEMAP_OP(11, 1, 8),
    TILEMAP_OP( 9, 1, 4),
    TILEMAP_OP(17, 0, 4),
    TILEMAP_OP( 9, 1, 2),
    TILEMAP_OP(19, 0, 2),
    TILEMAP_OP( 8, 1, 2),
    TILEMAP_OP(20, 0, 2),
    TILEMAP_OP( 8, 1, 2),
    TILEMAP_OP(20, 0, 2),
    TILEMAP_OP(13, 0, 4),
    TILEMAP_OP(12, 1, 2),
    TILEMAP_OP(16, 0, 2),
    TILEMAP_OP( 7, 0, 2),
    TILEMAP_OP(21, 0, 2),
    TILEMAP_OP( 7, 1, 6),
    TILEMAP_OP(17, 0, 6),

    TILEMAP_OP( 7, 1, 6),
    TILEMAP_OP(17, 0, 6),
    TILEMAP_OP(12, 1, 2),
    TILEMAP_OP(16, 0, 2),
    TILEMAP_OP( 7, 0, 2),
    TILEMAP_OP(21, 0, 2),
    TILEMAP_OP( 8, 1, 2),
    TILEMAP_OP(20, 0, 2),
    TILEMAP_OP(13, 0, 4),
    TILEMAP_OP( 8, 1, 2),
    TILEMAP_OP(20, 0, 2),
    TILEMAP_OP( 9, 1, 2),
    TILEMAP_OP(19, 0, 2),
    TILEMAP_OP( 9, 1, 4),
    TILEMAP_OP(17, 0, 4),
    TILEMAP_OP(11, 1, 8),
    TILEMAP_OP(13, 1, 4),
};

static const TransitionStateFunc sIntroWithRedTintAndGscPokeball_Funcs[] = {
    Intro_Init,
    Intro_ClearBg0,
    IntroWithRedTintAndGscPokeball_LoadTileset,
    IntroWithRedTintAndGscPokeball_FadeJustTint,
    IntroWithRedTintAndGscPokeball_LoadTilemap,
    IntroWithRedTintAndGscPokeball_FadeDarken,
    IntroWithRedTintAndGscPokeball_FadeLighten,
    IntroWithRedTintAndGscPokeball_FadeDarken,
    IntroWithRedTintAndGscPokeball_FadeLighten,
    IntroWithRedTintAndGscPokeball_FadeDarken,
    IntroWithRedTintAndGscPokeball_FadeLighten,
    IntroWithRedTintAndGscPokeball_FadeDarkenToZero,
    IntroWithRedTintAndGscPokeball_CopyPlttBufferFadedToUnfaded,
    Intro_End,
};

void Task_IntroWithRedTintAndGscPokeball(u8 taskId)
{
    while (sIntroWithRedTintAndGscPokeball_Funcs[gTasks[taskId].tState](&gTasks[taskId]));
}

static bool8 IntroWithRedTintAndGscPokeball_LoadTileset(struct Task *task)
{
    u16 charBase;
    u16 *tileset;

    LoadPalette(sGscPokeball_Palette, BG_PLTT_ID(15), sizeof(sGscPokeball_Palette));

    charBase = REG_BG1CNT >> 2;
    charBase <<= 14;
    charBase += TILE_OFFSET_4BPP(1023);
    tileset = (u16 *)(BG_VRAM + charBase);

    CpuFastCopy(sGscPokeball_Tileset, tileset, TILE_SIZE_4BPP);

    task->tState++;
    return TRUE;
}

static bool8 IntroWithRedTintAndGscPokeball_LoadTilemap(struct Task *task)
{
    s16 camera_x, camera_y;
    s32 screen_x, screen_y;
    u32 target_x, target_y;
    u32 tile_hofs, tile_vofs;
    u32 width;
    volatile u16 *tilemap1;
    volatile u16 *tilemap2;
    unsigned op_i, x_i;

    GetCameraOffsetWithPan(&camera_x, &camera_y);

    tilemap1 = (vu16 *)(BG_VRAM + (((REG_BG1CNT) >> 8) << 11));
    tilemap2 = (vu16 *)(BG_VRAM + (((REG_BG2CNT) >> 8) << 11));

    tile_hofs = camera_x / TILE_WIDTH;
    tile_vofs = camera_y / TILE_HEIGHT;

    screen_y = 2;

    for (op_i = 0; op_i < ARRAY_COUNT(sGscPokeball_Tilemap); op_i++)
    {
        screen_x = TILEMAP_OP_X(sGscPokeball_Tilemap[op_i]);
        screen_y += TILEMAP_OP_DY(sGscPokeball_Tilemap[op_i]);

        target_y = (screen_y + tile_vofs) % 32;

        width = TILEMAP_OP_WIDTH(sGscPokeball_Tilemap[op_i]);
        for (x_i = 0; x_i < width; x_i++)
        {
            target_x = (screen_x + tile_hofs + x_i) % 32;

            tilemap1[target_y * 32 + target_x] = 0x0000;
            tilemap2[target_y * 32 + target_x] = 0xF3FF;
        }
    }
    task->tState++;
    return TRUE;
}

static void IntroWithRedTintAndGscPokeball_LightenPallettes(s16 coeff)
{
    u16 color;
    unsigned i;
    int sum;
    int r, g, b;

    for (i = 0; i < 16 * 16 * 2; ++i)
    {
        color = gPlttBufferUnfaded[i];
        r = GET_R(color) / 2;
        g = GET_G(color);
        b = GET_B(color) / 2;

        sum = r + g + b + coeff;
        if (sum < 0)
            gPlttBufferFaded[i] = _RGB(0, 0, 0);
        else if (sum < 32)
            gPlttBufferFaded[i] = _RGB(sum, 0, 0);
        else if (sum < 31 * 2)
            gPlttBufferFaded[i] = _RGB(31, (sum - 31), (sum - 31));
        else
            gPlttBufferFaded[i] = _RGB(31, 31, 31);
    }
}

static bool8 IntroWithRedTintAndGscPokeball_FadeJustTint(struct Task *task)
{
    task->tState++;
    IntroWithRedTintAndGscPokeball_LightenPallettes(task->tBlend);

    return FALSE;
}

static bool8 IntroWithRedTintAndGscPokeball_FadeDarken(struct Task *task)
{
    task->tBlend -= FADE_INCREMENT;
    if (task->tBlend < FADE_DARKEST)
    {
        task->tBlend = FADE_DARKEST;
        task->tState++;
    }
    IntroWithRedTintAndGscPokeball_LightenPallettes(task->tBlend);

    return FALSE;
}

static bool8 IntroWithRedTintAndGscPokeball_FadeLighten(struct Task *task)
{
    task->tBlend += FADE_INCREMENT;
    if (task->tBlend > FADE_LIGHTEST)
    {
        task->tBlend = FADE_LIGHTEST;
        task->tState++;
    }
    IntroWithRedTintAndGscPokeball_LightenPallettes(task->tBlend);

    return FALSE;
}

static bool8 IntroWithRedTintAndGscPokeball_FadeDarkenToZero(struct Task *task)
{
    task->tBlend -= FADE_INCREMENT;
    if (task->tBlend < 0)
    {
        task->tBlend = 0;
        task->tState++;
    }
    IntroWithRedTintAndGscPokeball_LightenPallettes(task->tBlend);

    return FALSE;
}

static bool8 IntroWithRedTintAndGscPokeball_CopyPlttBufferFadedToUnfaded(struct Task *task)
{
    DmaCopy32(0, gPlttBufferFaded, gPlttBufferUnfaded, PLTT_SIZE);

    task->tState++;
    return TRUE;
}
