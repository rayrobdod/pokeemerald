#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>
#include "global.h"
#include "util.h"

struct ShortArray ReadWholeFileShort(char *path)
{
    struct ShortArray retval;

    FILE *fp = fopen(path, "rb");

    if (fp == NULL)
        FATAL_ERROR("Failed to open \"%s\" for reading.\n", path);

    fseek(fp, 0, SEEK_END);

    retval.size = ftell(fp) / 2;

    retval.buffer = malloc(retval.size * 2);

    if (retval.buffer == NULL)
        FATAL_ERROR("Failed to allocate memory for reading \"%s\".\n", path);

    rewind(fp);

    if (fread(retval.buffer, retval.size * sizeof(unsigned short), 1, fp) != 1)
        FATAL_ERROR("Failed to read \"%s\".\n", path);

    fclose(fp);

    return retval;
}

void WriteWholeFileShort(char *path, struct ShortArray data)
{
    FILE *fp = fopen(path, "wb");

    if (fp == NULL)
        FATAL_ERROR("Failed to open \"%s\" for writing.\n", path);

    if (fwrite(data.buffer, data.size * sizeof(unsigned short), 1, fp) != 1)
        FATAL_ERROR("Failed to write to \"%s\".\n", path);

    fclose(fp);
}

struct ByteArray ReadWholeFileByte(char *path)
{
    struct ByteArray retval;

    FILE *fp = fopen(path, "rb");

    if (fp == NULL)
        FATAL_ERROR("Failed to open \"%s\" for reading.\n", path);

    fseek(fp, 0, SEEK_END);

    retval.size = ftell(fp);

    retval.buffer = malloc(retval.size);

    if (retval.buffer == NULL)
        FATAL_ERROR("Failed to allocate memory for reading \"%s\".\n", path);

    rewind(fp);

    if (fread(retval.buffer, retval.size * sizeof(unsigned char), 1, fp) != 1)
        FATAL_ERROR("Failed to read \"%s\".\n", path);

    fclose(fp);

    return retval;
}

void WriteWholeFileByte(char *path, struct ByteArray data)
{
    FILE *fp = fopen(path, "wb");

    if (fp == NULL)
        FATAL_ERROR("Failed to open \"%s\" for writing.\n", path);

    if (fwrite(data.buffer, data.size * sizeof(unsigned char), 1, fp) != 1)
        FATAL_ERROR("Failed to write to \"%s\".\n", path);

    fclose(fp);
}
