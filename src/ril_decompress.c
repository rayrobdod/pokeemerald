#include "global.h"
#include "ril_decompress.h"

void Ril8DecompressWram(const u32 *src, void *dest)
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
    // ???
}

void Ril8DecompressVram(const u32 *src, void *dest)
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
                        dest8[destPos++ / 2] = buffer;
                    }
                    else
                    {
                        buffer = 0xFF & src8[srcPos++];
                        destPos++;
                    }
                    bufferHasValue = !bufferHasValue;
                }

                break;
            case 2:
                if (destPos + count > destSize)
                    goto fail;

                for (i = 0; i < count; i++)
                {
                    if (bufferHasValue)
                    {
                        buffer |= (0xFF & repeatValue) << 8;
                        dest8[destPos++ / 2] = buffer;
                    }
                    else
                    {
                        buffer = 0xFF & repeatValue;
                        destPos++;
                    }
                    bufferHasValue = !bufferHasValue;
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
                        dest8[destPos++ / 2] = buffer;
                    }
                    else
                    {
                        buffer = 0xFF & incrementValue;
                        destPos++;
                    }
                    bufferHasValue = !bufferHasValue;
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
    // ???
}

void Ril16DecompressVram(const u32 *src, void *dest)
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
                if (destPos + count * 2 > destSize)
                    goto fail;

                for (i = 0; i < count; i++)
                {
                    buffer = 0xFF & src8[srcPos++];
                    buffer |= (0xFF & src8[srcPos++]) << 8;
                    dest16[destPos / 2] = buffer;
                    destPos += 2;
                }

                break;
            case 2:
                if (destPos + count * 2 > destSize)
                    goto fail;

                for (i = 0; i < count; i++)
                {
                    dest16[destPos / 2] = repeatValue;
                    destPos += 2;
                }
                break;
            case 3:
                if (destPos + count * 2 > destSize)
                    goto fail;

                for (i = 0; i < count; i++)
                {
                    dest16[destPos / 2] = incrementValue;
                    destPos += 2;
                    incrementValue++;
                }
                break;
        }

        if (destPos == destSize)
            return;
    }

fail:
    // ???
}
