#include "global.h"
#include "battle_transition.h"
#include "battle_transition_shared.h"
#include "battle_transition_tasks/inverse.h"
#include "palette.h"
#include "malloc.h"
#include "task.h"
#include "scanline_effect.h"
#include "constants/rgb.h"

static bool8 InverseIntro_Init(struct Task *);
static bool8 InverseIntro_End(struct Task *);
static bool8 InverseIntro_FadeToGray(struct Task *);
static bool8 InverseIntro_FadeFromGray(struct Task *);
static bool8 InverseIntro_CopyInvertedPalette(struct Task *);

static void InverseIntro_DoPaletteCalculations(struct Task *task);
static u16 HueInvert(u16 in);

#define CALCULATED_PALS_PER_FRAME ((PLTT_BUFFER_SIZE / 24) + 1)

static const TransitionStateFunc sInverseIntro_Funcs[] = {
    InverseIntro_Init,
    InverseIntro_FadeToGray,
    InverseIntro_FadeFromGray,
    InverseIntro_FadeToGray,
    InverseIntro_CopyInvertedPalette,
    Intro_FadeFromGray,
    Intro_FadeToGray,
    Intro_FadeFromGray,
    InverseIntro_End
};

#define tBlend                 data[1]  // see battle_transition/intro.c
#define tInvertProgress        data[2]
#define bufferDataElem         (3)

// See: SetWordTaskArg
static void SetWordTaskArgPtr(struct Task *task, u8 dataElem, u32 value)
{
    if (dataElem < NUM_TASK_DATA - 1)
    {
        task->data[dataElem] = value;
        task->data[dataElem + 1] = value >> 16;
    }
}

// See: GetWordTaskArg
static u32 GetWordTaskArgPtr(struct Task *task, u8 dataElem)
{
    if (dataElem < NUM_TASK_DATA - 1)
        return (u16)task->data[dataElem] | (task->data[dataElem + 1] << 16);
    else
        return 0;
}

void Task_InverseIntro(u8 taskId)
{
    while (sInverseIntro_Funcs[gTasks[taskId].tState](&gTasks[taskId]));
}

static bool8 InverseIntro_Init(struct Task *task)
{
    InitTransitionData();
    ScanlineEffect_Clear();

    REG_WININ = WININ_WIN0_ALL;
    REG_WINOUT = 0;
    REG_WIN0H = DISPLAY_WIDTH;
    REG_WIN0V = DISPLAY_HEIGHT;

    task->tInvertProgress = 0;
    SetWordTaskArgPtr(task, bufferDataElem, (u32) Alloc(PLTT_BUFFER_SIZE * sizeof(u16)));

    return Intro_Init(task);
}

static bool8 InverseIntro_End(struct Task *task)
{
    Free((void*) GetWordTaskArgPtr(task, bufferDataElem));
    return Intro_End(task);
}

static bool8 InverseIntro_FadeToGray(struct Task *task)
{
    InverseIntro_DoPaletteCalculations(task);

    return Intro_FadeToGray(task);
}

static bool8 InverseIntro_FadeFromGray(struct Task *task)
{
    InverseIntro_DoPaletteCalculations(task);

    return Intro_FadeFromGray(task);
}

static bool8 InverseIntro_CopyInvertedPalette(struct Task *task)
{
    memcpy(gPlttBufferUnfaded, (void*) GetWordTaskArgPtr(task, bufferDataElem), PLTT_SIZE);
    task->tState++;
    return FALSE;
}

static void InverseIntro_DoPaletteCalculations(struct Task *task)
{
    int i;
    int end;
    u16 *plttBufferInverted;

    plttBufferInverted = (u16 *) GetWordTaskArgPtr(task, bufferDataElem);
    end = min(task->tInvertProgress + CALCULATED_PALS_PER_FRAME, PLTT_BUFFER_SIZE);
    for (i = task->tInvertProgress; i < end; i++)
    {
        plttBufferInverted[i] = HueInvert(gPlttBufferUnfaded[i]);
    }
    task->tInvertProgress = end;
}

static u16 HueInvert(u16 color)
{
    /* I suspect that there is some shortcut available due to the exactly
     * 180 degree rotation, but I don't know what that shortcut is
     */

    int /* [0, 31] */ r, g, b;
    int /* [0, 31] */ brightness;
    int /* [0, 31] */ delta;
    int /* [0, 6 * 256] fixpoint 256 */ hue;
    int /* [0, 31] */ saturation;
    int /* [0, 6 * 256] fixpoint 256 */ k;
    int /* [0, 256] fixpoint 256 */ y;
    int z;

    r = GET_R(color);
    g = GET_G(color);
    b = GET_B(color);

    // rgb to hsb
    brightness = max(r, max(b, g));
    delta = brightness - min(r, min(b, g));
    if (0 == delta)
    {
        hue = 0;
    }
    else if (brightness == r)
    {
        hue = (((256 * (g - b)) / delta) + (6 * 256)) % (6 * 256);
    }
    else if (brightness == g)
    {
        hue = ((256 * (g - b)) / delta) + (2 * 256);
    }
    else if (brightness == b)
    {
        hue = ((256 * (g - b)) / delta) + (4 * 256);
    }
    saturation = (0 == brightness ? 0 : 31 * delta / brightness);

    // hue shift
    hue += 3 * 256;

    // hsb to rgb
    k = ((5 * 256) + hue) % (6 * 256);
    y = max(0, min(256, min(k, 4 * 256 - k)));
    z = brightness * ((31 * 256) - (saturation * y));
    r = z / (256 * 31);

    k = ((3 * 256) + hue) % (6 * 256);
    y = max(0, min(256, min(k, 4 * 256 - k)));
    z = brightness * ((31 * 256) - (saturation * y));
    g = z / (256 * 31);

    k = ((1 * 256) + hue) % (6 * 256);
    y = max(0, min(256, min(k, 4 * 256 - k)));
    z = brightness * ((31 * 256) - (saturation * y));
    b = z / (256 * 31);

    return _RGB(r, g, b);
}
