#include "global.h"
#include "battle_transition.h"
#include "battle_transition_shared.h"
#include "battle_transition_tasks/swords_justice.h"
#include "gpu_regs.h"
#include "palette.h"
#include "scanline_effect.h"
#include "task.h"
#include "constants/rgb.h"

static bool8 SwordsOfJustice_Init(struct Task *);
static bool8 SwordsOfJustice_FadeToBlack(struct Task *task);
static bool8 SwordsOfJustice_SetSlashGfx(struct Task *task);
static bool8 SwordsOfJustice_AnimateSlash(struct Task *task);
static bool8 SwordsOfJustice_End(struct Task *);

static const u32 sSwordsOfJustice_Slash0_Tileset[] = INCBIN_U32("graphics/battle_transitions/swords_justice_slash_page_0.8bpp.lz");
static const u32 sSwordsOfJustice_Slash1_Tileset[] = INCBIN_U32("graphics/battle_transitions/swords_justice_slash_page_1.8bpp.lz");
static const u32 sSwordsOfJustice_Slash2_Tileset[] = INCBIN_U32("graphics/battle_transitions/swords_justice_slash_page_2.8bpp.lz");
static const u32 sSwordsOfJustice_Slash0_Tilemap[] = INCBIN_U32("graphics/battle_transitions/swords_justice_slash_page_0.tilemap.rl");
static const u32 sSwordsOfJustice_Slash1_Tilemap[] = INCBIN_U32("graphics/battle_transitions/swords_justice_slash_page_1.tilemap.rl");
static const u32 sSwordsOfJustice_Slash2_Tilemap[] = INCBIN_U32("graphics/battle_transitions/swords_justice_slash_page_2.tilemap.rl");

static const struct { const u32* tileset; const u32* tilemap; } sSwordsOfJustice_Slash_Tiles[] = {
    [0] = {
        .tileset = sSwordsOfJustice_Slash0_Tileset,
        .tilemap = sSwordsOfJustice_Slash0_Tilemap,
    },
    [1] = {
        .tileset = sSwordsOfJustice_Slash1_Tileset,
        .tilemap = sSwordsOfJustice_Slash1_Tilemap,
    },
    [2] = {
        .tileset = sSwordsOfJustice_Slash2_Tileset,
        .tilemap = sSwordsOfJustice_Slash2_Tilemap,
    },
};

#define SLASH_PALETTE_SIZE (51)
#define SLASHFRAMES_PREFIX (4)
#define SLASHFRAMES_INCREASING (7)
#define SLASHFRAMES_PEAK (3)
#define SLASHFRAMES_DECREASING (7)
#define SLASHFRAMES_SUFFIX (3)

static const u16 sSwordsOfJustice_Slash_Increasing_Palettes[SLASHFRAMES_INCREASING][SLASH_PALETTE_SIZE] = {
    INCBIN_U16("graphics/battle_transitions/swords_justice_slash_anim_0.gbapal"),
    INCBIN_U16("graphics/battle_transitions/swords_justice_slash_anim_1.gbapal"),
    INCBIN_U16("graphics/battle_transitions/swords_justice_slash_anim_2.gbapal"),
    INCBIN_U16("graphics/battle_transitions/swords_justice_slash_anim_3.gbapal"),
    INCBIN_U16("graphics/battle_transitions/swords_justice_slash_anim_4.gbapal"),
    INCBIN_U16("graphics/battle_transitions/swords_justice_slash_anim_5.gbapal"),
    INCBIN_U16("graphics/battle_transitions/swords_justice_slash_anim_6.gbapal")
};
static const u16 sSwordsOfJustice_Slash_Peak_Palette[SLASH_PALETTE_SIZE] = INCBIN_U16("graphics/battle_transitions/swords_justice_slash_anim_7.gbapal");
static const u16 sSwordsOfJustice_Slash_Decreasing_Palettes[SLASHFRAMES_DECREASING][SLASH_PALETTE_SIZE] = {
    INCBIN_U16("graphics/battle_transitions/swords_justice_slash_anim_8.gbapal"),
    INCBIN_U16("graphics/battle_transitions/swords_justice_slash_anim_9.gbapal"),
    INCBIN_U16("graphics/battle_transitions/swords_justice_slash_anim_10.gbapal"),
    INCBIN_U16("graphics/battle_transitions/swords_justice_slash_anim_11.gbapal"),
    INCBIN_U16("graphics/battle_transitions/swords_justice_slash_anim_12.gbapal"),
    INCBIN_U16("graphics/battle_transitions/swords_justice_slash_anim_13.gbapal"),
    INCBIN_U16("graphics/battle_transitions/swords_justice_slash_anim_14.gbapal"),
};

