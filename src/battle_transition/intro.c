#include "global.h"
#include "battle_transition.h"
#include "battle_transition_shared.h"
#include "battle_transition_tasks/intro.h"
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
    task->tBlend += FADE_INCREMENT;
    if (task->tBlend > 16)
    {
        task->tBlend = 16;
        task->tState++;
    }
    BlendPalettes(PALETTES_ALL, task->tBlend, RGB(11, 11, 11));

    return FALSE;
}

bool8 Intro_FadeFromGray(struct Task *task)
{
    task->tBlend -= FADE_INCREMENT;
    if (task->tBlend < 0)
    {
        task->tBlend = 0;
        task->tState++;
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
