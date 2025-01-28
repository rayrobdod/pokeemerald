// Copyright (c) 2015 YamaArashi

#ifndef GLOBAL_H
#define GLOBAL_H

#include <stdio.h>
#include <stdlib.h>


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


#ifdef _MSC_VER

#define FATAL_ERROR(format, ...)          \
do {                                      \
    fprintf(stderr, format, __VA_ARGS__); \
    exit(1);                              \
} while (0)

#define UNUSED

#else

#define FATAL_ERROR(format, ...)            \
do {                                        \
    fprintf(stderr, format, ##__VA_ARGS__); \
    exit(1);                                \
} while (0)

#define UNUSED __attribute__((__unused__))

#endif // _MSC_VER

#endif // GLOBAL_H
