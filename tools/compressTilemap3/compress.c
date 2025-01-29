#include "compress.h"

#include <stdlib.h>
#include <stdbool.h>
#include "global.h"

/*
  Very much takes after Quite OK Image

  OP_RUN       || 1 1 | {6} run ||
  OP_SLIDE     || 1 0 | +/- | {5} run ||
  OP_DIFF      || 0 1 | {2} flips | {4} d_index ||
  OP_INDEX     || 0 0 | {6} index ||
  OP_RAW       || 1 1 1 1 1 1 1 1 || byte 1 || byte 2 ||

*/

enum RunOrSlide {
    ROS_NONE,
    ROS_RUN,
    ROS_SLIDE_POS,
    ROS_SLIDE_NEG,
};

#define MAX_STRAIGHT_RUN_LENGTH (62)
#define MAX_DELTA_RUN_LENGTH (32)

#define DIFF_BIAS (8)
#define RUN_BIAS (-1)

#define NUM_REGS (64)
#define ENTRY_HASH(entry) (((entry & 0x3FF) + (((entry & 0xC00) >> 10) * 3) + (((entry & 0xF000) >> 12) * 5)) % NUM_REGS)

struct ShortArray decompress(struct ByteArray src, bool disassemble)
{
    struct ShortArray dest;
    dest.size = ((src.buffer[3] << 16) | (src.buffer[2] << 8) | src.buffer[1]) / sizeof(unsigned short);
    dest.buffer = malloc(sizeof(unsigned short) * dest.size);

    if (dest.buffer == NULL)
        goto fail;

    unsigned short regs[NUM_REGS] = {0};
    unsigned short last = 0;

    unsigned srcPos = 4;
    unsigned destPos = 0;

    while (srcPos < src.size && destPos < dest.size)
    {
        unsigned char opbyte = src.buffer[srcPos++];

        if (opbyte == 0xFF)
        {
            last = src.buffer[srcPos++];
            last |= src.buffer[srcPos++] << 8;
            if (disassemble)
                printf("OP_RAW   %04x\n", last);
            regs[ENTRY_HASH(last)] = last;
            dest.buffer[destPos++] = last;
        }
        else
        {
            switch (opbyte >> 6)
            {
            case 0:
                {
                    if (disassemble)
                        printf("OP_INDEX   %2d  (%04x)\n", opbyte & 0x3F, regs[opbyte & 0x3F]);
                    last = regs[opbyte & 0x3F];
                    dest.buffer[destPos++] = last;
                }
                break;
            case 1:
                {
                    unsigned short xor_flips = (opbyte & 0x30) << 6;
                    signed short delta_index = (opbyte & 0x0F) - DIFF_BIAS;
                    if (disassemble)
                        printf("OP_DELTA %d %2d\n", xor_flips >> 10, delta_index);
                    last = (last + delta_index) ^ xor_flips;
                    regs[ENTRY_HASH(last)] = last;
                    dest.buffer[destPos++] = last;
                }
                break;
            case 2:
                {
                    signed short delta = ((opbyte & 0x20) != 0 ? -1 : 1);
                    unsigned count = (opbyte & 0x1F) - RUN_BIAS;
                    if (disassemble)
                        printf("OP_SLIDE %s %2d\n", (delta > 0 ? "+" : "-"), count);
                    for (unsigned i = 0; i < count; i++)
                    {
                        last = last + delta;
                        dest.buffer[destPos++] = last;
                        regs[ENTRY_HASH(last)] = last;
                    }
                }
                break;
            case 3:
                {
                    unsigned count = (opbyte & 0x3F) - RUN_BIAS;
                    if (disassemble)
                        printf("OP_RUN     %2d\n", count);
                    for (unsigned i = 0; i < count; i++)
                    {
                        dest.buffer[destPos++] = last;
                    }
                }
                break;
            default:
                goto fail;
            }
        }
    }

    if (destPos != dest.size)
        FATAL_ERROR("dest sizes mismatch (was %d; claimed %zd)\n", destPos, dest.size);

    return dest;

fail:
    FATAL_ERROR("Fatal error while decompressing file.\n");
}

struct ByteArray compress(struct ShortArray src)
{
    if (src.size <= 0)
        goto fail;

    struct ByteArray dest = {0};
    dest.buffer = malloc(src.size * 3 + 4);

    if (dest.buffer == NULL)
        goto fail;

