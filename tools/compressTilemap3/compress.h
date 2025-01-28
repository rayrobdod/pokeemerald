#ifndef COMPRESS_H
#define COMPRESS_H

#include "global.h"
#include <stdbool.h>

struct ShortArray decompress(struct ByteArray, bool disassemble);
struct ByteArray compress(struct ShortArray);

#endif // COMPRESS_H
