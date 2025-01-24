#ifndef RIL_H
#define RIL_H

enum WordSize;

unsigned char *RILDecompress(unsigned char *src, unsigned int srcSize, unsigned int *uncompressedSize, enum WordSize wordSize);
unsigned char *RILCompress(unsigned char *src, unsigned int srcSize, unsigned int *compressedSize, enum WordSize wordSize);

#endif // RIL_H
