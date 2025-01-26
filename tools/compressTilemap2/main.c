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
        FATAL_ERROR("Usage: gbagfx INPUT_PATH OUTPUT_PATH [options...]\n");

    struct ShortArray input = ReadWholeFile(args.inputPath);

    struct ShortArray output;

    if (args.direction == COMPRESS)
    {
        output = compress(input);
    }
    else
    {
        output = decompress(input);
    }

    free(input.buffer);

    WriteWholeFile(args.outputPath, output);

    free(output.buffer);

    return 0;
}
