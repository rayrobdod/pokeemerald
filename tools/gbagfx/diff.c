#include "diff.h"

#include <stdlib.h>
#include <stdbool.h>
#include "global.h"

unsigned char *DiffUnfilter(unsigned char *src, int srcSize, int *uncompressedSize, int wordCountBytes)
{
    if (wordCountBytes != 1 && wordCountBytes != 2)
        FATAL_ERROR("Only supports wordCountBytes of 1 or 2. Was %d.\n", wordCountBytes);
    if (srcSize < 4)
        goto fail;

    int destSize = (src[3] << 16) | (src[2] << 8) | src[1];
    *uncompressedSize = destSize;

    if (srcSize != destSize + 4)
        FATAL_ERROR("Unexpected uncompressed size. Expected %d. Was %d.\n", srcSize - 4, destSize);

    unsigned char *dest = malloc(destSize);

    if (dest == NULL)
        goto fail;

    unsigned int previous = 0;

    int i = 0;
    for (; i < wordCountBytes; i++)
    {
        previous = src[4 + i] << (i * 8);
        dest[i] = src[4 + i];
    }

    for (; i < destSize; i += wordCountBytes)
    {
        unsigned int delta = 0;
        for (int j = 0; j < wordCountBytes; j++) {
            delta |= src[4 + i + j] << (j * 8);
        }
        unsigned int next = previous + delta;
        for (int j = 0; j < wordCountBytes; j++) {
            dest[i + j] = 0xFF & (next >> (j * 8));
        }
        previous = next;
    }

    return dest;

fail:
    FATAL_ERROR("Fatal error while decompressing diff file.\n");
}

unsigned char *DiffFilter(unsigned char *src, int srcSize, int *compressedSize, int wordCountBytes)
{
    if (wordCountBytes != 1 && wordCountBytes != 2)
        FATAL_ERROR("Only supports wordCountBytes of 1 or 2. Was %d.\n", wordCountBytes);
    if (srcSize <= 0)
        FATAL_ERROR("srcSize was negative\n");
    if (srcSize % wordCountBytes != 0)
        FATAL_ERROR("srcSize not a multiple of wordCountBytes\n");

    *compressedSize = 4 + srcSize;

    unsigned char *dest = malloc(*compressedSize);

    if (dest == NULL)
        goto fail;

    // header
    dest[0] = 0x80 | wordCountBytes;
    dest[1] = (unsigned char)srcSize;
    dest[2] = (unsigned char)(srcSize >> 8);
    dest[3] = (unsigned char)(srcSize >> 16);

    unsigned int previous = 0;

    int i = 0;
    for (; i < wordCountBytes; i++)
    {
        previous |= src[i] << (i * 8);
        dest[4 + i] = src[i];
    }

    for (; i < srcSize; i += wordCountBytes)
    {
        unsigned int next = 0;
        for (int j = 0; j < wordCountBytes; j++) {
            next |= src[i + j] << (j * 8);
        }
        unsigned int delta = next - previous;
        for (int j = 0; j < wordCountBytes; j++) {
            dest[4 + i + j] = 0xFF & (delta >> (j * 8));
        }
        previous = next;
    }

    return dest;

fail:
    FATAL_ERROR("Fatal error while compressing diff file.\n");
}
