#include "mode.h"

#include "global.h"
#include "util.h"
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <regex>

using std::string;

string groupLabel(DexGroup dex_group)
{
    switch (dex_group)
    {
    case DexGroup::HOENN:
        return "gEasyChatGroup_Pokemon";
    case DexGroup::KANTOJOHTO:
        return "gEasyChatGroup_Pokemon2";
    }
    FATAL_ERROR("illegal argument\n");
}

std::regex enumEntryRegex(DexGroup dex_group)
{
    switch (dex_group)
    {
    case DexGroup::HOENN:
        return std::regex("HOENN_DEX_(\\w+),");
    case DexGroup::KANTOJOHTO:
        return std::regex("NATIONAL_DEX_(\\w+),");
    }
    FATAL_ERROR("illegal argument\n");
}

std::regex countRegex(DexGroup dex_group)
{
    switch (dex_group)
    {
    case DexGroup::HOENN:
        return std::regex("#define HOENN_DEX_COUNT +HOENN_DEX_(\\w+)");
    case DexGroup::KANTOJOHTO:
        return std::regex("#define JOHTO_DEX_COUNT +NATIONAL_DEX_(\\w+)");
    }
    FATAL_ERROR("illegal argument\n");
}

std::string countHint(DexGroup dex_group)
{
    switch (dex_group)
    {
    case DexGroup::HOENN:
        return "HOENN_DEX_COUNT";
    case DexGroup::KANTOJOHTO:
        return "JOHTO_DEX_COUNT";
    }
    FATAL_ERROR("illegal argument\n");
}

void mode_pokemon(
        DexGroup dex_group,
        const std::filesystem::path& species_names_file,
        const std::filesystem::path& pokedex_file,
        const std::filesystem::path& output_file)
{
    string pokedex_str = read_text_file(pokedex_file);
    std::vector<string> full_dex;
    {
        std::regex entry_regex(enumEntryRegex(dex_group));
        auto entries_begin = std::sregex_iterator(pokedex_str.cbegin(), pokedex_str.cend(), entry_regex);
        auto entries_end = std::sregex_iterator();

        for (std::sregex_iterator i = entries_begin; i != entries_end; ++i)
        {
            if (i->str(1) == "NONE")
                continue;

            full_dex.push_back(i->str(1));
        }
    }

    string dex_count;
    {
        std::regex dex_count_regex(countRegex(dex_group));
        std::smatch m;
        std::regex_search(pokedex_str.cbegin(), pokedex_str.cend(), m, dex_count_regex);
        if (m.empty())
            FATAL_ERROR("malformed input: did not find %s\n", countHint(dex_group).c_str());
        dex_count = m.str(1);
    }

    std::vector<string>::iterator split = std::find(full_dex.begin(), full_dex.end(), dex_count) + 1;
    std::vector<string> group(full_dex.begin(), split);


    string species_names_str = read_text_file(species_names_file);
    std::map<string, string> species_names;
    {
        std::regex entry_regex("\\[SPECIES_(\\w+)\\]\\s*=\\s*_\\(\"([^\"]+)\"\\),");
        auto entries_begin = std::sregex_iterator(species_names_str.cbegin(), species_names_str.cend(), entry_regex);
        auto entries_end = std::sregex_iterator();

        for (std::sregex_iterator i = entries_begin; i != entries_end; ++i)
        {
            if (group.end() != std::find(group.begin(), group.end(), i->str(1)))
            {
                species_names.emplace(i->str(2), i->str(1));
            }
        }
    }

    std::ofstream output_stream(output_file);

    output_stream
        << "//"
        << std::endl
        << "// DO NOT MODIFY THIS FILE! It is auto-generated"
        << std::endl
        << "//"
        << std::endl << std::endl
        << "const u16 "
        << groupLabel(dex_group)
        << "[] = {"
        << std::endl;

    for (std::pair<string, string> valueKey : species_names)
    {
        output_stream
            << "    SPECIES_"
            << valueKey.second
            << ","
            << std::endl;
    }

    output_stream
        << "};"
        << std::endl;
}
