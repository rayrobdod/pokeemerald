#ifndef MODE_H
#define MODE_H

#include <filesystem>

enum class DexGroup {
    HOENN,
    KANTOJOHTO,
};

void mode_wordinfo(
        std::filesystem::path input_file,
        std::filesystem::path output_file);

void mode_pokemon(
        DexGroup regional,
        const std::filesystem::path& species_names_file,
        const std::filesystem::path& pokedex_file,
        const std::filesystem::path& output_file);

#endif // MODE_H
