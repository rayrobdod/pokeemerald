
#include "ril.h"

#include <stdlib.h>
#include <stdbool.h>
#include "global.h"

/* Run-Increment-Length encoding
 *
 * There are two variants. ril8 has a word size of 8 bits, and ril16 has a word size of 16 bits
 *
 * The format has two registers, `increment` and `repeat`. Both are initialized to zero, and are word-sized
 *
 * Starts with the same four bytes as the other compression formats: one magic number then three length bytes
 * The rest of the file is a sequence of instructions
 *
 * 0b00000000 {word} - Set incrementValue to the word
 * 0b00000001 {word} - Set repeatValue to B
 * 0b01AAAAAA {word}*{A + 1} - Write the next (A + 1) word literally
 * 0b10AAAAAA - (A + 1) times, write repeatValue
 * 0b11AAAAAA - A + 1 times, write then increment incrementValue

 * Other instructions are reserved and MUST NOT appear in compressed files
 */

static unsigned char *RILDecompress(unsigned char *src, int srcSize, int *uncompressedSize, int wordSize);
static unsigned char *RILCompress(unsigned char *src, int srcSize, int *compressedSize, int wordSize);

unsigned char *RIL8Decompress(unsigned char *src, int srcSize, int *uncompressedSize) {
    return RILDecompress(src, srcSize, uncompressedSize, 1);
}
unsigned char *RIL8Compress(unsigned char *src, int srcSize, int *compressedSize) {
    return RILCompress(src, srcSize, compressedSize, 1);
}

unsigned char *RIL16Decompress(unsigned char *src, int srcSize, int *uncompressedSize) {
    return RILDecompress(src, srcSize, uncompressedSize, 2);
}
unsigned char *RIL16Compress(unsigned char *src, int srcSize, int *compressedSize) {
    return RILCompress(src, srcSize, compressedSize, 2);
}

#define MAX_LENGTH_MINUS_ONE (0x3F)

#define NEXT_SRC_WORD (wordSize == 2 ? (src[srcPos + 1] << 8) | src[srcPos] : src[srcPos])
#define NEXT_NEXT_SRC_WORD (wordSize == 2 ? (src[srcPos + 3] << 8) | src[srcPos + 2] : src[srcPos + 1])

static unsigned char *RILDecompress(unsigned char *src, int srcSize, int *uncompressedSize, int wordSize)
{
    if (srcSize < 4)
        FATAL_ERROR("Source did not have at least four bytes while decompressing RIL file.\n");

    int destSize = (src[3] << 16) | (src[2] << 8) | src[1];

    unsigned char *dest = malloc(destSize);

    if (dest == NULL)
        goto fail;

    unsigned short incrementValue = 0;
    unsigned short repeatValue = 0;

    int srcPos = 4;
    int destPos = 0;

    for (;;)
    {
        if (srcPos >= srcSize) {
            printf("(destPos = %d; destSize = %d)\n", destPos, destSize);
            FATAL_ERROR("Reached end of input while decompressing RIL file.\n");
        }

        unsigned char flags = src[srcPos++];
        int count = (flags & 0x3F) + 1;

        printf("%02X ", flags);

        switch ((flags & 0xC0) >> 6)
        {
            case 0:
                switch (flags)
                {
                    case 0:
                        if (srcPos + wordSize >= srcSize)
                            goto fail;
                        incrementValue = NEXT_SRC_WORD;
                        printf("%04X ", incrementValue);
                        srcPos += wordSize;
                        break;
                    case 1:
                        if (srcPos + wordSize >= srcSize)
                            goto fail;
                        repeatValue = NEXT_SRC_WORD;
                        printf("%04X ", repeatValue);
                        srcPos += wordSize;
                        break;
                    default:
                        FATAL_ERROR("Unexpected command while decompressing RIL file.\n");
                }
                break;
            case 1:
                if (srcPos + count * wordSize >= srcSize)
                    FATAL_ERROR("src overrun while decompressing RIL file.\n");
                if (destPos + count * wordSize > destSize)
                    FATAL_ERROR("dest overrun while decompressing RIL file.\n");

                for (int i = 0; i < count * wordSize; i++)
                {
                    printf("%02X ", src[srcPos]);
                    dest[destPos++] = src[srcPos++];
                }

                break;
            case 2:
                printf("(count = %02X) ", count);
                if (destPos + count * wordSize > destSize)
                    FATAL_ERROR("dest overrun while decompressing RIL file.\n");

                for (int i = 0; i < count; i++)
                {
                    dest[destPos++] = 0xFF & repeatValue;
                    if (wordSize >= 2)
                    {
                        dest[destPos++] = 0xFF & (repeatValue >> 8);
                    }
                }
                break;
            case 3:
                printf("(count = %02X) ", count);
                if (destPos + count * wordSize > destSize)
                    FATAL_ERROR("dest overrun while decompressing RIL file.\n");

                for (int i = 0; i < count; i++)
                {
                    dest[destPos++] = 0xFF & incrementValue;
                    if (wordSize >= 2)
                    {
                        dest[destPos++] = 0xFF & (incrementValue >> 8);
                    }
                    incrementValue++;
                }
                printf("(incrementValue = %04X) ", incrementValue);
                break;
        }

        printf("\n");

        if (destPos == destSize)
        {
            *uncompressedSize = destSize;
            return dest;
        }
    }

fail:
    FATAL_ERROR("Fatal error while decompressing RIL file.\n");
}

