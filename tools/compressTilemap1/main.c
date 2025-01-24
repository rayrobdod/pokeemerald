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
    enum WordSize wordSize;
};


int main(int argc, char **argv)
{
    struct Args args = {
        .direction = COMPRESS,
        .wordSize = WORD_SIZE_16,
    };

    for (int i = 0; i < argc; i++)
    {
        if (0 == strcmp("-d", argv[i]))
            args.direction = DECOMPRESS;
        else if (0 == strcmp("-8", argv[i]))
            args.wordSize = WORD_SIZE_8;
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

    int fileSize;
    unsigned char *buffer = ReadWholeFile(args.inputPath, &fileSize);

    if (args.direction == COMPRESS)
    {
        int compressedSize;
        unsigned char *compressedData = RILCompress(buffer, fileSize, &compressedSize, args.wordSize);

        free(buffer);

        WriteWholeFile(args.outputPath, compressedData, compressedSize);

        free(compressedData);
    }
    else
    {
        int decompressedSize;
        unsigned char *decompressedData = RILDecompress(buffer, fileSize, &decompressedSize, args.wordSize);

        free(buffer);

        WriteWholeFile(args.outputPath, decompressedData, decompressedSize);

        free(decompressedData);
    }

    return 0;
}
