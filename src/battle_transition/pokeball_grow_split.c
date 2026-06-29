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

static const u8 sOamHeights[3][4] =
{
    [ST_OAM_SQUARE] =
    {
        [SPRITE_SIZE(8x8)]   =  8,
        [SPRITE_SIZE(16x16)] = 16,
        [SPRITE_SIZE(32x32)] = 32,
        [SPRITE_SIZE(64x64)] = 64,
    },
    [ST_OAM_H_RECTANGLE] =
    {
        [SPRITE_SIZE(16x8)]  =  8,
        [SPRITE_SIZE(32x8)]  =  8,
        [SPRITE_SIZE(32x16)] = 16,
        [SPRITE_SIZE(64x32)] = 32,
    },
    [ST_OAM_V_RECTANGLE] =
    {
        [SPRITE_SIZE(8x16)]  = 16,
        [SPRITE_SIZE(8x32)]  = 32,
        [SPRITE_SIZE(16x32)] = 32,
        [SPRITE_SIZE(32x64)] = 64,
    },
};

enum
{
    PALTAG_GROWING_POKEBALL = 0xFA5A,
    TILETAG_GROWING_POKEBALL = 0xFA5A,

    SPLIT_VELOCITY = 8,
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

#define tSpriteId data[2]
#define tTimer data[3]
#define tOffset data[4]

#define tWIN0H_Upper data[1]

static void PokeballGrowSplit_MoveSprites(void)
{
    unsigned sprite_i;
    for (sprite_i = 0; sprite_i < ARRAY_COUNT(gSprites); sprite_i++)
    {
        struct Sprite *sprite = &gSprites[sprite_i];
        if (sprite->inUse)
        {
            if (sprite->oam.y < DISPLAY_HEIGHT / 2)
            {
                sprite->x2 += SPLIT_VELOCITY;
            }
            else
            {
                sprite->x2 -= SPLIT_VELOCITY;
            }
        }
    }
}

static void PokeballGrowSplit_PrepareOamShear(unsigned offset)
{
    unsigned sprite_i;
    unsigned buffer_i = 0;
    struct OamData *oam = (struct OamData *) OAM;

    for (sprite_i = 0; sprite_i < (OAM_SIZE / sizeof(struct OamData)); sprite_i++, oam++) {
        if (oam->y < DISPLAY_HEIGHT / 2 &&
            (oam->y + sOamHeights[oam->shape][oam->size]) % 256 > DISPLAY_HEIGHT / 2)
        {
            gScanlineEffectRegBuffers[0][buffer_i++] = sprite_i;
            gScanlineEffectRegBuffers[0][buffer_i++] = oam->x - 2 * offset;
        }
    }
    gScanlineEffectRegBuffers[0][buffer_i] = 0xFF;
}

static void VBlankCB_PokeballGrowSplit_Split(void)
{
    VBlankCB_BattleTransition();
    REG_WININ = sTransitionData->WININ;
    REG_WINOUT = sTransitionData->WINOUT;
    REG_WIN0V = sTransitionData->WIN0V;
    REG_WIN1V = sTransitionData->WIN0V;

    REG_BG0HOFS = sTransitionData->BG0HOFS_Upper;
    REG_BG1HOFS = sTransitionData->cameraX + sTransitionData->BG0HOFS_Upper;
    REG_BG2HOFS = sTransitionData->cameraX + sTransitionData->BG0HOFS_Upper;
    REG_BG3HOFS = sTransitionData->cameraX + sTransitionData->BG0HOFS_Upper;
    REG_WIN0H = sTransitionData->tWIN0H_Upper;
}

static void HBlankCB_PokeballGrowSplit_Split(void)
{
    if (REG_VCOUNT == DISPLAY_HEIGHT / 2)
    {
        struct OamData *oams = (struct OamData*)OAM;
        unsigned buffer_i = 0;

        REG_BG0HOFS = sTransitionData->BG0HOFS_Lower;
        REG_BG1HOFS = sTransitionData->cameraX + sTransitionData->BG0HOFS_Lower;
        REG_BG2HOFS = sTransitionData->cameraX + sTransitionData->BG0HOFS_Lower;
        REG_BG3HOFS = sTransitionData->cameraX + sTransitionData->BG0HOFS_Lower;
        REG_WIN0H = sTransitionData->WIN0H;

        while (0xFF != gScanlineEffectRegBuffers[0][buffer_i])
        {
            unsigned sprite_i = gScanlineEffectRegBuffers[0][buffer_i++];
            unsigned new_x = gScanlineEffectRegBuffers[0][buffer_i++];

            oams[sprite_i].x = new_x;
        }
    }
}

void Task_PokeballGrowSplit(u8 taskId)
{
    while (sPokeballGrowSplit_Funcs[gTasks[taskId].tState](&gTasks[taskId]));
}

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
        gScanlineEffectRegBuffers[0][0] = 0xFF;
        sTransitionData->WININ = WININ_WIN0_BG_ALL | WININ_WIN0_OBJ;
        sTransitionData->WINOUT = 0;
        sTransitionData->WIN0V = WIN_RANGE(0, DISPLAY_HEIGHT);
        sTransitionData->WIN0H = WIN_RANGE(0, DISPLAY_WIDTH);
        sTransitionData->tWIN0H_Upper = WIN_RANGE(0, DISPLAY_WIDTH);
        EnableInterrupts(INTR_FLAG_HBLANK);
        SetVBlankCallback(VBlankCB_PokeballGrowSplit_Split);
        SetHBlankCallback(HBlankCB_PokeballGrowSplit_Split);

        task->tState++;
    }
    return FALSE;
}

static bool8 PokeballGrowSplit_Split(struct Task *task)
{
    task->tOffset += SPLIT_VELOCITY;

    sTransitionData->BG0HOFS_Lower = task->tOffset - SPLIT_VELOCITY;
    sTransitionData->BG0HOFS_Upper = -task->tOffset;
    sTransitionData->WIN0H = WIN_RANGE(0, DISPLAY_WIDTH - task->tOffset + SPLIT_VELOCITY);
    sTransitionData->tWIN0H_Upper = WIN_RANGE(task->tOffset, DISPLAY_WIDTH);
    PokeballGrowSplit_MoveSprites();
    PokeballGrowSplit_PrepareOamShear(task->tOffset - SPLIT_VELOCITY);

    if (task->tOffset > DISPLAY_WIDTH)
    {
        FadeScreenBlack();
        DestroyTask(FindTaskIdByFunc(Task_PokeballGrowSplit));
    }

    return FALSE;
}
