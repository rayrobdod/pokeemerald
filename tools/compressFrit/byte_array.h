#ifndef BYTE_ARRAY_H
#define BYTE_ARRAY_H

#include <stddef.h>

struct ByteArray
{
    size_t size;
    unsigned char* buffer;
};

struct ShortArray
{
    size_t size;
    unsigned short* buffer;
};

#endif        //  #ifndef BYTE_ARRAY_H
