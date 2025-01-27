#include "compress.h"

#include <stdlib.h>
#include <stdbool.h>
#include "global.h"

#define OP_XOR_HI(from, to, operand) ((0 << 12) | ((from) << 10) | ((to) << 8) | (operand))
#define OP_XOR_LO(from, to, operand) ((1 << 12) | ((from) << 10) | ((to) << 8) | (operand))
#define OP_WRITE(delta, from, to, count) ((3 << 14) | (((delta) == 0 ? 0 : (delta) == 1 ? 1 : 3) << 12) | ((from) << 10) | ((to) << 8) | (count - 1))

#define NUM_REGS (4)

struct Runs
{
    char delta;
    unsigned char length;
    unsigned short start;
};

struct ShortArray decompress(struct ShortArray src)
{
    struct ShortArray dest;
    dest.size = ((src.buffer[1] << 8) | ((src.buffer[0] >> 8) & 0xFF)) / 2;
    dest.buffer = malloc(sizeof(unsigned short) * dest.size);

    if (dest.buffer == NULL)
        goto fail;

    unsigned short regs[NUM_REGS] = {0};

    unsigned destPos = 0;

    for (int srcPos = 2; srcPos < src.size; srcPos++)
    {
        signed delta;
        unsigned short srcValue = src.buffer[srcPos];
        unsigned op = (srcValue & 0xF000) >> 12;
        unsigned from = (srcValue & 0xC00) >> 10;
        unsigned to = (srcValue & 0x300) >> 8;
        unsigned operand = srcValue & 0xFF;

        switch (op) {
        case 0:
            {
                printf("XOR_HI    %d->%d  0x%02x -- %04x %04x %04x %04x\n", from, to, operand, regs[0], regs[1], regs[2], regs[3]);
                regs[to] = regs[from] ^ (operand << 8);
            }
            break;
        case 1:
            {
                printf("XOR_LO    %d->%d  0x%02x -- %04x %04x %04x %04x\n", from, to, operand, regs[0], regs[1], regs[2], regs[3]);
                regs[to] = regs[from] ^ operand;
            }
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
            {
                unsigned count = operand;
                printf("WRITE %2d  %d->%d   %3d -- %04x %04x %04x %04x\n", delta, from, to, count + 1, regs[0], regs[1], regs[2], regs[3]);
                regs[to] = regs[from];
                for (int i = 0; i <= count; i++) {
                    dest.buffer[destPos++] = regs[to];
                    regs[to] += delta;
                }
            }
            break;
        default:
            goto fail;
        }
    }

    if (destPos != dest.size)
        FATAL_ERROR("dest sizes mismatch (was %d; claimed %zd)\n", destPos, dest.size);

    return dest;

fail:
    FATAL_ERROR("Fatal error while decompressing file.\n");
}

struct ShortArray compress(struct ShortArray src)
{
    if (src.size <= 0)
        goto fail;

    struct Runs* runs = malloc(sizeof(struct Runs) * src.size);

    if (runs == NULL)
        goto fail;

    size_t srcPos = 0;
    size_t runPos = 0;

    while (srcPos < src.size)
    {
        unsigned int length = 1;

        runs[runPos].start = src.buffer[srcPos];

        if (srcPos + 1 < src.size)
        {
            switch (src.buffer[srcPos + 1] - src.buffer[srcPos])
            {
            case 0:
                while (srcPos + length < src.size && src.buffer[srcPos] == src.buffer[srcPos + length] && length < 0xFF)
                {
                    length++;
                }
                runs[runPos].length = length;
                runs[runPos].delta = 0;
                srcPos += length;
                break;
            case 1:
                while (srcPos + length < src.size && src.buffer[srcPos] + length == src.buffer[srcPos + length] && length < 0xFF)
                {
                    length++;
                }
                runs[runPos].length = length;
                runs[runPos].delta = 1;
                srcPos += length;
                break;
            case -1:
                while (srcPos + length < src.size && src.buffer[srcPos] == src.buffer[srcPos + length] + length && length < 0xFF)
                {
                    length++;
                }
                runs[runPos].length = length;
                runs[runPos].delta = -1;
                srcPos += length;
                break;
            default:
                runs[runPos].length = 1;
                runs[runPos].delta = 0;
                srcPos++;
            }
        }
        else
        {
            runs[runPos].length = 1;
            runs[runPos].delta = 0;
            srcPos++;
        }
        runPos++;
    }

    const int runCount = runPos;

