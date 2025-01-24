// Copyright (c) 2015 YamaArashi

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "global.h"
#include "util.h"
#include "ril.h"

struct CommandHandler
{
    const char *inputFileExtension;
    const char *outputFileExtension;
    void(*function)(char *inputPath, char *outputPath, int argc, char **argv);
};

void HandleRIL16CompressCommand(char *inputPath, char *outputPath, int argc UNUSED, char **argv UNUSED)
{
    int fileSize;
    unsigned char *buffer = ReadWholeFile(inputPath, &fileSize);

    int compressedSize;
    unsigned char *compressedData = RIL16Compress(buffer, fileSize, &compressedSize);

    free(buffer);

    WriteWholeFile(outputPath, compressedData, compressedSize);

    free(compressedData);
}

void HandleRIL16DecompressCommand(char *inputPath, char *outputPath, int argc UNUSED, char **argv UNUSED)
{
    int fileSize;
    unsigned char *buffer = ReadWholeFile(inputPath, &fileSize);

    int uncompressedSize;
    unsigned char *uncompressedData = RIL16Decompress(buffer, fileSize, &uncompressedSize);

    free(buffer);

    WriteWholeFile(outputPath, uncompressedData, uncompressedSize);

    free(uncompressedData);
}

void HandleRIL8CompressCommand(char *inputPath, char *outputPath, int argc UNUSED, char **argv UNUSED)
{
    int fileSize;
    unsigned char *buffer = ReadWholeFile(inputPath, &fileSize);

    int compressedSize;
    unsigned char *compressedData = RIL8Compress(buffer, fileSize, &compressedSize);

    free(buffer);

    WriteWholeFile(outputPath, compressedData, compressedSize);

    free(compressedData);
}

void HandleRIL8DecompressCommand(char *inputPath, char *outputPath, int argc UNUSED, char **argv UNUSED)
{
    int fileSize;
    unsigned char *buffer = ReadWholeFile(inputPath, &fileSize);

    int uncompressedSize;
    unsigned char *uncompressedData = RIL8Decompress(buffer, fileSize, &uncompressedSize);

    free(buffer);

    WriteWholeFile(outputPath, uncompressedData, uncompressedSize);

    free(uncompressedData);
}

int main(int argc, char **argv)
{
    char converted = 0;

    if (argc < 3)
        FATAL_ERROR("Usage: gbagfx INPUT_PATH OUTPUT_PATH [options...]\n");

    struct CommandHandler handlers[] =
    {
        { NULL, "ril16", HandleRIL16CompressCommand },
        { "ril16", NULL, HandleRIL16DecompressCommand },
        { NULL, "ril8", HandleRIL8CompressCommand },
        { "ril8", NULL, HandleRIL8DecompressCommand },
        { NULL, NULL, NULL }
    };

    char *inputPath = argv[1];
    char *outputPath = argv[2];
    char *inputFileExtension = GetFileExtensionAfterDot(inputPath);
    char *outputFileExtension = GetFileExtensionAfterDot(outputPath);

    if (inputFileExtension == NULL)
        FATAL_ERROR("Input file \"%s\" has no extension.\n", inputPath);

    if (outputFileExtension == NULL)
    {
        outputFileExtension = GetFileExtension(outputPath);

        if (*outputFileExtension == '.')
            outputFileExtension++;

        if (*outputFileExtension == 0)
            FATAL_ERROR("Output file \"%s\" has no extension.\n", outputPath);

        size_t newOutputPathSize = strlen(inputPath) - strlen(inputFileExtension) + strlen(outputFileExtension);
        outputPath = malloc(newOutputPathSize);

        if (outputPath == NULL)
            FATAL_ERROR("Failed to allocate memory for new output path.\n");

        for (int i = 0; i < newOutputPathSize; i++)
        {
            outputPath[i] = inputPath[i];

            if (outputPath[i] == '.')
            {
                strcpy(&outputPath[i + 1], outputFileExtension);
                break;
            }
        }
    }

    for (int i = 0; handlers[i].function != NULL; i++)
    {
        if ((handlers[i].inputFileExtension == NULL || strcmp(handlers[i].inputFileExtension, inputFileExtension) == 0)
            && (handlers[i].outputFileExtension == NULL || strcmp(handlers[i].outputFileExtension, outputFileExtension) == 0))
        {
            handlers[i].function(inputPath, outputPath, argc, argv);
            converted = 1;
            break;
        }
    }

    if (outputPath != argv[2])
        free(outputPath);

    if (!converted)
        FATAL_ERROR("Don't know how to convert \"%s\" to \"%s\".\n", argv[1], argv[2]);

    return 0;
}
