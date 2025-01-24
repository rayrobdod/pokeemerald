#include "ril.h"

#include <stdlib.h>
#include <stdbool.h>
#include "global.h"
#include "options.h"

/* Run-Increment-Length encoding
 *
 * Two high bits are opcode.
 * Low 6 bits are length-minus-n, where n is 1 for COPY and 3 for others.
 * COPY (0b00) is copy the *length* words that follow the opcode from src to dest
 * RUN (0b10) is write the word that follows the opcode *length* times
 * INC (0b01) is write a sequence of *length* words, starting at the word that follows the opcode and incrementing by 1 each time
 * DEC (0b11) is like INC, but decrementing instead of incrementing
 */
 
enum {
    OPCODE_COPY = 0b00000000,
    OPCODE_INC = 0b01000000,
    OPCODE_RUN = 0b10000000,
    OPCODE_DEC = 0b11000000,
};


#define SRC_WORD(n) (wordSize == WORD_SIZE_16 ? (src[srcPos + 1 + n * 2] << 8) | src[srcPos + n * 2] : src[srcPos + n])
#define NEXT_SRC_WORD (wordSize == WORD_SIZE_16 ? (src[srcPos + 1] << 8) | src[srcPos] : src[srcPos])

unsigned char *RILDecompress(unsigned char *src, unsigned int srcSize, unsigned int *uncompressedSize, enum WordSize wordSize)
{
    if (srcSize < 4)
        FATAL_ERROR("Source did not have at least four bytes while decompressing RIL file.\n");

    int destSize = (src[3] << 16) | (src[2] << 8) | src[1];

    unsigned char *dest = malloc(destSize);

    if (dest == NULL)
        goto fail;


    int srcPos = 4;
    int destPos = 0;

    for (;;)
    {
        if (srcPos >= srcSize) {
            printf("(destPos = %d; destSize = %d)\n", destPos, destSize);
            FATAL_ERROR("Reached end of input while decompressing RIL file.\n");
        }

        unsigned char flags = src[srcPos++];

        if ((flags & 0xC0) == OPCODE_COPY)
        {
            unsigned short count = (flags & 0x3F) + 1;
            printf("COPY %02X: ", count);
            
            if (srcPos + count * wordSize >= srcSize)
                FATAL_ERROR("src overrun while decompressing RIL file.\n");
            if (destPos + count * wordSize > destSize)
                FATAL_ERROR("dest overrun while decompressing RIL file.\n");
            
            for (int i = 0; i < count * wordSize; i++)
            {
                printf("%02X ", src[srcPos]);
                dest[destPos++] = src[srcPos++];
            }
        }
        else
        {
            if (srcPos + wordSize >= srcSize)
                FATAL_ERROR("src overrun while decompressing RIL file.\n");
            
            unsigned short count = (flags & 0x3F) + 3;
            unsigned short value = NEXT_SRC_WORD;
            srcPos += wordSize;
            
            if (destPos + count * wordSize > destSize)
                FATAL_ERROR("dest overrun while decompressing RIL file.\n");
            
            int delta = 0;
            char* name = "";
            switch (flags & 0xC0)
            {
                case OPCODE_RUN:
                    delta = 0;
                    name = "RUN ";
                    break;
                case OPCODE_INC:
                    delta = 1;
                    name = "INC ";
                    break;
                case OPCODE_DEC:
                    delta = -1;
                    name = "DEC ";
                    break;
            }
            
            printf("%s %02X: %04X", name, count, value);
            
            for (int i = 0; i < count; i++)
            {
                dest[destPos++] = 0xFF & value;
                if (wordSize >= 2)
                {
                    dest[destPos++] = 0xFF & (value >> 8);
                }
                
                value += delta;
            }
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

unsigned char *RILCompress(unsigned char *src, unsigned int srcSize, unsigned int *compressedSize, enum WordSize wordSize)
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

    unsigned int srcPos = 0;
    unsigned int destPos = 4;

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

        const unsigned short firstValue = NEXT_SRC_WORD;
        srcPos += wordSize;
        
        if (srcPos == srcSize)
        {
            dest[destPos++] = 0b00000000;
            dest[destPos++] = 0xFF & firstValue;
            if (wordSize >= 2)
            {
                dest[destPos++] = 0xFF & (firstValue >> 8);
            }
            continue;
        }

        if (srcPos + wordSize > srcSize)
            goto fail;

        const unsigned short secondValue = NEXT_SRC_WORD;
        
        if (srcPos == srcSize)
        {
            dest[destPos++] = 0b00000001;
            dest[destPos++] = 0xFF & firstValue;
            if (wordSize >= 2)
            {
                dest[destPos++] = 0xFF & (firstValue >> 8);
            }
            dest[destPos++] = 0xFF & secondValue;
            if (wordSize >= 2)
            {
                dest[destPos++] = 0xFF & (secondValue >> 8);
            }
            continue;
        }

        if (srcPos + wordSize > srcSize)
            goto fail;

        const unsigned short thirdValue = SRC_WORD(1);
        
        if (firstValue - secondValue == 0 &&
            secondValue - thirdValue == 0)
        {
            srcPos += wordSize * 2;
            unsigned int lengthMinusThree = 0;
            while (srcPos < srcSize && firstValue == NEXT_SRC_WORD && lengthMinusThree < 0x3F) {
                lengthMinusThree += 1;
                srcPos += wordSize;
            }

            dest[destPos++] = OPCODE_RUN | lengthMinusThree;
            dest[destPos++] = 0xFF & firstValue;
            if (wordSize >= 2)
            {
                dest[destPos++] = 0xFF & (firstValue >> 8);
            }
        }
        else if (secondValue - firstValue == 1 &&
            thirdValue - secondValue == 1)
        {
            srcPos += wordSize * 2;
            unsigned short lengthMinusThree = 0;
            while (srcPos < srcSize && (firstValue + lengthMinusThree + 3) == NEXT_SRC_WORD && lengthMinusThree < 0x3F) {
                lengthMinusThree += 1;
                srcPos += wordSize;
            }

            dest[destPos++] = OPCODE_INC | lengthMinusThree;
            dest[destPos++] = 0xFF & firstValue;
            if (wordSize >= 2)
            {
                dest[destPos++] = 0xFF & (firstValue >> 8);
            }
        }
        else if (firstValue - secondValue == 1 &&
            secondValue - thirdValue == 1)
        {
            srcPos += wordSize * 2;
            unsigned short lengthMinusThree = 0;
            while (srcPos < srcSize && (firstValue - lengthMinusThree - 3) == NEXT_SRC_WORD && lengthMinusThree < 0x3F)
            {
                lengthMinusThree += 1;
                srcPos += wordSize;
            }

            dest[destPos++] = OPCODE_DEC | lengthMinusThree;
            dest[destPos++] = 0xFF & firstValue;
            if (wordSize >= 2)
            {
                dest[destPos++] = 0xFF & (firstValue >> 8);
            }
        }
        else
        {
            unsigned int opcodePos = destPos++;
            unsigned short lengthMinusOne = 0;
            dest[destPos++] = 0xFF & firstValue;
            if (wordSize >= 2)
            {
                dest[destPos++] = 0xFF & (firstValue >> 8);
            }
            
            while (srcPos < srcSize && lengthMinusOne < 0x3F && (SRC_WORD(0) - SRC_WORD(1) != SRC_WORD(1) - SRC_WORD(2)))
            {
                lengthMinusOne += 1;
                dest[destPos++] = src[srcPos++];
                if (wordSize >= 2)
                {
                    dest[destPos++] = src[srcPos++];
                }
            }
            
            dest[opcodePos] = OPCODE_COPY | lengthMinusOne;
        }
    }

fail:
    FATAL_ERROR("Fatal error while compressing RIL file.\n");
}
