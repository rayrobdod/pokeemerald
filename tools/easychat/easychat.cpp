
#include "global.h"
#include "wordinfo.h"
#include "util.h"
#include <filesystem>
#include <regex>
#include <vector>

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
    else if (mode == "by_letter")
    {
        if (argc != 7)
            FATAL_ERROR("USAGE: easychat by_letter <...>\n");

        const std::string moves_filename_prefix("easy_chat_group_move_");

        std::filesystem::path input_directory(argv[2]);
        std::filesystem::path species_names_file(argv[3]);
        std::filesystem::path pokedex_constants_file(argv[4]);
        std::filesystem::path move_names_file(argv[5]);
        std::filesystem::path output_file(argv[6]);

        std::vector<WordInfoList> wordss;
        wordss.push_back(parsePokemon(DexGroup::HOENN, species_names_file, pokedex_constants_file));
        wordss.push_back(parsePokemon(DexGroup::KANTOJOHTO, species_names_file, pokedex_constants_file));

        for (auto const& dir_entry : std::filesystem::recursive_directory_iterator{input_directory}) {
            if (dir_entry.is_regular_file()) {
                if (dir_entry.path().extension() == ".json") {
                    wordss.push_back(parseJson(dir_entry));
                }
                if (dir_entry.path().extension() == ".h" && moves_filename_prefix == dir_entry.path().stem().string().substr(0, moves_filename_prefix.size())) {
                    wordss.push_back(parseMoves(move_names_file, dir_entry));
                }
            }
        }

        std::vector<string> unused_letter_suffixes = parseUnusedLettersFile(
                input_directory / "easy_chat_words_by_letter_unused.h");

        writeByLetter(output_file, wordss, unused_letter_suffixes, {});
    }
    else
    {
        FATAL_ERROR("ERROR: <mode> must be 'wordinfo', 'pokemon', 'by_letter'.\n");
    }

    return 0;
}
