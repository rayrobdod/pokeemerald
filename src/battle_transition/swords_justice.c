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
static const u16 sSwordsOfJustice_Slash5_Palette[] = INCBIN_U16("graphics/battle_transitions/swords_justice_slash_anim_5.gbapal");
static const u16 sSwordsOfJustice_Slash6_Palette[] = INCBIN_U16("graphics/battle_transitions/swords_justice_slash_anim_6.gbapal");
static const u16 sSwordsOfJustice_Slash_Peak_Palette[] = INCBIN_U16("graphics/battle_transitions/swords_justice_slash_anim_7.gbapal");
static const u16 sSwordsOfJustice_Slash8_Palette[] = INCBIN_U16("graphics/battle_transitions/swords_justice_slash_anim_8.gbapal");
static const u16 sSwordsOfJustice_Slash9_Palette[] = INCBIN_U16("graphics/battle_transitions/swords_justice_slash_anim_9.gbapal");
static const u16 sSwordsOfJustice_Slash10_Palette[] = INCBIN_U16("graphics/battle_transitions/swords_justice_slash_anim_10.gbapal");
static const u16 sSwordsOfJustice_Slash11_Palette[] = INCBIN_U16("graphics/battle_transitions/swords_justice_slash_anim_11.gbapal");
static const u16 sSwordsOfJustice_Slash12_Palette[] = INCBIN_U16("graphics/battle_transitions/swords_justice_slash_anim_12.gbapal");
static const u16 sSwordsOfJustice_Slash13_Palette[] = INCBIN_U16("graphics/battle_transitions/swords_justice_slash_anim_13.gbapal");
static const u16 sSwordsOfJustice_Slash14_Palette[] = INCBIN_U16("graphics/battle_transitions/swords_justice_slash_anim_14.gbapal");

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
        case 5:
            CpuCopy16(sSwordsOfJustice_Slash1_Palette, &gPlttBufferFaded[1], sizeof(sSwordsOfJustice_Slash1_Palette));
            break;
        case 6:
            CpuCopy16(sSwordsOfJustice_Slash2_Palette, &gPlttBufferFaded[1], sizeof(sSwordsOfJustice_Slash2_Palette));
            break;
        case 7:
            CpuCopy16(sSwordsOfJustice_Slash3_Palette, &gPlttBufferFaded[1], sizeof(sSwordsOfJustice_Slash3_Palette));
            break;
        case 8:
            CpuCopy16(sSwordsOfJustice_Slash4_Palette, &gPlttBufferFaded[1], sizeof(sSwordsOfJustice_Slash4_Palette));
            break;
        case 9:
            CpuCopy16(sSwordsOfJustice_Slash5_Palette, &gPlttBufferFaded[1], sizeof(sSwordsOfJustice_Slash5_Palette));
            break;
        case 10:
            CpuCopy16(sSwordsOfJustice_Slash6_Palette, &gPlttBufferFaded[1], sizeof(sSwordsOfJustice_Slash6_Palette));
            break;
        case 11:
            CpuCopy16(sSwordsOfJustice_Slash_Peak_Palette, &gPlttBufferFaded[1], sizeof(sSwordsOfJustice_Slash_Peak_Palette));
            break;
        case 14:
            CpuCopy16(sSwordsOfJustice_Slash8_Palette, &gPlttBufferFaded[1], sizeof(sSwordsOfJustice_Slash8_Palette));
            break;
        case 15:
            CpuCopy16(sSwordsOfJustice_Slash9_Palette, &gPlttBufferFaded[1], sizeof(sSwordsOfJustice_Slash9_Palette));
            break;
        case 16:
            CpuCopy16(sSwordsOfJustice_Slash10_Palette, &gPlttBufferFaded[1], sizeof(sSwordsOfJustice_Slash10_Palette));
            break;
        case 17:
            CpuCopy16(sSwordsOfJustice_Slash11_Palette, &gPlttBufferFaded[1], sizeof(sSwordsOfJustice_Slash11_Palette));
            break;
        case 18:
            CpuCopy16(sSwordsOfJustice_Slash12_Palette, &gPlttBufferFaded[1], sizeof(sSwordsOfJustice_Slash12_Palette));
            break;
        case 19:
            CpuCopy16(sSwordsOfJustice_Slash13_Palette, &gPlttBufferFaded[1], sizeof(sSwordsOfJustice_Slash13_Palette));
            break;
        case 20:
            CpuCopy16(sSwordsOfJustice_Slash14_Palette, &gPlttBufferFaded[1], sizeof(sSwordsOfJustice_Slash14_Palette));
            break;
        case 21:
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
