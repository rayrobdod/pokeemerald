#ifndef COMPRESS_H
#define COMPRESS_H

#include <stdbool.h>
#include "byte_array.h"

enum DecompressVerbosity
{
    VERBOSE_SILENT = 0,
    VERBOSE_DISASSEMBLE = 1,
};

struct ByteArray decompress8(struct ByteArray, enum DecompressVerbosity);
struct ByteArray compress8(struct ByteArray);

struct ShortArray decompress16(struct ByteArray, enum DecompressVerbosity);
struct ByteArray compress16(struct ShortArray);

#endif // COMPRESS_H
