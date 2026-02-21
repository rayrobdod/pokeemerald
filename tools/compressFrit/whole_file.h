#ifndef WHOLE_FILE_H
#define WHOLE_FILE_H

#include "byte_array.h"

struct ShortArray ReadWholeFileShort(const char *path);
struct ByteArray ReadWholeFileByte(const char *path);

void WriteWholeFileShort(const char *path, struct ShortArray data);
void WriteWholeFileByte(const char *path, struct ByteArray data);

#endif // WHOLE_FILE_H
