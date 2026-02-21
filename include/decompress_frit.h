#ifndef GUARD_DECOMPRESS_FRIT_H
#define GUARD_DECOMPRESS_FRIT_H

void Frit16UnComp(const u32 *src, void *dest);
void Frit8UnCompVram(const u32 *src, void *dest);
void Frit8UnCompWram(const u32 *src, void *dest);

#endif // GUARD_DECOMPRESS_FRIT_H
