#include "global.h"
#include "test/test.h"

#include "battle_setup.h"
#include "battle_transition.h"
#include "bg.h"
#include "decompress.h"
#include "field_effect.h"
#include "gpu_regs.h"
#include "graphics.h"
#include "main.h"
#include "malloc.h"
#include "menu.h"
#include "palette.h"
#include "scanline_effect.h"
#include "sprite.h"
#include "task.h"
#include "tilesets.h"
#include "trainer_pokemon_sprites.h"

#include "constants/rgb.h"

// Battle Transition DebugPrintf switch
#define T_BATTLE_TRANSITION_SHOULD_PRINT_FRAME_COUNT FALSE
#define T_BATTLE_TRANSITION_SHOULD_PRINT_CYCLE_COUNT FALSE


static const struct BgTemplate sOverworldBgTemplates[] =
{
    {
        .bg = 0,
        .charBaseIndex = 2,
        .mapBaseIndex = 31,
        .screenSize = 0,
        .paletteMode = 0,
        .priority = 0,
        .baseTile = 0
    },
    {
        .bg = 1,
        .charBaseIndex = 0,
        .mapBaseIndex = 29,
        .screenSize = 0,
        .paletteMode = 0,
        .priority = 1,
        .baseTile = 0
    },
    {
        .bg = 2,
        .charBaseIndex = 0,
        .mapBaseIndex = 28,
        .screenSize = 0,
        .paletteMode = 0,
        .priority = 2,
        .baseTile = 0
    },
    {
        .bg = 3,
        .charBaseIndex = 0,
        .mapBaseIndex = 30,
        .screenSize = 0,
        .paletteMode = 0,
        .priority = 3,
        .baseTile = 0
    }
};

static const struct WindowTemplate sWindowTemplates[] =
{
    {
        .bg = 0,
        .tilemapLeft = 2,
        .tilemapTop = 15,
        .width = 27,
        .height = 4,
        .paletteNum = 15,
        .baseBlock = 1
    },
    DUMMY_WIN_TEMPLATE
};

#define TAG_PLAYER (0x5000)
extern const struct OamData gObjectEventBaseOam_16x32;
const u32 gObjectEventPic_Brendan[] = INCBIN_U32("graphics/object_events/pics/people/brendan/walking.4bpp");
const u32 gObjectEventPic_May[] = INCBIN_U32("graphics/object_events/pics/people/may/walking.4bpp");
static const struct SpriteSheet sSpriteSheet_Brendan =
{
    .data = gObjectEventPic_Brendan,
    .size = 8 * 4 * 2 * 4,
    .tag = TAG_PLAYER,
};
static const struct SpriteSheet sSpriteSheet_May =
{
    .data = gObjectEventPic_May,
    .size = 8 * 4 * 2 * 4,
    .tag = TAG_PLAYER,
};

static const struct SpritePalette sSpritePalette_Brendan =
{
    .data = gObjectEventPal_Brendan,
    .tag = TAG_PLAYER,
};
static const struct SpritePalette sSpritePalette_May =
{
    .data = gObjectEventPal_May,
    .tag = TAG_PLAYER,
};

static const struct SpriteTemplate sSpriteTemplate_Player = {
    .tileTag = TAG_PLAYER,
    .paletteTag = TAG_PLAYER,
    .oam = &gObjectEventBaseOam_16x32,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = NULL,
};

static void BattleTransitionTest_InitScreen()
{
    ResetBgsAndClearDma3BusyFlags(FALSE);
    InitBgsFromTemplates(0, sOverworldBgTemplates, NELEMS(sOverworldBgTemplates));
    void* tilemapBuffer0 = AllocZeroed(BG_SCREEN_SIZE);
    assertf(tilemapBuffer0 != NULL, "tilemapBuffer0 == NULL") {
    }
    SetBgTilemapBuffer(0, tilemapBuffer0);
    SetGpuReg(REG_OFFSET_MOSAIC, 0);
    SetGpuReg(REG_OFFSET_WININ, WININ_WIN0_BG_ALL | WININ_WIN0_OBJ | WININ_WIN1_BG_ALL | WININ_WIN1_OBJ);
    SetGpuReg(REG_OFFSET_WINOUT, WINOUT_WIN01_BG0 | WINOUT_WINOBJ_BG0);
    SetGpuReg(REG_OFFSET_WIN0V, WIN_RANGE(0, 0xFF));
    SetGpuReg(REG_OFFSET_WIN0H, WIN_RANGE(0, 0xFF));
    SetGpuReg(REG_OFFSET_WIN1V, WIN_RANGE(0xFF, 0xFF));
    SetGpuReg(REG_OFFSET_WIN1H, WIN_RANGE(0xFF, 0xFF));
    SetGpuReg(REG_OFFSET_BLDCNT, BLDCNT_TGT2_BG1 | BLDCNT_TGT2_BG2 | BLDCNT_TGT2_BG3
                               | BLDCNT_TGT2_OBJ | BLDCNT_EFFECT_BLEND);
    SetGpuReg(REG_OFFSET_BLDALPHA, BLDALPHA_BLEND(13, 7));
    ChangeBgX(0, 0, BG_COORD_SET);
    ChangeBgY(0, 0, BG_COORD_SET);
}