    // header
    unsigned size_in_bytes = src.size * sizeof(unsigned short);

    dest.buffer[0] = 0x00; // TODO compression type
    dest.buffer[1] = (unsigned char)size_in_bytes;
    dest.buffer[2] = (unsigned char)(size_in_bytes >> 8);
    dest.buffer[3] = (unsigned char)(size_in_bytes >> 16);

    size_t destPos = 4;

    unsigned short regs[NUM_REGS] = {0};
    unsigned short last = 0;
    enum RunOrSlide prev_op_is_a_run = ROS_NONE;

    for (size_t srcPos = 0; srcPos < src.size; srcPos++)
    {
        if (prev_op_is_a_run == ROS_RUN && last == src.buffer[srcPos]
                && dest.buffer[destPos - 1] < 0xFC)
        {
            (dest.buffer[destPos - 1])++;
        }
        else if (prev_op_is_a_run == ROS_SLIDE_POS && last + 1 == src.buffer[srcPos]
                && dest.buffer[destPos - 1] < 0x9F)
        {
            (dest.buffer[destPos - 1])++;
        }
        else if (prev_op_is_a_run == ROS_SLIDE_NEG && last - 1 == src.buffer[srcPos]
                && dest.buffer[destPos - 1] < 0xBF)
        {
            (dest.buffer[destPos - 1])++;
        }
        else if (last == src.buffer[srcPos])
        {
            prev_op_is_a_run = ROS_RUN;
            dest.buffer[destPos++] = 0xC0;
        }
        else if (last + 1 == src.buffer[srcPos])
        {
            prev_op_is_a_run = ROS_SLIDE_POS;
            dest.buffer[destPos++] = 0x80;
        }
        else if (last - 1 == src.buffer[srcPos])
        {
            prev_op_is_a_run = ROS_SLIDE_NEG;
            dest.buffer[destPos++] = 0xA0;
        }
        else if ((last & 0xF000) == (src.buffer[srcPos] & 0xF000) &&
                (last & 0x03FF) >= (src.buffer[srcPos] & 0x03FF) &&
                ((last & 0x03FF) - (src.buffer[srcPos] & 0x03FF)) < 8)
        {
            prev_op_is_a_run = ROS_NONE;
            unsigned prev_flips = (last & 0xC00) >> 6;
            unsigned new_flips = (src.buffer[srcPos] & 0xC00) >> 6;
            unsigned delta = DIFF_BIAS - ((last & 0x03FF) - (src.buffer[srcPos] & 0x03FF));
            dest.buffer[destPos++] = 0x40 | (prev_flips ^ new_flips) | delta;
        }
        else if ((src.buffer[srcPos] & 0xF000) == (last & 0xF000) &&
                (src.buffer[srcPos] & 0x03FF) >= (last & 0x03FF) &&
                ((src.buffer[srcPos] & 0x03FF) - (last & 0x03FF)) < 8)
        {
            prev_op_is_a_run = ROS_NONE;
            unsigned prev_flips = (last & 0xC00) >> 6;
            unsigned new_flips = (src.buffer[srcPos] & 0xC00) >> 6;
            unsigned delta = DIFF_BIAS + (src.buffer[srcPos] & 0x03FF) - (last & 0x03FF);
            dest.buffer[destPos++] = 0x40 | (prev_flips ^ new_flips) | delta;
        }
        else
        {
            prev_op_is_a_run = ROS_NONE;

            unsigned regPos;
            for (regPos = 0; regPos < NUM_REGS; regPos++)
                if (regs[regPos] == src.buffer[srcPos] && regPos == ENTRY_HASH(regs[regPos]))
                    break;

            if (regPos < NUM_REGS)
            {
                dest.buffer[destPos++] = regPos;
            }
            else
            {
                dest.buffer[destPos++] = 0xFF;
                dest.buffer[destPos++] = src.buffer[srcPos] & 0xFF;
                dest.buffer[destPos++] = (src.buffer[srcPos] >> 8) & 0xFF;
            }
        }

        last = src.buffer[srcPos];
        regs[ENTRY_HASH(last)] = last;
    }

    // Round up to the next multiple of four bytes
    while (destPos % 4 != 0)
        dest.buffer[destPos++] = 0;
    dest.size = destPos;
    return dest;

fail:
    FATAL_ERROR("Fatal error while compressing file.\n");
}
