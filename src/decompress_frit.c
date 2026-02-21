#include "global.h"
#include "decompress_frit.h"

void Frit16UnComp(const u32 *srcV, void *destV)
{
    volatile u16* dest = (volatile u16*)destV;
    volatile u16* const dest_end = dest + ((srcV[0] >> 8) / sizeof(u16));
    const u8* src8 = (const u8*)(srcV + 1);

    // zero-initializing a `u16 x[4]` array translates into a memset call.
    // assigning each item to zero translates into two `strs` instructions
    // The latter is 200 cycles faster. out of tens of thousands of cycles, but still.
    u16 regs[4];
    regs[0] = 0;
    regs[1] = 0;
    regs[2] = 0;
    regs[3] = 0;

    while (dest < dest_end)
    {
        u8 op = *(src8++);
        unsigned op_code = op >> 6;

        if (0 == op_code)
        {
            unsigned from = op >> 4;
            unsigned to = (op >> 2) & 0x3;
            unsigned hi = (op & 0x2);
            unsigned low = (op & 0x1);
            unsigned hiValue = (hi ? *(src8++) << 8 : 0);
            unsigned lowValue = (low ? *(src8++) : 0);
            unsigned operand = hiValue | lowValue;

            regs[to] = regs[from] ^ operand;
        }
        else
        {
            unsigned regId = (op >> 4) & 0x3;
            unsigned regValue = regs[regId];
            unsigned length = (op & 0x0F) + 1;
            if (length == 0x10)
            {
                length = *(src8++) + 31;
            }

            if (2 == op_code)
            {
                // Did try 32-bit writes, but empirically branching to align is slower than
                // just doing more 16-bit writes

                // Benchmarks are happier if this loop ends with `length > 0` guaranteed
                for (; length > 4; length -= 4)
                {
                    *(dest++) = regValue;
                    *(dest++) = regValue;
                    *(dest++) = regValue;
                    *(dest++) = regValue;
                }
                for (; length > 0; --length)
                {
                    *(dest++) = regValue;
                }
            }
            else if (1 == op_code)
            {
                signed delta = ((signed) op_code) - 2;

                // Benchmarks are happier if this loop ends with `length > 0` guaranteed
                for (; length > 4; length -= 4)
                {
                    *(dest+0) = regValue;
                    *(dest+1) = regValue + delta;
                    *(dest+2) = regValue + delta * 2;
                    *(dest+3) = regValue + delta * 3;
                    dest += 4;
                    regValue += delta * 4;
                }
                for (; length > 0; --length)
                {
                    *(dest++) = regValue;
                    regValue += delta;
                }
                regs[regId] = regValue;
            }
            else
            {
                signed delta = ((signed) op_code) - 2;
                // looking at assembly indicates that gcc does know that `op_code == 3` and therefore `delta == 1` here
                // Being able to `+ 3` or `- 3` instead of `+ 3 * delta` seems to be a significant speedup

                // Benchmarks are happier if this loop ends with `length > 0` guaranteed
                for (; length > 4; length -= 4)
                {
                    *(dest+0) = regValue;
                    *(dest+1) = regValue + delta;
                    *(dest+2) = regValue + delta * 2;
                    *(dest+3) = regValue + delta * 3;
                    dest += 4;
                    regValue += delta * 4;
                }
                for (; length > 0; --length)
                {
                    *(dest++) = regValue;
                    regValue += delta;
                }
                regs[regId] = regValue;
            }
        }
    }
}

