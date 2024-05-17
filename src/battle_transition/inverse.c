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

#define FADE_INCREMENT (2)
#define CALCULATED_PALS_PER_FRAME ((PLTT_BUFFER_SIZE / 24) + 1)

static const TransitionStateFunc sInverseIntro_Funcs[] = {
    InverseIntro_Init,
    InverseIntro_FadeToGray,
    InverseIntro_FadeFromGray,
    InverseIntro_FadeToGray,
    InverseIntro_CopyInvertedPalette,
    // You could have extra functions for fades without the palette calculations,
    // but the calculations are bounded to PLTT_BUFFER_SIZE,
    // so using the same functions skips pal calculations, just requiring calculating bounds
    // and everything still fits in a frame,
    // so adding extra functions in exchange not calculating bounds isn't worthwhile
    InverseIntro_FadeFromGray,
    InverseIntro_FadeToGray,
    InverseIntro_FadeFromGray,
    InverseIntro_End
};

#define tBlend                 data[1]
#define tInvertProgress        data[2]

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

    task->tState++;
    task->tBlend = 0;
    task->tInvertProgress = 0;

    return FALSE;
}

static bool8 InverseIntro_End(struct Task *task)
{
    DestroyTask(FindTaskIdByFunc(task->func));
    return FALSE;
}

static bool8 InverseIntro_FadeToGray(struct Task *task)
{
    task->tBlend += FADE_INCREMENT;
    if (task->tBlend > 16)
    {
        task->tBlend = 16;
        task->tState++;
    }
    BlendPalettes(PALETTES_ALL, task->tBlend, RGB(11, 11, 11));

    InverseIntro_DoPaletteCalculations(task);

    return FALSE;
}

static bool8 InverseIntro_FadeFromGray(struct Task *task)
{
    task->tBlend -= FADE_INCREMENT;
    if (task->tBlend < 0)
    {
        task->tBlend = 0;
        task->tState++;
    }
    BlendPalettes(PALETTES_ALL, task->tBlend, RGB(11, 11, 11));

    InverseIntro_DoPaletteCalculations(task);

    return FALSE;
}

static bool8 InverseIntro_CopyInvertedPalette(struct Task *task)
{
    memcpy(gPlttBufferUnfaded, gPaletteDecompressionBuffer, PLTT_SIZE);
    task->tState++;
    return FALSE;
}

static void InverseIntro_DoPaletteCalculations(struct Task *task)
{
    int i;
    int end;
    u16 *plttBufferInverted;

    plttBufferInverted = (u16 *)(gPaletteDecompressionBuffer);
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
