#include "global.h"
#include "tilemap3_decompress.h"

#define DIFF_BIAS (8)
#define RUN_BIAS (-1)

#define NUM_REGS (64)
#define ENTRY_HASH(entry) (((entry & 0x3FF) + (((entry & 0xC00) >> 10) * 3) + (((entry & 0xF000) >> 12) * 5)) % NUM_REGS)

void Tilemap3Decompress(const u32 *src_32, void *dest_v)
{
    unsigned i;
    const u8* src = (const u8 *)src_32;
    u16* dest = (u16 *) dest_v;
    unsigned destSize = ((src[3] << 16) | (src[2] << 8) | src[1]) / sizeof(u16);

    unsigned short regs[NUM_REGS] = {0};
    unsigned short last = 0;

    unsigned srcPos = 4;
    unsigned destPos = 0;

    while (destPos < destSize)
    {
        unsigned char opbyte = src[srcPos++];

        if (opbyte == 0xFF)
        {
            last = src[srcPos++];
            last |= src[srcPos++] << 8;
            regs[ENTRY_HASH(last)] = last;
            dest[destPos++] = last;
        }
        else
        {
            switch (opbyte >> 6)
            {
            case 0:
                {
                    last = regs[opbyte & 0x3F];
                    dest[destPos++] = last;
                }
                break;
            case 1:
                {
                    unsigned short new_flips = (opbyte & 0x30) << 6;
                    signed short delta_index = (opbyte & 0x0F) - DIFF_BIAS;
                    last = ((last & 0xF3FF) + delta_index) | new_flips;
                    regs[ENTRY_HASH(last)] = last;
                    dest[destPos++] = last;
                }
                break;
            case 2:
                {
                    signed short delta = ((opbyte & 0x20) != 0 ? -1 : 1);
                    unsigned count = (opbyte & 0x1F) - RUN_BIAS;
                    for (i = 0; i < count; i++)
                    {
                        last = last + delta;
                        dest[destPos++] = last;
                        regs[ENTRY_HASH(last)] = last;
                    }
                }
                break;
            case 3:
                {
                    unsigned count = (opbyte & 0x3F) - RUN_BIAS;
                    for (i = 0; i < count; i++)
                    {
                        dest[destPos++] = last;
                    }
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