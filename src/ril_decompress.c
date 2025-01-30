#include "global.h"
#include "ril_decompress.h"

//__attribute__((optimize("-O3")))
void Ril16DecompressVram(const u32 *src, void *dest)
{
    const u8 *src8 = (const u8 *) src;
    u16 *dest16 = (u16 *) dest;

    u16* destEnd = dest16 + ((src8[3] << 16) | (src8[2] << 8) | src8[1]) / sizeof(u16);

    unsigned value;
    unsigned count;
    unsigned opcode;

    src8 += 4;

    while (dest16 < destEnd)
    {
        opcode = *(src8++);

        switch ((opcode & 0xC0) >> 6)
        {
            case 0:
                count = (opcode & 0x3F) + 1;

                if ((dest16 + count) > destEnd)
                    goto fail;

                for (; count > 0; count--)
                {
                    value = *(src8++);
                    value |= *(src8++) << 8;
                    *(dest16++) = value;
                }

                break;
            case 1:
                count = (opcode & 0x3F) + 3;

                value = *(src8++);
                value |= *(src8++) << 8;

                if ((dest16 + count) > destEnd)
                    goto fail;

                for (; count > 7; count -= 8)
                {
                    *(dest16++) = (value++);
                    *(dest16++) = (value++);
                    *(dest16++) = (value++);
                    *(dest16++) = (value++);
                    *(dest16++) = (value++);
                    *(dest16++) = (value++);
                    *(dest16++) = (value++);
                    *(dest16++) = (value++);
                }

                for (; count > 0; count--)
                    *(dest16++) = (value++);
                break;
            case 2:
                count = (opcode & 0x3F) + 3;

                value = *(src8++);
                value |= *(src8++) << 8;

                if ((dest16 + count) > destEnd)
                    goto fail;

                for (; count > 7; count -= 8)
                {
                    *(dest16++) = value;
                    *(dest16++) = value;
                    *(dest16++) = value;
                    *(dest16++) = value;
                    *(dest16++) = value;
                    *(dest16++) = value;
                    *(dest16++) = value;
                    *(dest16++) = value;
                }

                for (; count > 0; count--)
                    *(dest16++) = value;

                break;
            case 3:
                count = (opcode & 0x3F) + 3;

                value = *(src8++);
                value |= *(src8++) << 8;

                if ((dest16 + count) > destEnd)
                    goto fail;

                for (; count > 7; count -= 8)
                {
                    *(dest16++) = (value--);
                    *(dest16++) = (value--);
                    *(dest16++) = (value--);
                    *(dest16++) = (value--);
                    *(dest16++) = (value--);
                    *(dest16++) = (value--);
                    *(dest16++) = (value--);
                    *(dest16++) = (value--);
                }

                for (; count > 0; count--)
                    *(dest16++) = (value--);

                break;
        }
    }

    return;

fail:
    ; // ???
}
