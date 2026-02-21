#include "compress.h"

#include <stdio.h>
#include "fatal_error.h"

#define LONGEST_RUN (1+14+16+255)
#define NUM_REGS (4)


struct Run
{
    char delta;
    size_t length;
    unsigned short start;
};

struct RunArray
{
    size_t size;
    struct Run* buffer;
};

static size_t find_run_with_start(struct RunArray haystack, size_t start, unsigned short needle) {
    size_t i;
    for (i = start; i < haystack.size; i++)
    {
        if (haystack.buffer[i].start == needle)
            break;
    }
    return i;
}

static unsigned short run_last(struct Run run) {
    return run.start + (run.length - 1) * run.delta;
}
static unsigned short run_end(struct Run run) {
    return run.start + run.length * run.delta;
}

#define DECOMPRESS_FUNCTION_NAME decompress16
#define COMPRESS_FUNCTION_NAME compress16
#define WORD unsigned short
#define WORDARRAY struct ShortArray
#include "compress_template.h"
#undef DECOMPRESS_FUNCTION_NAME
#undef COMPRESS_FUNCTION_NAME
#undef WORD
#undef WORDARRAY

#define DECOMPRESS_FUNCTION_NAME decompress8
#define COMPRESS_FUNCTION_NAME compress8
#define WORD unsigned char
#define WORDARRAY struct ByteArray
#include "compress_template.h"
#undef DECOMPRESS_FUNCTION_NAME
#undef COMPRESS_FUNCTION_NAME
#undef WORD
#undef WORDARRAY
