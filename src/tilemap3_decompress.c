#include "global.h"
#include "tilemap3_decompress.h"

#define DIFF_BIAS (8)
#define RUN_BIAS (-1)

#define NUM_REGS (64)
#define ENTRY_HASH(entry) (((entry & 0x3FF) + (((entry & 0xC00) >> 10) * 3) + (((entry & 0xF000) >> 12) * 5)) % NUM_REGS)

void Tilemap3Decompress(const u32 *src_32, void *dest_v)
{
    const u8* src = (const u8 *)src_32;
    u16* dest = (u16 *) dest_v;
    u16* destEnd = dest + ((src[3] << 16) | (src[2] << 8) | src[1]) / sizeof(u16);

    unsigned short regs[NUM_REGS] = {0};
    unsigned short last = 0;

    src += 4;

    while (dest < destEnd)
    {
        unsigned char opbyte = *(src++);

        if (opbyte == 0xFF)
        {
            last = *(src++);
            last |= *(src++) << 8;
            regs[ENTRY_HASH(last)] = last;
            *(dest++) = last;
        }
        else
        {
            switch (opbyte >> 6)
            {
            case 0:
                {
                    last = regs[opbyte & 0x3F];
                    *(dest++) = last;
                }
                break;
            case 1:
                {
                    unsigned short xor_flips = (opbyte & 0x30) << 6;
                    signed short delta_index = (opbyte & 0x0F) - DIFF_BIAS;
                    last = (last + delta_index) ^ xor_flips;
                    regs[ENTRY_HASH(last)] = last;
                    *(dest++) = last;
                }
                break;
            case 2:
                {
                    signed short delta = ((opbyte & 0x20) != 0 ? -1 : 1);
                    unsigned count = (opbyte & 0x1F) - RUN_BIAS;
                    for (; count > 0; count--)
                    {
                        last = last + delta;
                        *(dest++) = last;
                        regs[ENTRY_HASH(last)] = last;
                    }
                }
                break;
            case 3:
                {
                    unsigned count = (opbyte & 0x3F) - RUN_BIAS;
                    for (; count > 7; count -= 8)
                    {
                        *(dest++) = last;
                        *(dest++) = last;
                        *(dest++) = last;
                        *(dest++) = last;
                        *(dest++) = last;
                        *(dest++) = last;
                        *(dest++) = last;
                        *(dest++) = last;
                    }
                    for (; count > 0; count--)
                        *(dest++) = last;
                }
                break;
            default:
                goto fail;
            }
        }
    }

    return;

fail:
    ; // ???
}