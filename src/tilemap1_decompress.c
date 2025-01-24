#include "global.h"
#include "tilemap1_decompress.h"

void Tilemap1_8DecompressWram(const u32 *src, void *dest)
{
    const u8 *src8 = (const u8 *) src;
    u8 *dest8 = (u8 *) dest;

    int destSize = (src8[3] << 16) | (src8[2] << 8) | src8[1];

    int incrementValue = 0;
    int repeatValue = 0;

    int srcPos = 4;
    int destPos = 0;

    u8 flags;
    int count;
    int i;

    for (;;)
    {
        flags = src8[srcPos++];
        count = (flags & 0x3F) + 1;

        switch ((flags & 0xC0) >> 6)
        {
            case 0:
                switch (flags)
                {
                    case 0:
                        incrementValue = src8[srcPos++];
                        break;
                    case 1:
                        repeatValue = src8[srcPos++];
                        break;
                    default:
                        goto fail;
                }
                break;
            case 1:
                if (destPos + count > destSize)
                    goto fail;

                for (i = 0; i < count; i++)
                    dest8[destPos++] = src8[srcPos++];

                break;
            case 2:
                if (destPos + count > destSize)
                    goto fail;

                for (i = 0; i < count; i++)
                    dest8[destPos++] = 0xFF & repeatValue;

                break;
            case 3:
                if (destPos + count > destSize)
                    goto fail;

                for (i = 0; i < count; i++)
                {
                    dest8[destPos++] = 0xFF & incrementValue;
                    incrementValue++;
                }
                break;
        }

        if (destPos == destSize)
        {
            return;
        }
    }

fail:
    ; // ???
}

void Tilemap1_8DecompressVram(const u32 *src, void *dest)
{
    const u8 *src8 = (const u8 *) src;
    u16 *dest8 = (u16 *) dest;

    int destSize = (src8[3] << 16) | (src8[2] << 8) | src8[1];

    int incrementValue = 0;
    int repeatValue = 0;

    int srcPos = 4;
    int destPos = 0;

    u8 flags;
    bool8 bufferHasValue = FALSE;
    int count;
    int i;
    u16 buffer;

    for (;;)
    {
        flags = src8[srcPos++];
        count = (flags & 0x3F) + 1;

        switch ((flags & 0xC0) >> 6)
        {
            case 0:
                switch (flags)
                {
                    case 0:
                        incrementValue = src8[srcPos++];
                        break;
                    case 1:
                        repeatValue = src8[srcPos++];
                        break;
                    default:
                        goto fail;
                }
                break;
            case 1:
                if (destPos + count > destSize)
                    goto fail;

                for (i = 0; i < count; i++)
                {
                    if (bufferHasValue)
                    {
                        buffer |= (0xFF & src8[srcPos++]) << 8;
                        dest8[destPos++] = buffer;
                    }
                    else
                    {
                        buffer = 0xFF & src8[srcPos++];
                    }
                    bufferHasValue = !bufferHasValue;
                }

                break;
            case 2:
                if (destPos + count > destSize)
                    goto fail;

                if (bufferHasValue)
                {
                    buffer |= (0xFF & repeatValue) << 8;
                    dest8[destPos++] = buffer;
                    count--;
                }

                bufferHasValue = count % 2;
                count -= bufferHasValue;

                buffer = (repeatValue << 8) | repeatValue;

                for (i = 0; i < count / 2; i++)
                {
                    dest8[destPos++] = buffer;
                }

                if (bufferHasValue)
                {
                    buffer = 0xFF & repeatValue;
                }

                break;
            case 3:
                if (destPos + count > destSize)
                    goto fail;

                for (i = 0; i < count; i++)
                {
                    if (bufferHasValue)
                    {
                        buffer |= (0xFF & incrementValue) << 8;
                        dest8[destPos++] = buffer;
                    }
                    else
                    {
                        buffer = 0xFF & incrementValue;
                    }
                    bufferHasValue = !bufferHasValue;
                    incrementValue++;
                }
                break;
        }

        if (destPos * 2 == destSize)
        {
            return;
        }
    }

fail:
    ; // ???
}

void Tilemap1_16DecompressVram(const u32 *src, void *dest)
{
    const u8 *src8 = (const u8 *) src;
    u16 *dest16 = (u16 *) dest;

    int destSize = (src8[3] << 16) | (src8[2] << 8) | src8[1];

    int incrementValue = 0;
    int repeatValue = 0;

    int srcPos = 4;
    int destPos = 0;

    u16 buffer;
    u8 flags;
    int count;
    int i;

    for (;;)
    {
        flags = src8[srcPos++];
        count = (flags & 0x3F) + 1;

        switch ((flags & 0xC0) >> 6)
        {
            case 0:
                switch (flags)
                {
                    case 0:
                        incrementValue = (src8[srcPos + 1] << 8) | src8[srcPos];
                        srcPos += 2;
                        break;
                    case 1:
                        repeatValue = (src8[srcPos + 1] << 8) | src8[srcPos];
                        srcPos += 2;
                        break;
                    default:
                        goto fail;
                }
                break;
            case 1:
                if ((destPos + count) * 2 > destSize)
                    goto fail;

                for (i = 0; i < count; i++)
                {
                    buffer = 0xFF & src8[srcPos++];
                    buffer |= (0xFF & src8[srcPos++]) << 8;
                    dest16[destPos] = buffer;
                    destPos += 1;
                }

                break;
            case 2:
                if ((destPos + count) * 2 > destSize)
                    goto fail;

                for (i = 0; i < count; i++)
                {
                    dest16[destPos] = repeatValue;
                    destPos += 1;
                }
                break;
            case 3:
                if ((destPos + count) * 2 > destSize)
                    goto fail;

                for (i = 0; i < count; i++)
                {
                    dest16[destPos] = incrementValue;
                    destPos += 1;
                    incrementValue++;
                }
                break;
        }

        if (destPos * 2 == destSize)
            return;
    }

fail:
    ; // ???
}
