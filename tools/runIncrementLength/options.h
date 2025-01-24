#ifndef OPTIONS_H
#define OPTIONS_H

enum Direction
{
    COMPRESS,
    DECOMPRESS,
};

enum WordSize
{
    WORD_SIZE_8 = 1,
    WORD_SIZE_16 = 2,
}; 

struct Args
{
    char *programName;
    char *inputPath;
    char *outputPath;
    enum Direction direction;
    enum WordSize wordSize;
};

#endif
