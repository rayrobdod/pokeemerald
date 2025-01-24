#ifndef RIL_H
#define RIL_H

unsigned char *RIL8Decompress(unsigned char *src, int srcSize, int *uncompressedSize);
unsigned char *RIL8Compress(unsigned char *src, int srcSize, int *compressedSize);

unsigned char *RIL16Decompress(unsigned char *src, int srcSize, int *uncompressedSize);
unsigned char *RIL16Compress(unsigned char *src, int srcSize, int *compressedSize);

#endif // RIL_H
