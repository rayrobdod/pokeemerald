#include "compress.h"

#include <stdlib.h>
#include <stdbool.h>
#include "global.h"

/// All four registers initialized to 0x0000
///
/// XOR
///     || 0 0 | from:2 | to: 2 | hi | low || (if hi) Δhi : 8 || (if low) Δlow ||
/// `to <- from XOR (Δhi << 8 | Δlow)`
///
/// If hi is 0, do not serialize the Δhi byte, and treat as 0 instead
/// If low is 0, do not serialize the Δlow byte, and treat as 0 instead
///
/// RUN
///     || 1 0 | reg : 2 | lengthM1 : 4 || (if lengthM1 == F) lengthM31 : 8 ||
/// length times, write the value in register `reg` to output
///
/// If lengthM1 is not 0b1111, length is lengthM1+1, and lengthM31 is not serialized
/// Otherwise, length is lengthM31 + 31.
///
/// Use two consecutive `RUN`s to represent a RUN between 16 and 30 inclusive
///
/// SLIDE UP
///     || 1 1 | ... (same as RUN)
/// length times, write the value in register `reg` to output and increment the value in `reg`
///
/// SLIDE DOWN
///     || 0 1 | ... (same as RUN)
/// length times, write the value in register `reg` to output and decrement the value in `reg`

#define NUM_REGS (4)
#define LONGEST_RUN (1+14+16+255)

#define CAT1(A, B) A ## B
#define CAT(A, B) CAT1(A, B)

struct Runs
{
    char delta;
    unsigned short length;
    unsigned short start;
};

static unsigned findFirstRunWithStart(struct Runs* runs, unsigned length, unsigned short start)
{
    for (unsigned i = 0; i < length; i++)
    {
        if (runs[i].start == start)
            return i;
    }
    return 0xFFFFFF;
}

struct ARRAY CAT(decompress, SIZE)(struct ByteArray src, bool decompile)
{
    struct ARRAY dest;
    dest.size = ((src.buffer[3] << 16) | (src.buffer[2] << 8) | src.buffer[1]) / sizeof(unsigned TYPE);
    dest.buffer = malloc(sizeof(unsigned TYPE) * dest.size);

    if (dest.buffer == NULL)
        goto fail;

    unsigned short regs[NUM_REGS] = {0};

    unsigned destPos = 0;
    unsigned srcPos = 4;

    while (srcPos < src.size)
    {
        int delta;
        unsigned op = src.buffer[srcPos++];
        switch (op >> 6)
        {
        case 0:
            {
                unsigned from = (op & 0x30) >> 4;
                unsigned to = (op & 0xC) >> 2;
                unsigned hi = (op & 0x2);
                unsigned low = (op & 0x1);
                unsigned hiValue = (hi ? src.buffer[srcPos++] << 8 : 0);
                unsigned lowValue = (low ? src.buffer[srcPos++] : 0);
                unsigned operand = hiValue | lowValue;

                if (decompile)
                {
                    char hiStr[8] = "N/A";
                    char lowStr[8] = "N/A";
                    if (hi)
                        snprintf(hiStr, 8, "%02x", hiValue >> 8);
                    if (low)
                        snprintf(lowStr, 8, "%02x", lowValue);

                    printf("%04x %04x %04x %04x  %8d | XOR   %d->%d   %3s %3s\n",
                        regs[0], regs[1], regs[2], regs[3], destPos, from, to, hiStr, lowStr);
                }

                regs[to] = regs[from] ^ operand;
            }
            break;
        case 1:
            delta = -1;
            goto run;
        case 2:
            delta = 0;
            goto run;
        case 3:
            delta = 1;
        run:
            {
                unsigned reg = (op & 0x30) >> 4;
                unsigned length = (op & 0x0F) + 1;
                if (length == 0x10)
                {
                    length = src.buffer[srcPos++] + 31;
                }

                if (decompile)
                {
                    char deltaStr = (delta == 0 ? '0' : delta == 1 ? '+' : '-');

                    printf("%04x %04x %04x %04x  %8d | RUN%c  %d    %3d\n",
                        regs[0], regs[1], regs[2], regs[3], destPos, deltaStr, reg, length);
                }

                for (unsigned i = 0; i < length; i++)
                {
                    dest.buffer[destPos++] = regs[reg];
                    regs[reg] += delta;
                }
            }
            break;
        }
    }

    if (destPos != dest.size)
        FATAL_ERROR("dest sizes mismatch (was %d; claimed %zd)\n", destPos, dest.size);

    return dest;

fail:
    FATAL_ERROR("Fatal error while decompressing file.\n");
}

