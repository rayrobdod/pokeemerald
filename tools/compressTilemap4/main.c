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

enum WordSize
{
    WORD_SIZE_8,
    WORD_SIZE_16,
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
        FATAL_ERROR("Usage: compressTilemap4 INPUT_PATH OUTPUT_PATH [options...]\n");

    switch (args.wordSize)
    {
    case WORD_SIZE_16:
        switch (args.direction)
        {
        case COMPRESS:
            {
                struct ShortArray input = ReadWholeFileShort(args.inputPath);
                struct ByteArray output = compress16(input);
                struct ShortArray round_trip = decompress16(output, false);
                for (int i = 0; i < input.size; i++)
                    if (input.buffer[i] != round_trip.buffer[i])
                        FATAL_ERROR("Mismatch: index %d, input %04x, roundtrip %04x\n", i, input.buffer[i], round_trip.buffer[i]);

                free(round_trip.buffer);
                free(input.buffer);
                WriteWholeFileByte(args.outputPath, output);
                free(output.buffer);
            }
            break;
        case DECOMPRESS:
            {
                struct ByteArray input = ReadWholeFileByte(args.inputPath);
                struct ShortArray output = decompress16(input, true);
                free(input.buffer);
                WriteWholeFileShort(args.outputPath, output);
                free(output.buffer);
            }
            break;
        }
        break;
    case WORD_SIZE_8:
        switch (args.direction)
        {
        case COMPRESS:
            {
                struct ByteArray input = ReadWholeFileByte(args.inputPath);
                struct ByteArray output = compress8(input);
                struct ByteArray round_trip = decompress8(output, false);
                for (int i = 0; i < input.size; i++)
                    if (input.buffer[i] != round_trip.buffer[i])
                        FATAL_ERROR("Mismatch: index %d, input %04x, roundtrip %04x\n", i, input.buffer[i], round_trip.buffer[i]);

                free(round_trip.buffer);
                free(input.buffer);
                WriteWholeFileByte(args.outputPath, output);
                free(output.buffer);
            }
            break;
        case DECOMPRESS:
            {
                struct ByteArray input = ReadWholeFileByte(args.inputPath);
                struct ByteArray output = decompress8(input, true);
                free(input.buffer);
                WriteWholeFileByte(args.outputPath, output);
                free(output.buffer);
            }
            break;
        }
        break;
    }

    return 0;
}