static unsigned char *RILCompress(unsigned char *src, int srcSize, int *compressedSize, int wordSize)
{
    if (srcSize <= 0)
        goto fail;

    // Round up to the next multiple of four.
    int worstCaseDestSize = (7 + srcSize * (wordSize + 1)) & ~3;

    unsigned char *dest = malloc(worstCaseDestSize);

    if (dest == NULL)
        goto fail;

    // header
    dest[0] = 0x00; // TODO compression type
    dest[1] = (unsigned char)srcSize;
    dest[2] = (unsigned char)(srcSize >> 8);
    dest[3] = (unsigned char)(srcSize >> 16);

    int srcPos = 0;
    int destPos = 4;

    unsigned short incrementValue = 0;
    unsigned short repeatValue = 0;

    for (;;)
    {
        if (srcPos == srcSize)
        {
            int remainder = destPos % 4;

            if (remainder != 0)
            {
                for (int i = 0; i < 4 - remainder; i++)
                    dest[destPos++] = 0;
            }

            *compressedSize = destPos;
            return dest;
        }

        if (srcPos + wordSize > srcSize)
            goto fail;

        unsigned short firstValue = NEXT_SRC_WORD;
        srcPos += wordSize;

        if (srcPos == srcSize)
        {
            if (firstValue == repeatValue)
            {
                dest[destPos++] = 0b10000000;
            }
            else if (firstValue == incrementValue)
            {
                dest[destPos++] = 0b11000000;
                // The increment should not be needed, since this is the end of the file,
                // but include it just to be safe
                incrementValue++;
            }
            else
            {
                dest[destPos++] = 0b01000000;
                dest[destPos++] = 0xFF & firstValue;
                if (wordSize >= 2)
                {
                    dest[destPos++] = 0xFF & (firstValue >> 8);
                }
            }
            continue;
        }

        if (srcPos + wordSize > srcSize)
            goto fail;

        unsigned short secondValue = NEXT_SRC_WORD;

        if (firstValue == secondValue)
        {
            if (firstValue != repeatValue)
            {
                repeatValue = firstValue;
                dest[destPos++] = 0b00000001;
                dest[destPos++] = 0xFF & firstValue;
                if (wordSize >= 2)
                {
                    dest[destPos++] = 0xFF & (firstValue >> 8);
                }
            }

            srcPos += wordSize;
            int lengthMinusOne = 1;
            while (srcPos + wordSize <= srcSize &&
                    lengthMinusOne < MAX_LENGTH_MINUS_ONE &&
                    repeatValue == NEXT_SRC_WORD)
            {
                lengthMinusOne++;
                srcPos += wordSize;
            }

            unsigned char flags = 0b10000000 | (0x3F & lengthMinusOne);
            dest[destPos++] = flags;
        }
        else if (firstValue + 1 == secondValue)
        {
            if (firstValue != incrementValue)
            {
                incrementValue = firstValue;
                dest[destPos++] = 0b00000000;
                dest[destPos++] = 0xFF & firstValue;
                if (wordSize >= 2)
                {
                    dest[destPos++] = 0xFF & (firstValue >> 8);
                }
            }

            incrementValue += 2;
            srcPos += wordSize;
            int lengthMinusOne = 1;
            while (srcPos + wordSize <= srcSize &&
                    lengthMinusOne < MAX_LENGTH_MINUS_ONE &&
                    incrementValue == NEXT_SRC_WORD)
            {
                lengthMinusOne++;
                incrementValue++;
                srcPos += wordSize;
            }

            unsigned char flags = 0b11000000 | (0x3F & lengthMinusOne);
            dest[destPos++] = flags;
        }
        else if (firstValue == repeatValue)
        {
            dest[destPos++] = 0b10000000;
        }
        else if (firstValue == incrementValue)
        {
            dest[destPos++] = 0b11000000;
            incrementValue++;
        }
        else
        {
            int startPos = srcPos - wordSize;
            int lengthMinusOne = 0;

            while (srcPos + wordSize <= srcSize
                    && lengthMinusOne < MAX_LENGTH_MINUS_ONE
                    && repeatValue != NEXT_SRC_WORD
                    && incrementValue != NEXT_SRC_WORD
                    && (srcPos + (wordSize * 2) <= srcSize ? NEXT_SRC_WORD != NEXT_NEXT_SRC_WORD : true)
                    && (srcPos + (wordSize * 2) <= srcSize ? (NEXT_SRC_WORD + 1) != NEXT_NEXT_SRC_WORD : true)
                    )
            {
                lengthMinusOne++;
                srcPos += wordSize;
            }

            unsigned char flags = 0b01000000 | (0x3F & lengthMinusOne);
            dest[destPos++] = flags;
            while (startPos < srcPos)
            {
                dest[destPos++] = src[startPos++];
            }
        }
    }

fail:
    FATAL_ERROR("Fatal error while compressing RIL file.\n");
}
