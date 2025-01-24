#ifndef RIL_H
#define RIL_H

enum WordSize
{
    WORD_SIZE_8 = 1,
    WORD_SIZE_16 = 2,
};

unsigned char *RILDecompress(unsigned char *src, int srcSize, int *uncompressedSize, enum WordSize wordSize);
unsigned char *RILCompress(unsigned char *src, int srcSize, int *compressedSize, enum WordSize wordSize);

#endif // RIL_H