#define tSlashFrame data[2]
#define tBlend data[2]
#define tScene data[3]

static const TransitionStateFunc sSwordsOfJustice_Funcs[] = {
    SwordsOfJustice_Init,
    SwordsOfJustice_FadeToBlack,
    SwordsOfJustice_SetSlashGfx,
    SwordsOfJustice_AnimateSlash,
    SwordsOfJustice_SetSlashGfx,
    SwordsOfJustice_AnimateSlash,
    SwordsOfJustice_SetSlashGfx,
    SwordsOfJustice_AnimateSlash,
    SwordsOfJustice_End,
};

void Task_SwordsOfJustice(u8 taskId)
{
    while (sSwordsOfJustice_Funcs[gTasks[taskId].tState](&gTasks[taskId]));
}

static bool8 SwordsOfJustice_FadeToBlack(struct Task *task)
{
    task->tBlend += 2;
    if (task->tBlend > 16)
        task->tBlend = 16;
    BlendPalettes(PALETTES_ALL, task->tBlend, RGB_BLACK);

    if (task->tBlend >= 16)
    {
        SetGpuReg(REG_OFFSET_BG0CNT, REG_BG0CNT | BGCNT_256COLOR);
        task->tState++;
    }
    return FALSE;
}

static bool8 SwordsOfJustice_Init(struct Task *task)
{
    InitTransitionData();
    ScanlineEffect_Clear();

    REG_WININ = WININ_WIN0_ALL;
    REG_WINOUT = 0;
    REG_WIN0H = DISPLAY_WIDTH;
    REG_WIN0V = DISPLAY_HEIGHT;

    task->tState++;
    task->tBlend = 0;
    task->tScene = 0;
    return FALSE;
}

static bool8 SwordsOfJustice_AnimateSlash(struct Task *task)
{
    const u16* palette = NULL;

    u16 slashFrame = task->tSlashFrame;

    if (slashFrame >= SLASHFRAMES_PREFIX)
    {
        slashFrame -= SLASHFRAMES_PREFIX;
        if (slashFrame < SLASHFRAMES_INCREASING)
        {
            palette = sSwordsOfJustice_Slash_Increasing_Palettes[slashFrame];
        }
        else
        {
            slashFrame -= SLASHFRAMES_INCREASING;
            if (slashFrame == 0)
            {
                palette = sSwordsOfJustice_Slash_Peak_Palette;
            }
            else if (slashFrame >= SLASHFRAMES_PEAK)
            {
                slashFrame -= SLASHFRAMES_PEAK;
                if (slashFrame < SLASHFRAMES_DECREASING)
                {
                    palette = sSwordsOfJustice_Slash_Decreasing_Palettes[slashFrame];
                }
                else
                {
                    slashFrame -= SLASHFRAMES_DECREASING;
                    if (slashFrame == 0)
                    {
                        CpuFill16(RGB_BLACK, &gPlttBufferFaded[1], sizeof(u16) * SLASH_PALETTE_SIZE);
                    }
                    else if (slashFrame == SLASHFRAMES_SUFFIX)
                    {
                        task->tState++;
                    }
                }
            }
        }
    }

    if (NULL != palette) {
        // The slash animation doesn't need palette fading,
        // and a todo is to fade back to the overworld with an overlaid screen-shattered overlay,
        // so preserve `gPlttBufferUnfaded` by writing to `gPlttBufferFaded` directly instead of calling `LoadPalette`
        CpuCopy16(palette, &gPlttBufferFaded[1], sizeof(u16) * SLASH_PALETTE_SIZE);
    }
    task->tSlashFrame++;
    return FALSE;
}

static bool8 SwordsOfJustice_SetSlashGfx(struct Task *task)
{
    u16 *tilemap, *tileset;

    GetBg0TilesDst(&tilemap, &tileset);
    LZ77UnCompVram(sSwordsOfJustice_Slash_Tiles[task->tScene].tileset, tileset);
    RLUnCompVram(sSwordsOfJustice_Slash_Tiles[task->tScene].tilemap, tilemap);

    task->tState++;
    task->tScene++;
    task->tSlashFrame = 0;
    return FALSE;
}

static bool8 SwordsOfJustice_End(struct Task *task)
{
    DmaStop(0);
    FadeScreenBlack();
    DestroyTask(FindTaskIdByFunc(task->func));
    return FALSE;
}
