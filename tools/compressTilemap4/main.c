#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "global.h"
#include "util.h"
#include "compress.h"

enum Direction
{
    COMPRESS,
    DECOMPRESS,
};

struct Args
{
    char *programName;
    char *inputPath;
    char *outputPath;
    enum Direction direction;
};

int main(int argc, char **argv)
{
    struct Args args = {
        .direction = COMPRESS,
    };

    for (int i = 0; i < argc; i++)
    {
        if (0 == strcmp("-d", argv[i]))
            args.direction = DECOMPRESS;
        else if ('-' == argv[i][0])
            FATAL_ERROR("Unknown argument: %s", argv[i]);
        else if (NULL == args.programName)
            args.programName = argv[i];
        else if (NULL == args.inputPath)
            args.inputPath = argv[i];
        else if (NULL == args.outputPath)
            args.outputPath = argv[i];
        else
            FATAL_ERROR("Too many positional arguments");
    }

    if (NULL == args.outputPath)
        FATAL_ERROR("Usage: compressTilemap3 INPUT_PATH OUTPUT_PATH [options...]\n");

    if (args.direction == COMPRESS)
    {
        struct ShortArray input = ReadWholeFileShort(args.inputPath);
        struct ByteArray output = compress(input);
        struct ShortArray round_trip = decompress(output, false);
        for (int i = 0; i < input.size; i++)
            if (input.buffer[i] != round_trip.buffer[i])
                FATAL_ERROR("Mismatch: index %d, input %04x, roundtrip %04x\n", i, input.buffer[i], round_trip.buffer[i]);

        free(round_trip.buffer);
        free(input.buffer);
        WriteWholeFileByte(args.outputPath, output);
        free(output.buffer);
    }
    else
    {
        struct ByteArray input = ReadWholeFileByte(args.inputPath);
        struct ShortArray output = decompress(input, true);
        free(input.buffer);
        WriteWholeFileShort(args.outputPath, output);
        free(output.buffer);
    }

    return 0;
}
