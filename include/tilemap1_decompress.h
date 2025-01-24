#ifndef GUARD_TILEMAP1_DECOMPRESS_H
#define GUARD_TILEMAP1_DECOMPRESS_H

void Tilemap1_8DecompressWram(const u32 *src, void *dest);
void Tilemap1_8DecompressVram(const u32 *src, void *dest);

void Tilemap1_16DecompressVram(const u32 *src, void *dest);

#endif // GUARD_TILEMAP1_DECOMPRESS_H
