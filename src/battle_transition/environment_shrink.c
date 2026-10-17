#include "global.h"
#include "battle_transition.h"
#include "battle_transition_shared.h"
#include "battle_transition_yoshord.h"
#include "bg.h"
#include "gpu_regs.h"
#include "io_reg.h"
#include "scanline_effect.h"
#include "task.h"
#include "constants/rgb.h"

#define tStep data[2]
#define tDelay data[3]
#define tWinV data[4]

static bool8 EnvironmentShrink_Init(struct Task *);
static bool8 EnvironmentShrink_Main(struct Task *);
static bool8 EnvironmentShrink_End(struct Task *);

static const TransitionStateFunc sEnvironmentShrink_Funcs[] = {
    EnvironmentShrink_Init,
    EnvironmentShrink_Main,
    EnvironmentShrink_End,
};

void Task_EnvironmentShrink(u8 taskId)
{
    while (sEnvironmentShrink_Funcs[gTasks[taskId].tState](&gTasks[taskId]));
}

#include "field_camera.h"

static bool8 EnvironmentShrink_Init(struct Task *task)
{
    InitTransitionData();
    ScanlineEffect_Clear();

    // Assumes the 0 tile for all backgrounds is transparent

    task->tState++;
    task->tDelay = 2;
    return TRUE;
}

static bool8 EnvironmentShrink_Main(struct Task *task)
{
    s32 screen_x, screen_y;
    u32 target_x, target_y, target_flip_y;
    u32 source_x, source_y, source_flip_y;
    u32 tile_hofs, tile_vofs;

    if (1 == task->tDelay)
    {
        if (task->tStep % 3 != 1) {task->tWinV += 8;}

        SetGpuReg_ForcedBlank(REG_OFFSET_WIN0H, WIN_RANGE(task->tStep * 8 + 8, DISPLAY_WIDTH - task->tStep * 8 - 8));
        SetGpuReg_ForcedBlank(REG_OFFSET_WIN0V, WIN_RANGE(task->tWinV, DISPLAY_HEIGHT - task->tWinV));
    }
    if (0 == task->tDelay)
    {
        u32 bg = 0;
        volatile u16* bgcnt = (vu16 *)REG_ADDR_BG0CNT;

        for (bg = 0; bg < 4; bg++, bgcnt++)
        {
            volatile u16* tilemap = (vu16 *)(BG_VRAM + (((*bgcnt) >> 8) << 11));
            if (0 == bg)
            {
                tile_hofs = tile_vofs = 0;
            }
            else
            {
                tile_hofs = sTransitionData->cameraX / TILE_WIDTH;
                tile_vofs = sTransitionData->cameraY / TILE_HEIGHT;
            }

            for (screen_y = DISPLAY_TILE_HEIGHT / 2 - 1; screen_y > 0; --screen_y)
            {
                target_y = (screen_y + tile_vofs) % 32;
                source_y = (task->tStep % 3 == 1 ? target_y : (screen_y - 1 + tile_vofs) % 32);
                target_flip_y = ((DISPLAY_TILE_HEIGHT - 1) - screen_y + tile_vofs) % 32;
                source_flip_y = (task->tStep % 3 == 1 ? target_flip_y : ((DISPLAY_TILE_HEIGHT - 1) - (screen_y - 1) + tile_vofs) % 32);

                for (screen_x = DISPLAY_TILE_WIDTH / 2 - 1; screen_x > 0; --screen_x)
                {
                    target_x = (screen_x + tile_hofs) % 32;
                    source_x = (screen_x - 1 + tile_hofs) % 32;

                    tilemap[target_y * 32 + target_x] = tilemap[source_y * 32 + source_x];
                    tilemap[target_flip_y * 32 + target_x] = tilemap[source_flip_y * 32 + source_x];

                    target_x = ((DISPLAY_TILE_WIDTH - 1) - screen_x + tile_hofs) % 32;
                    source_x = ((DISPLAY_TILE_WIDTH - 1) - (screen_x - 1) + tile_hofs) % 32;

                    tilemap[target_y * 32 + target_x] = tilemap[source_y * 32 + source_x];
                    tilemap[target_flip_y * 32 + target_x] = tilemap[source_flip_y * 32 + source_x];
                }

                target_x = tile_hofs % 32;
                tilemap[target_y * 32 + target_x] = 0;
                tilemap[target_flip_y * 32 + target_x] = 0;

                target_x = (target_x + DISPLAY_TILE_WIDTH - 1) % 32;
                tilemap[target_y * 32 + target_x] = 0;
                tilemap[target_flip_y * 32 + target_x] = 0;
            }

            for (screen_x = DISPLAY_TILE_WIDTH - 1; screen_x >= 0; --screen_x)
            {
                target_x = (screen_x + tile_hofs) % 32;
                target_y = (0 + tile_vofs) % 32;
                tilemap[target_y * 32 + target_x] = 0;
                target_y = (19 + tile_vofs) % 32;
                tilemap[target_y * 32 + target_x] = 0;
            }
        }

        task->tDelay = 4;
        // The horizontal steps cover less distance than the diagonal steps,
        // so make the horizontal steps take less time to compensate
        if (task->tStep % 3 == 0) {task->tDelay = 3;}
        task->tStep++;
    }
    task->tDelay--;

    if (task->tStep == DISPLAY_TILE_WIDTH / 2)
    {
        task->tState++;
    }

    return FALSE;
}

static bool8 EnvironmentShrink_End(struct Task *task)
{
    FadeScreenBlack();
    DestroyTask(FindTaskIdByFunc(Task_EnvironmentShrink));

    return FALSE;
}
