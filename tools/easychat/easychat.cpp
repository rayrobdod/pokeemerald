
#include "global.h"
#include "mode.h"
#include <filesystem>

using std::string;

int main(int argc, char *argv[])
{
    if (argc < 2)
        FATAL_ERROR("USAGE: easychat <mode> [options]\n");

    string mode(argv[1]);
    if (mode == "wordinfo")
    {
        if (argc != 4)
            FATAL_ERROR("USAGE: easychat wordinfo <input_file> <output_file>\n");

        std::filesystem::path input_file(argv[2]);
        std::filesystem::path output_file(argv[3]);

        mode_wordinfo(input_file, output_file);
    }
    else
    {
        FATAL_ERROR("ERROR: <mode> must be 'wordinfo'.\n");
    }

    return 0;
}
