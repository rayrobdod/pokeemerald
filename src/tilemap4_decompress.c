#include "global.h"
#include "tilemap4_decompress.h"

#define NUM_REGS (4)

#if MODERN
__attribute__((optimize("-O3")))
#endif
void Tilemap4Decompress(const u32 *src_32, void *dest_v)
{
    const u8* src = (const u8 *)src_32;
    u16* dest = (u16 *) dest_v;
    u16* destEnd = dest + ((src[3] << 16) | (src[2] << 8) | src[1]) / sizeof(u16);

    // Did try unrolling this array too, but it seems that branching is slower than not using registers
    unsigned short reg[4] = {0};

    src += 4;

    while (dest < destEnd)
    {
        unsigned opbyte = *(src++);
        unsigned opcode = opbyte;
        opcode <<= (32 - (6 + 2));
        opcode >>= (32 - 2);

        switch (opcode) {
        case 0:
            {
                unsigned from, to, hi, low, operand;
                from = opbyte;
                from <<= (32 - (2 + 4));
                from >>= (32 - 2);
                to = opbyte;
                to <<= (32 - (2 + 2));
                to >>= (32 - 2);
                operand = 0;
                hi = (opbyte & 0x2);
                low = (opbyte & 0x1);
                operand = (hi ? *(src++) << 8 : 0);
                operand |= (low ? *(src++) : 0);

                reg[to] = reg[from] ^ operand;
            }
            break;
        case 1:
            {
                unsigned from, length, value;
                from = opbyte;
                from <<= (32 - (2 + 4));
                from >>= (32 - 2);
                value = reg[from];
                length = opbyte;
                length <<= (32 - (0 + 4));
                length >>= (32 - 4);
                if (length == 0xF)
                {
                    length = *(src++) + 30;
                }
                length++;

                if (dest + length > destEnd)
                    goto fail;

                for(; length > 8; length -= 8)
                {
                    *(dest++) = value--;
                    *(dest++) = value--;
                    *(dest++) = value--;
                    *(dest++) = value--;
                    *(dest++) = value--;
                    *(dest++) = value--;
                    *(dest++) = value--;
                    *(dest++) = value--;
                }
                for(; length > 0; length -= 1)
                {
                    *(dest++) = value--;
                }

                reg[from] = value;
                break;
            }
        case 2:
            {
                unsigned from, length, value;
                from = opbyte;
                from <<= (32 - (2 + 4));
                from >>= (32 - 2);
                value = reg[from];
                length = opbyte;
                length <<= (32 - (0 + 4));
                length >>= (32 - 4);
                if (length == 0xF)
                {
                    length = *(src++) + 30;
                }
                length++;

                if (dest + length > destEnd)
                    goto fail;

                for(; length > 8; length -= 8)
                {
                    *(dest++) = value;
                    *(dest++) = value;
                    *(dest++) = value;
                    *(dest++) = value;
                    *(dest++) = value;
                    *(dest++) = value;
                    *(dest++) = value;
                    *(dest++) = value;
                }
                for(; length > 0; length -= 1)
                {
                    *(dest++) = value;
                }
                break;
            }
        case 3:
            {
                unsigned from, length, value;
                from = opbyte;
                from <<= (32 - (2 + 4));
                from >>= (32 - 2);
                value = reg[from];
                length = opbyte;
                length <<= (32 - (0 + 4));
                length >>= (32 - 4);
                if (length == 0xF)
                {
                    length = *(src++) + 30;
                }
                length++;

            if (dest + length > destEnd)
                goto fail;

                for(; length > 8; length -= 8)
                {
                    *(dest++) = value++;
                    *(dest++) = value++;
                    *(dest++) = value++;
                    *(dest++) = value++;
                    *(dest++) = value++;
                    *(dest++) = value++;
                    *(dest++) = value++;
                    *(dest++) = value++;
                }
                for(; length > 0; length -= 1)
                {
                    *(dest++) = value++;
                }

                reg[from] = value;
                break;
            }

        default:
            goto fail;
        }
    }

    return;

fail:
    ; // ???
}
