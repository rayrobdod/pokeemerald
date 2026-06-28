#include "global.h"
#include "battle_transition.h"
#include "battle_transition_shared.h"
#include "battle_transition_tasks/pokeball_grow_split.h"
#include "decompress.h"
#include "scanline_effect.h"
#include "task.h"
#include "palette.h"
#include "gpu_regs.h"
#include "main.h"

static bool8 PokeballGrowSplit_Init(struct Task *);
static bool8 PokeballGrowSplit_Grow(struct Task *);
static bool8 PokeballGrowSplit_Split(struct Task *);
static bool8 PokeballGrowSplit_Wait(struct Task *);
bool8 BigPokeball_SetGfx(struct Task *);

static const u32 sBigPokeball_Tileset[] = INCGFX_U32("graphics/battle_transitions/big_pokeball.png", ".4bpp");
static const u16 sFieldEffectPal_Pokeball[] = INCGFX_U16("graphics/field_effects/palettes/pokeball.pal", ".gbapal");

static const u32 sGrowingPokeball_Tileset[] = INCGFX_U32("graphics/battle_transitions/pokeball_64.png", ".4bpp.lz");
static const u16 sGrowingPokeball_Palette[] = INCGFX_U16("graphics/battle_transitions/pokeball_64.png", ".gbapal");

enum
{
    PALTAG_GROWING_POKEBALL = 0xFA5A,
    TILETAG_GROWING_POKEBALL = 0xFA5A,
};

static const struct CompressedSpriteSheet sSpriteSheet_GrowingPokeball =
{
    .data = sGrowingPokeball_Tileset,
    .size = TILE_OFFSET_4BPP(8 * 8),
    .tag = TILETAG_GROWING_POKEBALL,
};

static const struct SpritePalette sSpritePalette_GrowingPokeball =
{
    .data = sGrowingPokeball_Palette,
    .tag = PALTAG_GROWING_POKEBALL,
};

static const struct OamData sOamData_GrowingPokeball =
{
    .y = 0,
    .affineMode = ST_OAM_AFFINE_DOUBLE,
    .objMode = ST_OAM_OBJ_NORMAL,
    .mosaic = FALSE,
    .bpp = ST_OAM_4BPP,
    .shape = SPRITE_SHAPE(64x64),
    .x = 0,
    .matrixNum = 0,
    .size = SPRITE_SIZE(64x64),
    .tileNum = 0,
    .priority = 0,
    .paletteNum = 0,
    .affineParam = 0
};

static const union AffineAnimCmd sSpriteAffineAnimTable_GrowingPokeball_Grow[] =
{
    AFFINEANIMCMD_FRAME(8, 8, 0, 0),
    AFFINEANIMCMD_FRAME(8, 8, 8, 1),
    AFFINEANIMCMD_FRAME(16, 16, 8, 31),
    AFFINEANIMCMD_END,
};

const union AffineAnimCmd *const sSpriteAffineAnimTable_GrowingPokeball[] =
{
    sSpriteAffineAnimTable_GrowingPokeball_Grow,
};

static const struct SpriteTemplate sSpriteTemplate_GrowingPokeball =
{
    .tileTag = TILETAG_GROWING_POKEBALL,
    .paletteTag = PALTAG_GROWING_POKEBALL,
    .oam = &sOamData_GrowingPokeball,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .affineAnims = sSpriteAffineAnimTable_GrowingPokeball,
    .callback = SpriteCallbackDummy,
};

static const TransitionStateFunc sPokeballGrowSplit_Funcs[] = {
    PokeballGrowSplit_Init,
    PokeballGrowSplit_Grow,
    BigPokeball_SetGfx,
    PokeballGrowSplit_Wait,
    PokeballGrowSplit_Split,
};

static void VBlankCB_PokeballGrowSplit_Split(void)
{
    VBlankCB_BattleTransition();
    REG_WININ = sTransitionData->WININ;
    REG_WINOUT = sTransitionData->WINOUT;
    REG_WIN0V = sTransitionData->WIN0V;
    REG_WIN1V = sTransitionData->WIN0V;
}

#define tWIN1H data[0]
#define tWIN0H_Upper data[1]
#define tWIN1H_Upper data[2]

