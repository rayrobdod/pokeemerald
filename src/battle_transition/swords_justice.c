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
static bool8 SwordsOfJustice_Slash0_SetGfx(struct Task *task);
static bool8 SwordsOfJustice_Slash1_SetGfx(struct Task *task);
static bool8 SwordsOfJustice_Slash2_SetGfx(struct Task *task);
static bool8 SwordsOfJustice_AnimateSlash(struct Task *task);
static bool8 SwordsOfJustice_End(struct Task *);

static const u32 sSwordsOfJustice_Slash0_Tileset[] = INCBIN_U32("graphics/battle_transitions/swords_justice_slash_page_0.8bpp.lz");
static const u32 sSwordsOfJustice_Slash1_Tileset[] = INCBIN_U32("graphics/battle_transitions/swords_justice_slash_page_1.8bpp.lz");
static const u32 sSwordsOfJustice_Slash2_Tileset[] = INCBIN_U32("graphics/battle_transitions/swords_justice_slash_page_2.8bpp.lz");
static const u32 sSwordsOfJustice_Slash0_Tilemap[] = INCBIN_U32("graphics/battle_transitions/swords_justice_slash_page_0.tilemap.lz");
static const u32 sSwordsOfJustice_Slash1_Tilemap[] = INCBIN_U32("graphics/battle_transitions/swords_justice_slash_page_1.tilemap.lz");
static const u32 sSwordsOfJustice_Slash2_Tilemap[] = INCBIN_U32("graphics/battle_transitions/swords_justice_slash_page_2.tilemap.lz");

static const u16 sSwordsOfJustice_Slash0_Palette[] = INCBIN_U16("graphics/battle_transitions/swords_justice_slash_anim_0.gbapal");
static const u16 sSwordsOfJustice_Slash1_Palette[] = INCBIN_U16("graphics/battle_transitions/swords_justice_slash_anim_1.gbapal");
static const u16 sSwordsOfJustice_Slash2_Palette[] = INCBIN_U16("graphics/battle_transitions/swords_justice_slash_anim_2.gbapal");
static const u16 sSwordsOfJustice_Slash3_Palette[] = INCBIN_U16("graphics/battle_transitions/swords_justice_slash_anim_3.gbapal");
static const u16 sSwordsOfJustice_Slash4_Palette[] = INCBIN_U16("graphics/battle_transitions/swords_justice_slash_anim_4.gbapal");

#define tSlashFrame data[2]
#define tBlend data[2]

static const TransitionStateFunc sSwordsOfJustice_Funcs[] = {
    SwordsOfJustice_Init,
    SwordsOfJustice_FadeToBlack,
    SwordsOfJustice_Slash0_SetGfx,
    SwordsOfJustice_AnimateSlash,
    SwordsOfJustice_Slash1_SetGfx,
    SwordsOfJustice_AnimateSlash,
    SwordsOfJustice_Slash2_SetGfx,
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
    u16 *tilemap, *tileset;
    InitTransitionData();
    ScanlineEffect_Clear();

    REG_WININ = WININ_WIN0_ALL;
    REG_WINOUT = 0;
    REG_WIN0H = DISPLAY_WIDTH;
    REG_WIN0V = DISPLAY_HEIGHT;

    task->tState++;
    task->tBlend = 0;
    return FALSE;
}

static bool8 SwordsOfJustice_AnimateSlash(struct Task *task)
{
    // The slash animation doesn't need palette fading,
    // and a todo is to fade back to the overworld with an overlaid screen-shattered overlay,
    // so preserve `gPlttBufferUnfaded` by writing to `gPlttBufferFaded` directly instead of calling `LoadPalette`
    switch (task->tSlashFrame) {
        case 4:
            CpuCopy16(sSwordsOfJustice_Slash0_Palette, &gPlttBufferFaded[1], sizeof(sSwordsOfJustice_Slash0_Palette));
            break;
        case 7:
            CpuCopy16(sSwordsOfJustice_Slash1_Palette, &gPlttBufferFaded[1], sizeof(sSwordsOfJustice_Slash1_Palette));
            break;
        case 10:
            CpuCopy16(sSwordsOfJustice_Slash2_Palette, &gPlttBufferFaded[1], sizeof(sSwordsOfJustice_Slash2_Palette));
            break;
        case 14:
            CpuCopy16(sSwordsOfJustice_Slash3_Palette, &gPlttBufferFaded[1], sizeof(sSwordsOfJustice_Slash3_Palette));
            break;
        case 17:
            CpuCopy16(sSwordsOfJustice_Slash4_Palette, &gPlttBufferFaded[1], sizeof(sSwordsOfJustice_Slash4_Palette));
            break;
        case 20:
            CpuFill16(RGB_BLACK, &gPlttBufferFaded[1], sizeof(sSwordsOfJustice_Slash4_Palette));
            break;
        case 24:
            task->tState++;
            break;
        default:
            break;
    }
    task->tSlashFrame++;
    return FALSE;
}

static bool8 SwordsOfJustice_Slash0_SetGfx(struct Task *task)
{
    u16 *tilemap, *tileset;

    GetBg0TilesDst(&tilemap, &tileset);
    LZ77UnCompVram(sSwordsOfJustice_Slash0_Tileset, tileset);
    LZ77UnCompVram(sSwordsOfJustice_Slash0_Tilemap, tilemap);

    task->tState++;
    task->tSlashFrame = 0;
    return FALSE;
}

static bool8 SwordsOfJustice_Slash1_SetGfx(struct Task *task)
{
    u16 *tilemap, *tileset;

    GetBg0TilesDst(&tilemap, &tileset);
    LZ77UnCompVram(sSwordsOfJustice_Slash1_Tileset, tileset);
    LZ77UnCompVram(sSwordsOfJustice_Slash1_Tilemap, tilemap);

    task->tState++;
    task->tSlashFrame = 0;
    return FALSE;
}

static bool8 SwordsOfJustice_Slash2_SetGfx(struct Task *task)
{
    u16 *tilemap, *tileset;

    GetBg0TilesDst(&tilemap, &tileset);
    LZ77UnCompVram(sSwordsOfJustice_Slash2_Tileset, tileset);
    LZ77UnCompVram(sSwordsOfJustice_Slash2_Tilemap, tilemap);

    task->tState++;
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
