#ifndef COMPRESS_H
#define COMPRESS_H

#include "global.h"
#include <stdbool.h>

struct ByteArray decompress8(struct ByteArray, bool disassemble);
struct ByteArray compress8(struct ByteArray);

struct ShortArray decompress16(struct ByteArray, bool disassemble);
struct ByteArray compress16(struct ShortArray);

#endif // COMPRESS_H
