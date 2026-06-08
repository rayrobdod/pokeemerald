#include "global.h"
#include "decompress.h"
#include "main.h"
#include "malloc.h"
#include "sprite.h"
#include "party_menu.h"
#include "test/test.h"
#include "config/test.h"
#include "config/general.h"
#include "constants/pokeball.h"

TEST("Party Menu Ball: Smol baseline")
{
    static const u16 origFile[] = INCGFX_U16("test/party_menu_ball/pokeball_full.png", ".4bpp");
    static const u32 compFile[] = INCGFX_U32("test/party_menu_ball/pokeball_full.png", ".4bpp.smol");
    static const u32 compSize = sizeof(compFile);

    u32 tilemapSize = GetDecompressedDataSize(compFile);
    u16 *compBuffer = (u16 *)VRAM;

    CycleCountStart();
    DecompressDataWithHeaderVram(compFile, compBuffer);
    s32 timeTaken = CycleCountEnd();

    DebugPrintf("Time %s %s: %d         Size: %d", "Party Menu Ball", "Smol", timeTaken, compSize);

    bool32 areEqual = TRUE;
    for (u32 i = 0; i < tilemapSize/2; i++)
    {
        if (origFile[i] != compBuffer[i])
        {
            areEqual = FALSE;
            break;
        }
    }

    EXPECT_EQ(areEqual, TRUE);
}

TEST("Party Menu Ball: poke")
{
    static const u16 origFile[] = INCGFX_U16("test/party_menu_ball/pokeball_full.png", ".4bpp");

    u32 tilemapSize = sizeof(origFile);
    u16 *compBuffer = (u16 *)VRAM;

    CycleCountStart();
    BuildPartyMonPokeballSprite(compBuffer, BALL_POKE);
    s32 timeTaken = CycleCountEnd();

    DebugPrintf("Time %s %s: %d", "Party Menu Ball", "Poke", timeTaken);

    bool32 areEqual = TRUE;
    for (u32 i = 0; i < tilemapSize/2; i++)
    {
        if (origFile[i] != compBuffer[i])
        {
            areEqual = FALSE;
            break;
        }
    }

    EXPECT_EQ(areEqual, TRUE);
}

TEST("Party Menu Ball: strange")
{
    static const u16 origFile[] = INCGFX_U16("test/party_menu_ball/strangeball_full.png", ".4bpp");

    u32 tilemapSize = sizeof(origFile);
    u16 *compBuffer = (u16 *)VRAM;

    CycleCountStart();
    BuildPartyMonPokeballSprite(compBuffer, BALL_STRANGE);
    s32 timeTaken = CycleCountEnd();

    DebugPrintf("Time %s %s: %d", "Party Menu Ball", "Strange", timeTaken);

    bool32 areEqual = TRUE;
    for (u32 i = 0; i < tilemapSize/2; i++)
    {
        if (origFile[i] != compBuffer[i])
        {
            areEqual = FALSE;
            break;
        }
    }

    EXPECT_EQ(areEqual, TRUE);
}
