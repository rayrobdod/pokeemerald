#include "global.h"
#include "test/test.h"

#include "battle_transition.h"
#include "palette.h"
#include "sprite.h"
#include "task.h"

// Battle Transition DebugPrintf switch
#define T_BATTLE_TRANSITION_SHOULD_PRINT FALSE

TEST("Battle Transition")
{
#ifndef NDEBUG
    u32 frame = 0;
#endif
    u32 i;
    u32 transition = 0;

    for (i = 0; i < B_TRANSITION_COUNT; i++)
    {
        PARAMETRIZE_LABEL("%d", i) { transition = i; }
    }

    ResetTasks();
    ResetPaletteFadeControl();

    BattleTransition_Start(transition);

    while (! IsBattleTransitionDone())
    {
#ifndef NDEBUG
        if (T_BATTLE_TRANSITION_SHOULD_PRINT)
            CycleCountStart();
#endif

        RunTasks();

        AnimateSprites();
        UpdatePaletteFade();
        TransferPlttBuffer();

#ifndef NDEBUG
        if (T_BATTLE_TRANSITION_SHOULD_PRINT)
        {
            s32 timeTaken = CycleCountEnd();
            const unsigned CYCLES_PER_FRAME = 280896;

            //if (B_TRANSITION_SWIRL == transition)
            //if (CYCLES_PER_FRAME * 7 / 10 < timeTaken)
            {
                DebugPrintf(
                    "Transition %3d ; Frame %3d ; Time taken: %7d (%d.%03d frames)",
                    transition,
                    frame,
                    timeTaken,
                    timeTaken / CYCLES_PER_FRAME,
                    (timeTaken * 1000 / CYCLES_PER_FRAME) % 1000);
            }
            ++frame;
        }
#endif
    }
}
