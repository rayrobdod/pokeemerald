#include "global.h"
#include "battle_transition.h"
#include "battle_transition_shared.h"
#include "field_weather.h"
#include "gpu_regs.h"
#include "palette.h"
#include "task.h"
#include "constants/rgb.h"

#define FADE_INCREMENT (2)

static const TransitionStateFunc sIntro_Funcs[] = {
    Intro_Init,
    Intro_FadeToGray,
    Intro_FadeFromGray,
    Intro_FadeToGray,
    Intro_FadeFromGray,
    Intro_FadeToGray,
    Intro_FadeFromGray,
    Intro_End,
};

static const TransitionStateFunc sIntroWithClearBg0_Funcs[] = {
    Intro_Init,
    Intro_FadeToGray,
    Intro_FadeFromGray,
    Intro_FadeToGray,
    Intro_ClearBg0,
    Intro_FadeFromGray,
    Intro_FadeToGray,
    Intro_FadeFromGray,
    Intro_End,
};

#define tBlend data[1]
#define tBldCntSaved data[8]
#define tShadowColor data[9]

void Task_Intro(u8 taskId)
{
    while (sIntro_Funcs[gTasks[taskId].tState](&gTasks[taskId]));
}

void Task_IntroWithClearBg0(u8 taskId)
{
    while (sIntroWithClearBg0_Funcs[gTasks[taskId].tState](&gTasks[taskId]));
}

bool8 Intro_Init(struct Task *task)
{
    task->tBlend = 0;
    task->tState++;
    return TRUE;
}

bool8 Intro_End(struct Task *task)
{
    DestroyTask(FindTaskIdByFunc(task->func));
    return FALSE;
}

bool8 Intro_FadeToGray(struct Task *task)
{
    u8 paletteNum = IndexOfSpritePaletteTag(TAG_WEATHER_START);
    u16 index = OBJ_PLTT_ID(paletteNum) + SHADOW_COLOR_INDEX;
    task->tBlend += FADE_INCREMENT;
    if (task->tBlend > 16)
    {
        task->tBlend = 16;
        task->tState++;

        // Save BLDCNT and turn off targets temporarily
        task->tBldCntSaved = GetGpuReg(REG_OFFSET_BLDCNT);
        SetGpuReg(REG_OFFSET_BLDCNT, task->tBldCntSaved & ~BLDCNT_TGT2_BG_ALL);
        if (paletteNum < 16)
            gPlttBufferFaded[index] = RGB(11, 11, 11);
    }
    if (paletteNum < 16)
        task->tShadowColor = gPlttBufferFaded[index];
    BlendPalettes(PALETTES_ALL, task->tBlend, RGB(11, 11, 11));
    if (paletteNum < 16)
        gPlttBufferFaded[index] = task->tShadowColor;

    return FALSE;
}

bool8 Intro_FadeFromGray(struct Task *task)
{
    u8 paletteNum = IndexOfSpritePaletteTag(TAG_WEATHER_START);
    task->tBlend -= FADE_INCREMENT;
    if (task->tBlend < 0)
    {
        task->tBlend = 0;
        task->tState++;

        // Restore BLDCNT
        SetGpuReg(REG_OFFSET_BLDCNT, task->tBldCntSaved);
        if (paletteNum < 16)
        {
            u16 index = OBJ_PLTT_ID(paletteNum) + SHADOW_COLOR_INDEX;
            gPlttBufferFaded[index] = task->tShadowColor;
        }
    }
    BlendPalettes(PALETTES_ALL, task->tBlend, RGB(11, 11, 11));

    return FALSE;
}

bool8 Intro_ClearBg0(struct Task * task)
{
    u16 *tilemap, *tileset;
    GetBg0TilesDst(&tilemap, &tileset);
    CpuFill16(0, tilemap, BG_SCREEN_SIZE);
    CpuFill16(0, tileset, TILE_SIZE_4BPP);

    task->tState++;
    return TRUE;
}
