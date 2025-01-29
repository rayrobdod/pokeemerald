#include "global.h"
#include "tilemap1_decompress.h"

//__attribute__((optimize("-O3")))
void Tilemap1_8DecompressWram(const u32 *src, void *dest)
{
    const u8 *src8 = (const u8 *) src;
    u8 *dest8 = (u8 *) dest;

    u8 *destEnd = dest8 + ((src8[3] << 16) | (src8[2] << 8) | src8[1]);

    unsigned incrementValue = 0;
    u8 repeatValue = 0;

    u8 flags;
    unsigned count;

    src8 += 4;

    for (;;)
    {
        flags = *(src8++);
        count = (flags & 0x3F) + 1;

        switch ((flags & 0xC0) >> 6)
        {
            case 0:
                switch (flags)
                {
                    case 0:
                        incrementValue = *(src8++);
                        break;
                    case 1:
                        repeatValue = *(src8++);
                        break;
                    default:
                        goto fail;
                }
                break;
            case 1:
                if (dest8 + count > destEnd)
                    goto fail;

                for (; count > 8; count -= 8)
                {
                    *(dest8++) = *(src8++);
                    *(dest8++) = *(src8++);
                    *(dest8++) = *(src8++);
                    *(dest8++) = *(src8++);
                    *(dest8++) = *(src8++);
                    *(dest8++) = *(src8++);
                    *(dest8++) = *(src8++);
                    *(dest8++) = *(src8++);
                }

                for (; count > 0; count--)
                    *(dest8++) = *(src8++);

                break;
            case 2:
                if (dest8 + count > destEnd)
                    goto fail;

                // the compiler's unrolling is smarter than mine
                for (; count > 0; count--)
                    *(dest8++) = repeatValue;

                break;
            case 3:
                if (dest8 + count > destEnd)
                    goto fail;

                // the compiler's unrolling is smarter than mine
                for (; count > 0; count--)
                    *(dest8++) = (incrementValue++);

                break;
        }

        if (dest8 == destEnd)
        {
            return;
        }
    }

fail:
    ; // ???
}

//__attribute__((optimize("-O3")))
void Tilemap1_8DecompressVram(const u32 *src, void *dest)
{
    const u8 *src8 = (const u8 *) src;
    u16 *dest16 = (u16 *) dest;

    u16 *destEnd = dest16 + ((src8[3] << 16) | (src8[2] << 8) | src8[1]) / sizeof(u16);

    unsigned incrementValue = 0;
    unsigned repeatValue = 0;

    u8 flags;
    bool8 bufferHasValue = FALSE;
    unsigned count;
    unsigned buffer;

    src8 += 4;

    for (;;)
    {
        flags = *(src8++);
        count = (flags & 0x3F) + 1;

        switch ((flags & 0xC0) >> 6)
        {
        case 0:
            switch (flags)
            {
            case 0:
                incrementValue = *(src8++);
                break;
            case 1:
                repeatValue = *(src8++);
                break;
            default:
                goto fail;
            }
            break;
        case 1:
            if (dest16 + count / sizeof(u16) > destEnd)
                goto fail;

            if (bufferHasValue)
            {
                *(dest16++) = buffer | (0xFF & *(src8++)) << 8;
                count --;
            }

            bufferHasValue = count % 2;
            count /= 2;

            for (; count > 0; count--)
            {
                buffer = *(src8++);
                buffer |= *(src8++) << 8;
                *(dest16++) = buffer;
            }

            if (bufferHasValue)
            {
                buffer = *(src8++);
            }

            break;
        case 2:
            if (dest16 + count / sizeof(u16) > destEnd)
                goto fail;

            if (bufferHasValue)
            {
                *(dest16++) = buffer | (0xFF & repeatValue) << 8;
                count--;
            }

            bufferHasValue = count % 2;
            count /= 2;

            buffer = (repeatValue << 8) | repeatValue;
            for (; count > 3; count -= 4)
            {
                *(dest16++) = buffer;
                *(dest16++) = buffer;
                *(dest16++) = buffer;
                *(dest16++) = buffer;
            }
            for (; count > 0; count--)
            {
                *(dest16++) = buffer;
            }

            if (bufferHasValue)
            {
                buffer = repeatValue;
            }

            break;
        case 3:
            if (dest16 + count / sizeof(u16) > destEnd)
                goto fail;

            if (bufferHasValue)
            {
                *(dest16++) = buffer | (0xFF & incrementValue++) << 8;
                count--;
            }

            bufferHasValue = count % 2;
            count /= 2;

            for (; count > 0; count--)
            {
                buffer = (incrementValue++);
                buffer |= (incrementValue++) << 8;
                *(dest16++) = buffer;
            }

            if (bufferHasValue)
            {
                buffer = incrementValue++;
            }
            break;
        }

        if (dest16 == destEnd)
        {
            return;
        }
    }

fail:
    ; // ???
}

//__attribute__((optimize("-O3")))
void Tilemap1_16DecompressVram(const u32 *src, void *dest)
{
    const u8 *src8 = (const u8 *) src;
    u16 *dest16 = (u16 *) dest;

    u16* destEnd = dest16 + ((src8[3] << 16) | (src8[2] << 8) | src8[1]) / sizeof(u16);

    unsigned incrementValue = 0;
    unsigned repeatValue = 0;

    u16 buffer;
    u8 flags;
    int count;

    src8 += 4;

    while (dest16 < destEnd)
    {
        flags = *(src8++);
        count = (flags & 0x3F) + 1;

        switch ((flags & 0xC0) >> 6)
        {
            case 0:
                switch (flags)
                {
                    case 0:
                        incrementValue = *(src8++);
                        incrementValue |= *(src8++) << 8;
                        break;
                    case 1:
                        repeatValue = *(src8++);
                        repeatValue |= *(src8++) << 8;
                        break;
                    default:
                        goto fail;
                }
                break;
            case 1:
                if ((dest16 + count) > destEnd)
                    goto fail;

                for (; count > 0; count--)
                {
                    buffer = *(src8++);
                    buffer |= *(src8++) << 8;
                    *(dest16++) = buffer;
                }

                break;
            case 2:
                if ((dest16 + count) > destEnd)
                    goto fail;

                for (; count > 8; count -= 8)
                {
                    *(dest16++) = repeatValue;
                    *(dest16++) = repeatValue;
                    *(dest16++) = repeatValue;
                    *(dest16++) = repeatValue;
                    *(dest16++) = repeatValue;
                    *(dest16++) = repeatValue;
                    *(dest16++) = repeatValue;
                    *(dest16++) = repeatValue;
                }

                for (; count > 0; count--)
                    *(dest16++) = repeatValue;

                break;
            case 3:
                if ((dest16 + count) > destEnd)
                    goto fail;

                for (; count > 8; count -= 8)
                {
                    *(dest16++) = (incrementValue++);
                    *(dest16++) = (incrementValue++);
                    *(dest16++) = (incrementValue++);
                    *(dest16++) = (incrementValue++);
                    *(dest16++) = (incrementValue++);
                    *(dest16++) = (incrementValue++);
                    *(dest16++) = (incrementValue++);
                    *(dest16++) = (incrementValue++);
                }

                for (; count > 0; count--)
                    *(dest16++) = (incrementValue++);
                break;
        }
    }

    return;

fail:
    ; // ???
}
