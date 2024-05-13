#ifndef GUARD_RILDECOMPRESS_H
#define GUARD_RILDECOMPRESS_H

void Ril8DecompressWram(const u32 *src, void *dest);
void Ril8DecompressVram(const u32 *src, void *dest);

void Ril16DecompressVram(const u32 *src, void *dest);

#endif // GUARD_RILDECOMPRESS_H