struct ByteArray CAT(compress, SIZE)(struct ARRAY src)
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
                while (srcPos + length < src.size && src.buffer[srcPos] == src.buffer[srcPos + length] && length < LONGEST_RUN)
                {
                    length++;
                }
                runs[runPos].length = length;
                runs[runPos].delta = 0;
                srcPos += length;
                break;
            case 1:
                while (srcPos + length < src.size && src.buffer[srcPos] + length == src.buffer[srcPos + length] && length < LONGEST_RUN)
                {
                    length++;
                }
                runs[runPos].length = length;
                runs[runPos].delta = 1;
                srcPos += length;
                break;
            case -1:
                while (srcPos + length < src.size && src.buffer[srcPos] == src.buffer[srcPos + length] + length && length < LONGEST_RUN)
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

    /* run optimization pass */
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

    struct ByteArray dest = {0};
    dest.buffer = malloc(sizeof(unsigned char) * runCount * 5 + 4);

    if (dest.buffer == NULL)
        goto fail;

    // header
    unsigned size_in_bytes = src.size * sizeof(unsigned TYPE);

    dest.buffer[0] = 0x00; // TODO compression type
    dest.buffer[1] = (unsigned char)size_in_bytes;
    dest.buffer[2] = (unsigned char)(size_in_bytes >> 8);
    dest.buffer[3] = (unsigned char)(size_in_bytes >> 16);

    size_t destPos = 4;

    unsigned short regs[NUM_REGS] = {0};

    for (runPos = 0; runPos < runCount; runPos++)
    {
        #ifdef DEBUG
            printf("%04x %04x %04x %04x || ", regs[0], regs[1], regs[2], regs[3]);
            printf("%04x - %3d - %2d\n", runs[runPos].start, runs[runPos].length, runs[runPos].delta);
        #endif
        unsigned runReg;
        for (runReg = 0; runReg < NUM_REGS; runReg++)
        {
            if (runs[runPos].start == regs[runReg])
                break;
        }

        if (runReg >= NUM_REGS)
        {
            if (regs[3] == regs[2] || regs[3] == regs[1] || regs[3] == regs[0])
                runReg = 3;
            else if (regs[2] == regs[1] || regs[2] == regs[0])
                runReg = 2;
            else if (regs[1] == regs[0])
                runReg = 1;

            if (runReg >= NUM_REGS)
            {
                unsigned nextRegUse[NUM_REGS];
                for (unsigned i = 0; i < NUM_REGS; i++)
                {
                    nextRegUse[i] = findFirstRunWithStart(runs + runPos, runCount - runPos, regs[i]);
                }

                runReg = 0;
                for (unsigned i = 1; i < NUM_REGS; i++)
                {
                    if (nextRegUse[i] > nextRegUse[runReg])
                        runReg = i;
                }
            }

            unsigned fromReg = runReg;
            for (unsigned i = 0; i < NUM_REGS; i++)
            {
                if ((runs[runPos].start & 0xFF) == (regs[i] & 0xFF))
                {
                    fromReg = i;
                    break;
                }
                if ((runs[runPos].start & 0xFF00) == (regs[i] & 0xFF00))
                {
                    fromReg = i;
                    break;
                }
            }

            unsigned hi = (runs[runPos].start & 0xFF00) ^ (regs[fromReg] & 0xFF00);
            unsigned low = (runs[runPos].start & 0xFF) ^ (regs[fromReg] & 0xFF);

            #ifdef DEBUG
                printf("    XOR %d->%d %04x\n", fromReg, runReg, hi | low);
            #endif

            dest.buffer[destPos++] = ((fromReg << 4) | (runReg << 2) | (hi ? 2 : 0) | (low ? 1 : 0));
            if (hi)
                dest.buffer[destPos++] = hi >> 8;
            if (low)
                dest.buffer[destPos++] = low;
        }

        char opcode = (2 + runs[runPos].delta) & 0x3;
        unsigned lengthSub1 = runs[runPos].length - 1;

        if (lengthSub1 < 15)
        {
            dest.buffer[destPos++] = (opcode << 6) | (runReg << 4) | lengthSub1;
            #ifdef DEBUG
                printf("    RUN %d %d %d\n", opcode, runReg, lengthSub1);
            #endif
        }
        else
        {
            unsigned lengthSub16 = lengthSub1 - 15;

            if (lengthSub16 < 15)
            {
                dest.buffer[destPos++] = (opcode << 6) | (runReg << 4) | 0xE;
                dest.buffer[destPos++] = (opcode << 6) | (runReg << 4) | lengthSub16;
                #ifdef DEBUG
                    printf("    RUN %d %d 14\n", opcode, runReg);
                    printf("    RUN %d %d %d\n", opcode, runReg, lengthSub16);
                #endif
            }
            else
            {
                unsigned lengthSub31 = lengthSub16 - 15;
                dest.buffer[destPos++] = (opcode << 6) | (runReg << 4) | 0xF;
                dest.buffer[destPos++] = lengthSub31;
                #ifdef DEBUG
                    printf("    RUN %d %d 15 [%d]\n", opcode, runReg, lengthSub31);
                #endif
            }
        }

        unsigned short endValue = runs[runPos].start + runs[runPos].delta * runs[runPos].length;
        regs[runReg] = endValue;
    }

    free(runs);
    // Round up to the next multiple of four bytes
    while (destPos % 4 != 0)
        dest.buffer[destPos++] = 0;
    dest.size = destPos;
    return dest;

fail:
    FATAL_ERROR("Fatal error while compressing file.\n");
}