static void HBlankCB_PokeballGrowSplit_Split(void)
{
    if (REG_VCOUNT < DISPLAY_HEIGHT / 2 || REG_VCOUNT > DISPLAY_HEIGHT)
    {
        REG_BG0HOFS = sTransitionData->BG0HOFS_Lower;
        REG_BG1HOFS = sTransitionData->cameraX + sTransitionData->BG0HOFS_Lower;
        REG_BG2HOFS = sTransitionData->cameraX + sTransitionData->BG0HOFS_Lower;
        REG_BG3HOFS = sTransitionData->cameraX + sTransitionData->BG0HOFS_Lower;
        REG_WIN0H = sTransitionData->WIN0H;
        REG_WIN1H = sTransitionData->tWIN1H;
    }
    else
    {
        REG_BG0HOFS = sTransitionData->BG0HOFS_Upper;
        REG_BG1HOFS = sTransitionData->cameraX + sTransitionData->BG0HOFS_Upper;
        REG_BG2HOFS = sTransitionData->cameraX + sTransitionData->BG0HOFS_Upper;
        REG_BG3HOFS = sTransitionData->cameraX + sTransitionData->BG0HOFS_Upper;
        REG_WIN0H = sTransitionData->tWIN0H_Upper;
        REG_WIN1H = sTransitionData->tWIN1H_Upper;
    }
}

void Task_PokeballGrowSplit(u8 taskId)
{
    while (sPokeballGrowSplit_Funcs[gTasks[taskId].tState](&gTasks[taskId]));
}

#define tSpriteId data[2]
#define tTimer data[3]
#define tOffset data[4]

static bool8 PokeballGrowSplit_Init(struct Task *task)
{
    u16 *tilemap, *tileset;

    InitTransitionData();
    ScanlineEffect_Clear();

    GetBg0TilesDst(&tilemap, &tileset);
    CpuFill16(0, tilemap, BG_SCREEN_SIZE);
    LoadSpritePalette(&sSpritePalette_GrowingPokeball);
    LoadPalette(sFieldEffectPal_Pokeball, BG_PLTT_ID(15), sizeof(sFieldEffectPal_Pokeball));

    LoadCompressedSpriteSheet(&sSpriteSheet_GrowingPokeball);
    CpuCopy16(sBigPokeball_Tileset, tileset, sizeof(sBigPokeball_Tileset));

    task->tSpriteId = CreateSprite(&sSpriteTemplate_GrowingPokeball, DISPLAY_WIDTH / 2, DISPLAY_HEIGHT / 2, 0);

    task->tState++;
    return TRUE;
}

static bool8 PokeballGrowSplit_Grow(struct Task *task)
{
    if (gSprites[task->tSpriteId].affineAnimEnded)
    {
        DestroySpriteAndFreeResources(&gSprites[task->tSpriteId]);
        task->tState++;
        task->tSpriteId = 0xFF;
        task->tTimer = 8;
    }
    return FALSE;
}

static bool8 PokeballGrowSplit_Wait(struct Task *task)
{
    if (0 == --task->tTimer)
    {
        sTransitionData->WININ = WININ_WIN0_BG_ALL | WININ_WIN1_OBJ;
        sTransitionData->WINOUT = 0;
        sTransitionData->WIN0V = WIN_RANGE(0, DISPLAY_HEIGHT);
        sTransitionData->WIN0H = WIN_RANGE(0, DISPLAY_WIDTH);
        sTransitionData->tWIN1H = WIN_RANGE(0, DISPLAY_WIDTH);
        sTransitionData->tWIN0H_Upper = WIN_RANGE(0, DISPLAY_WIDTH);
        sTransitionData->tWIN1H_Upper = WIN_RANGE(0, DISPLAY_WIDTH);
        EnableInterrupts(INTR_FLAG_HBLANK);
        SetVBlankCallback(VBlankCB_PokeballGrowSplit_Split);
        SetHBlankCallback(HBlankCB_PokeballGrowSplit_Split);
        task->tState++;
    }
    return FALSE;
}

static bool8 PokeballGrowSplit_Split(struct Task *task)
{
    task->tOffset += 8;

    sTransitionData->VBlank_DMA = FALSE;
    sTransitionData->BG0HOFS_Lower = task->tOffset;
    sTransitionData->BG0HOFS_Upper = -task->tOffset;
    sTransitionData->WIN0H = WIN_RANGE(0, DISPLAY_WIDTH - task->tOffset);
    sTransitionData->tWIN1H = WIN_RANGE(0, max(0, DISPLAY_WIDTH / 2 - task->tOffset));
    sTransitionData->tWIN0H_Upper = WIN_RANGE(task->tOffset, DISPLAY_WIDTH);
    sTransitionData->tWIN1H_Upper = WIN_RANGE(min(DISPLAY_WIDTH, task->tOffset + DISPLAY_WIDTH / 2), DISPLAY_WIDTH);
    sTransitionData->VBlank_DMA = TRUE;

    if (task->tOffset >= DISPLAY_WIDTH)
    {
        FadeScreenBlack();
        DestroyTask(FindTaskIdByFunc(Task_PokeballGrowSplit));
    }

    return FALSE;
}
