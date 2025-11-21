
#include "global.h"
#include "wordinfo.h"
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

        WordInfoList data = parseJson(input_file);
        writeWordInfo(output_file, data, input_file);
    }
    else if (mode == "pokemon")
    {
        if (argc != 6)
            FATAL_ERROR("USAGE: easychat pokemon hoenn|kantojohto <species_names_file> <pokedex_constants_file> <output_file>\n");

        string dex_group_str(argv[2]);
        enum DexGroup dex_group;
        if (dex_group_str == "hoenn")
            dex_group = DexGroup::HOENN;
        else if (dex_group_str == "kantojohto")
            dex_group = DexGroup::KANTOJOHTO;
        else
            FATAL_ERROR("USAGE: easychat pokemon hoenn|kantojohto <species_names_file> <pokedex_constants_file> <output_file>\n");

        std::filesystem::path species_names_file(argv[3]);
        std::filesystem::path pokedex_constants_file(argv[4]);
        std::filesystem::path output_file(argv[5]);

        WordInfoList data = parsePokemon(dex_group, species_names_file, pokedex_constants_file);
        writeValueList(output_file, data, {species_names_file, pokedex_constants_file});
    }
    else
    {
        FATAL_ERROR("ERROR: <mode> must be 'wordinfo', 'pokemon'.\n");
    }

    return 0;
}
