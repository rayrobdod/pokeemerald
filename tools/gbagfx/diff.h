#ifndef DIFF_H
#define DIFF_H

unsigned char *DiffUnfilter(unsigned char *src, int srcSize, int *uncompressedSize, int wordCountBytes);
unsigned char *DiffFilter(unsigned char *src, int srcSize, int *compressedSize, int wordCountBytes);

#endif // DIFF_H
