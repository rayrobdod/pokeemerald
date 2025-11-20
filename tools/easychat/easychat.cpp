
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
    else if (mode == "pokemon")
    {
        if (argc != 6)
            FATAL_ERROR("USAGE: easychat pokemon regional|national <species_names_file> <pokedex_constants_file> <output_file>\n");

        string group_str(argv[2]);
        enum DexGroup group;
        if (group_str == "hoenn")
            group = DexGroup::HOENN;
        else if (group_str == "kantojohto")
            group = DexGroup::KANTOJOHTO;
        else
            FATAL_ERROR("USAGE: easychat pokemon regional|national <species_names_file> <pokedex_constants_file> <output_file>\n");

        std::filesystem::path species_names_file(argv[3]);
        std::filesystem::path pokedex_constants_file(argv[4]);
        std::filesystem::path output_file(argv[5]);

        mode_pokemon(group, species_names_file, pokedex_constants_file, output_file);
    }
    else
    {
        FATAL_ERROR("ERROR: <mode> must be 'wordinfo'.\n");
    }

    return 0;
}
