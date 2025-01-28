#include "global.h"
#include "tilemap2_decompress.h"

#define NUM_REGS (4)

void Tilemap2Decompress(const u32 *src_32, void *dest_v)
{
    const u16* src = (const u16 *)src_32;
    u16* dest = (u16 *) dest_v;
    unsigned destSize = ((src[1] << 8) | ((src[0] >> 8) & 0xFF)) / sizeof(u16);

    unsigned short regs[NUM_REGS] = {0};

    unsigned srcPos = 2;
    unsigned destPos = 0;

    while (destPos < destSize)
    {
        signed delta;
        unsigned i;
        unsigned short srcValue = src[srcPos++];
        unsigned op = (srcValue & 0xF000) >> 12;
        unsigned from = (srcValue & 0xC00) >> 10;
        unsigned to = (srcValue & 0x300) >> 8;
        unsigned operand = srcValue & 0xFF;

        switch (op) {
        case 0:
            regs[to] = regs[from] ^ (operand << 8);
            break;
        case 1:
            regs[to] = regs[from] ^ operand;
            break;
        case 12:
            delta = 0;
            goto write;
        case 13:
            delta = 1;
            goto write;
        case 15:
            delta = -1;
write:
            regs[to] = regs[from];
            for (i = 0; i <= operand; i++) {
                dest[destPos++] = regs[to];
                regs[to] += delta;
            }
            break;
        default:
            goto fail;
        }
    }

    return;

fail:
    ; // ???
}


void Tilemap2Decompress_Unrolled(const u32 *src_32, void *dest_v)
{
    const u16* src = (const u16 *)src_32;
    u16* dest = (u16 *) dest_v;
    u16* destEnd = dest + (((src[1] << 8) | ((src[0] >> 8) & 0xFF)) / sizeof(u16));

    // Did try unrolling this array too, but it seems that branching is slower than not using registers
    unsigned short reg[4] = {0};

    src += 2;

    while (dest < destEnd)
    {
        unsigned i;
        unsigned value;
        unsigned short srcValue = *src;
        unsigned srcValuePart;
        unsigned operand;

        srcValuePart = srcValue;
        srcValuePart <<= (32 - (10 + 2));
        srcValuePart >>= ((32 - 2));
        value = reg[srcValuePart];

        operand = srcValue & 0xFF;

        srcValuePart = srcValue;
        srcValuePart <<= (32 - (12 + 4));
        srcValuePart >>= (32 - 4);

        switch (srcValuePart) {
        case 0:
            value ^= (operand << 8);
            break;
        case 1:
            value ^= operand;
            break;
        case 12:
            for (i = 0; i <= operand; i++) {
                *dest = value;
                dest++;
            }
            break;
        case 13:
            for (i = 0; i <= operand; i++) {
                *dest = value;
                dest++;
                value += 1;
            }
            break;
        case 15:
            for (i = 0; i <= operand; i++) {
                *dest = value;
                dest++;
                value -= 1;
            }
            break;
        default:
            goto fail;
        }

        srcValuePart = srcValue;
        srcValuePart = srcValuePart << (32 - (8 + 2));
        srcValuePart = srcValuePart >> (32 - 2);
        reg[srcValuePart] = value;

        src++;
    }

    return;

fail:
    ; // ???
}
