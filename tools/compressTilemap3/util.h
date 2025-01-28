#ifndef UTIL_H
#define UTIL_H

struct ShortArray ReadWholeFileShort(char *path);
struct ByteArray ReadWholeFileByte(char *path);

void WriteWholeFileShort(char *path, struct ShortArray data);
void WriteWholeFileByte(char *path, struct ByteArray data);

#endif // UTIL_H