static void BattleTransitionTest_DrawScene()
{
    FillPalette(RGB_MAGENTA, BG_PLTT_ID(0), PLTT_SIZEOF(16 * 32));

    // draw ground tiles
    LoadPalette(gTileset_General.palettes, BG_PLTT_ID(0), PLTT_SIZEOF(16 * 7));
    FillPalette(RGB_BLACK, BG_PLTT_ID(0), PLTT_SIZEOF(1));
    DecompressDataWithHeaderVram(gTileset_General.tiles, (void*) BG_CHAR_ADDR(0));
    DmaFill16(3, 0, BG_CHAR_ADDR(2), TILE_SIZE_4BPP);
    DmaFill16(3, 0, BG_SCREEN_ADDR(28), BG_SCREEN_SIZE * 4);

    for (unsigned i = 0; i < 32 * 32; i++) {
        u16 v = 0x2002 + (i % 2 == (i / 32) % 2 ? 0 : 1);
        u16* bg = (u16*) BG_SCREEN_ADDR(30);
        bg[i] = v;
    }

    // draw a dialog box on BG0
    LoadMessageBoxAndBorderGfx();
    InitWindows(sWindowTemplates);
    PutWindowTilemap(0);
    DrawDialogueFrame(0, FALSE);
    CopyWindowToVram(0, COPYWIN_FULL);

    // draw a sprite
    if (Random() % 2) {
        LoadSpritePalette(&sSpritePalette_Brendan);
        LoadSpriteSheet(&sSpriteSheet_Brendan);
    } else {
        LoadSpritePalette(&sSpritePalette_May);
        LoadSpriteSheet(&sSpriteSheet_May);
    }
    CreateSprite(&sSpriteTemplate_Player, 120, 80, 0);
}

static void VBlankCB_Field(void)
{
    LoadOam();
    ProcessSpriteCopyRequests();
    ScanlineEffect_InitHBlankDmaTransfer();
    TransferPlttBuffer();
    UpdatePaletteFade();
}

static IntrCallback sSavedVblankCallback = NULL;
static void VBlankCB_StartCycleCount(void)
{
    CycleCountStart();
    sSavedVblankCallback();
}


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

    memset(&gTrainerBattleParameter, 0, sizeof(TrainerBattleParameter));
    gTrainerBattleParameter.params.opponentA = 1;
    gPartnerTrainerId = TRAINER_NONE;

    DisableInterrupts(INTR_FLAG_HBLANK);
    DmaStop(0);
    ResetTasks();
    ResetSpriteData();
    ResetPaletteFade();
    ScanlineEffect_Clear();
    ResetAllPicSprites();
    FreeAllSpritePalettes();
    FieldEffectActiveListClear();
    ResetPaletteFadeControl();
    SetVBlankCallback(VBlankCB_Field);

    BattleTransitionTest_InitScreen();
    BattleTransitionTest_DrawScene();
    VBlankIntrWait();

    BeginNormalPaletteFade(PALETTES_ALL, 1, 16, 0, RGB_BLACK);

    SetGpuReg(REG_OFFSET_DISPCNT, DISPCNT_OBJ_ON | DISPCNT_WIN0_ON | DISPCNT_WIN1_ON
                                | DISPCNT_OBJ_1D_MAP | DISPCNT_HBLANK_INTERVAL);
    ShowBg(0);
    ShowBg(1);
    ShowBg(2);
    ShowBg(3);


    AnimateSprites();
    BuildOamBuffer();
    while (gPaletteFade.active)
    {
        VBlankIntrWait();
    }

    BattleTransition_Start(transition);

    while (! IsBattleTransitionDone())
    {
#ifndef NDEBUG
        if (T_BATTLE_TRANSITION_SHOULD_PRINT_CYCLE_COUNT
                && gMain.vblankCallback != VBlankCB_StartCycleCount)
        {
            sSavedVblankCallback = gMain.vblankCallback;
            SetVBlankCallback(VBlankCB_StartCycleCount);
        }
#endif

        VBlankIntrWait();

        RunTasks();

        AnimateSprites();
        BuildOamBuffer();
        UpdatePaletteFade();
        TransferPlttBuffer();

#ifndef NDEBUG
        if (T_BATTLE_TRANSITION_SHOULD_PRINT_CYCLE_COUNT)
        {
            s32 timeTaken = CycleCountEnd();
            const unsigned CYCLES_PER_FRAME = 280896;

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
        }
        if (T_BATTLE_TRANSITION_SHOULD_PRINT_CYCLE_COUNT
                || T_BATTLE_TRANSITION_SHOULD_PRINT_FRAME_COUNT)
        {
            ++frame;
        }
#endif
    }

#ifndef NDEBUG
    if (T_BATTLE_TRANSITION_SHOULD_PRINT_FRAME_COUNT)
    {
        DebugPrintf(
            "Transition %3d ; Frames %3d",
            transition,
            frame);
    }
#endif

    void* tilemapBuffer0 = GetBgTilemapBuffer(0);
    if (tilemapBuffer0)
    {
        UnsetBgTilemapBuffer(0);
        Free(tilemapBuffer0);
    }
    FreeAllWindowBuffers();
}