    for (runPos = 0; runPos < runCount; runPos++)
    {
        if (1 == runs[runPos].length)
        {
            // If this 1-length run could end at the next run's starting point, it would have been merged with that run, I think
            if (runPos + 2 < runCount && runs[runPos + 2].start == runs[runPos].start + 1)
            {
                runs[runPos].delta = 1;
            }
            else if (runPos + 2 < runCount && runs[runPos + 2].start == runs[runPos].start - 1)
            {
                runs[runPos].delta = -1;
            }
        }

        if (runPos + 1 < runCount &&
            0 == runs[runPos].delta &&
            1 < runs[runPos].length &&
            1 == runs[runPos + 1].delta &&
            runs[runPos].start + runs[runPos + 1].delta == runs[runPos + 1].start &&
            true)
        {
            runs[runPos].length -= 1;
            runs[runPos+1].length += 1;
            runs[runPos+1].start -= runs[runPos+1].delta;
        }
    }

#ifdef DEBUG
    for (int i = 0; i < runCount; i++) {
        printf("%04x - %3d - %d\n", runs[i].start, runs[i].length, runs[i].delta);
    }
#endif

    struct ShortArray dest = {0};
    dest.buffer = malloc(sizeof(unsigned short) * runCount * 3 + 2);

    if (dest.buffer == NULL)
        goto fail;

    // header
    dest.buffer[0] = (unsigned short)((2 * src.size) << 8) | 0x00;
    dest.buffer[1] = (unsigned short)((2 * src.size) >> 8);

    unsigned int destPos = 2;

    unsigned short regs[NUM_REGS] = {0};

    unsigned leastRecentlyUsed[NUM_REGS] = {0};
    unsigned nextLRUvalue = 1;

    for (runPos = 0; runPos < runCount; runPos++)
    {
        #ifdef DEBUG
            printf("%04x %04x %04x %04x\n", regs[0], regs[1], regs[2], regs[3]);
        #endif
        unsigned toReg = 0;
        if (leastRecentlyUsed[1] < leastRecentlyUsed[toReg])
            toReg = 1;
        if (leastRecentlyUsed[2] < leastRecentlyUsed[toReg])
            toReg = 2;
        if (leastRecentlyUsed[3] < leastRecentlyUsed[toReg])
            toReg = 3;

        unsigned fromReg;
        for (fromReg = 0; fromReg < NUM_REGS; fromReg++)
        {
            if (runs[runPos].start == regs[fromReg])
                break;
        }

        if (fromReg < NUM_REGS)
        {
            unsigned short endValue = regs[fromReg] + runs[runPos].delta * runs[runPos].length;
            unsigned matchEndValueReg;
            for (matchEndValueReg = 0; matchEndValueReg < NUM_REGS; matchEndValueReg++)
            {
                if (endValue == regs[matchEndValueReg])
                    break;
            }
            if (matchEndValueReg < NUM_REGS)
                toReg = matchEndValueReg;

            dest.buffer[destPos++] = OP_WRITE(runs[runPos].delta, fromReg, toReg, runs[runPos].length);
            regs[toReg] = endValue;
            leastRecentlyUsed[fromReg] = (nextLRUvalue++);
            leastRecentlyUsed[toReg] = nextLRUvalue;
        }
        else
        {
            for (fromReg = 0; fromReg < NUM_REGS; fromReg++) {
                if ((regs[fromReg] & 0xFF) == (runs[runPos].start & 0xFF))
                    break;
                if ((regs[fromReg] & 0xFF00) == (runs[runPos].start & 0xFF00))
                    break;
            }
            if (fromReg >= NUM_REGS)
                fromReg = 0;

            #ifdef DEBUG
                printf("        to: %d %04x\n", toReg, regs[toReg]);
                printf("      from: %d %04x\n", fromReg, regs[fromReg]);
                printf("    target:   %04x\n", runs[runPos].start);
                printf("       xor:   %04x\n",  runs[runPos].start ^ regs[fromReg]);
            #endif

            if ((regs[fromReg] & 0xFF) == (runs[runPos].start & 0xFF))
            {
                dest.buffer[destPos++] = OP_XOR_HI(fromReg, toReg, (regs[fromReg] ^ runs[runPos].start) >> 8);
                regs[toReg] = (runs[runPos].start & 0xFF00) | (regs[fromReg] & 0xFF);
            }
            else
            {
                unsigned xorValue = (regs[fromReg] ^ runs[runPos].start) & 0xFF;

                dest.buffer[destPos++] = OP_XOR_LO(fromReg, toReg, xorValue);
                regs[toReg] = (runs[runPos].start & 0xFF) | (regs[fromReg] & 0xFF00);
            }

            leastRecentlyUsed[fromReg] = (nextLRUvalue++);
            leastRecentlyUsed[toReg] = nextLRUvalue;

            runPos--;
        }
    }

    free(runs);
    // Round up to the next multiple of four bytes
    if (destPos % 2 != 0)
        dest.buffer[destPos++] = 0;
    dest.size = destPos;
    return dest;

fail:
    FATAL_ERROR("Fatal error while compressing file.\n");
}