void Frit8UnCompWram(const u32 *srcV, void *destV)
{
    volatile u8* dest = (volatile u8*)destV;
    volatile u8* const dest_end = dest + ((srcV[0] >> 8) / sizeof(u8));
    const u8* src8 = (const u8*)(srcV + 1);

    // zero-initializing a `u16 x[4]` array translates into a memset call.
    // assigning each item to zero translates into two `strs` instructions
    // The latter is 200 cycles faster. out of tens of thousands of cycles, but still.
    u8 regs[4];
    regs[0] = 0;
    regs[1] = 0;
    regs[2] = 0;
    regs[3] = 0;

    while (dest < dest_end)
    {
        u8 op = *(src8++);
        unsigned op_code = op >> 6;

        if (0 == op_code)
        {
            unsigned from = op >> 4;
            unsigned to = (op >> 2) & 0x3;
            unsigned hi = (op & 0x2);
            unsigned low = (op & 0x1);
            unsigned hiValue = (hi ? src8++, 0 : 0);
            unsigned lowValue = (low ? *(src8++) : 0);
            unsigned operand = hiValue | lowValue;

            regs[to] = regs[from] ^ operand;
        }
        else
        {
            unsigned regId = (op >> 4) & 0x3;
            unsigned regValue = regs[regId];
            unsigned length = (op & 0x0F) + 1;
            if (length == 0x10)
            {
                length = *(src8++) + 31;
            }

            if (2 == op_code)
            {
                // Benchmarks are happier if this loop ends with `length > 0` guaranteed
                for (; length > 4; length -= 4)
                {
                    *(dest++) = regValue;
                    *(dest++) = regValue;
                    *(dest++) = regValue;
                    *(dest++) = regValue;
                }
                for (; length > 0; --length)
                {
                    *(dest++) = regValue;
                }
            }
            else if (1 == op_code)
            {
                signed delta = ((signed) op_code) - 2;

                // Benchmarks are happier if this loop ends with `length > 0` guaranteed
                for (; length > 4; length -= 4)
                {
                    *(dest+0) = regValue;
                    *(dest+1) = regValue + delta;
                    *(dest+2) = regValue + delta * 2;
                    *(dest+3) = regValue + delta * 3;
                    dest += 4;
                    regValue += delta * 4;
                }
                for (; length > 0; --length)
                {
                    *(dest++) = regValue;
                    regValue += delta;
                }
                regs[regId] = regValue;
            } else {
                signed delta = ((signed) op_code) - 2;
                // looking at assembly indicates that gcc does know that `op_code == 3` and therefore `delta == 1` here
                // Being able to `+ 3` or `- 3` instead of `+ 3 * delta` seems to be a significant speedup

                // Benchmarks are happier if this loop ends with `length > 0` guaranteed
                for (; length > 4; length -= 4)
                {
                    *(dest+0) = regValue;
                    *(dest+1) = regValue + delta;
                    *(dest+2) = regValue + delta * 2;
                    *(dest+3) = regValue + delta * 3;
                    dest += 4;
                    regValue += delta * 4;
                }
                for (; length > 0; --length)
                {
                    *(dest++) = regValue;
                    regValue += delta;
                }
                regs[regId] = regValue;
            }
        }
    }
}

void Frit8UnCompVram(const u32 *srcV, void *destV)
{
    volatile u16* dest = (volatile u16*)destV;
    volatile u16* const dest_end = dest + ((srcV[0] >> 8) / sizeof(u16));
    const u8* src8 = (const u8*)(srcV + 1);

    unsigned buffer = 0;
    bool32 buffer_has_value = FALSE;

    u8 regs[4] = {0};

    while (dest < dest_end)
    {
        u8 op = *(src8++);
        unsigned op_code = op >> 6;

        if (0 == op_code)
        {
            unsigned from = op >> 4;
            unsigned to = (op >> 2) & 0x3;
            unsigned hi = (op & 0x2);
            unsigned low = (op & 0x1);
            unsigned operand;
            if (hi) {src8++;}
            operand = (low ? *(src8++) : 0);

            regs[to] = regs[from] ^ operand;
        }
        else
        {
            unsigned regId = (op >> 4) & 0x3;
            unsigned length = (op & 0x0F) + 1;
            unsigned regValue = regs[regId];
            if (length == 0x10)
            {
                length = *(src8++) + 31;
            }

            if (2 == op_code)
            {
                if (length > 0 && buffer_has_value)
                {
                    buffer |= regValue << 8;
                    *(dest++) = buffer;
                    length--;
                }

                buffer = (regValue << 8) | regValue;

                for (; length > 4; length -= 4)
                {
                    *(dest++) = buffer;
                    *(dest++) = buffer;
                }
                for (; length > 1; length -= 2)
                {
                    *(dest++) = buffer;
                }

                buffer_has_value = length > 0;
                if (buffer_has_value)
                {
                    buffer = regValue;
                }
            }
            else
            {
                signed delta = ((signed) op_code) - 2;

                if (length > 0 && buffer_has_value)
                {
                    buffer |= regValue << 8;
                    *(dest++) = buffer;
                    length--;
                    regValue += delta;
                }

                for (; length > 1; length -= 2)
                {
                    buffer = regValue & 0xFF;
                    regValue += delta;
                    buffer |= regValue << 8;
                    regValue += delta;
                    *(dest++) = buffer;
                }

                buffer_has_value = length > 0;
                if (buffer_has_value)
                {
                    buffer = regValue & 0xFF;
                    regValue += delta;
                }
                regs[regId] = regValue;
            }
        }
    }
}
