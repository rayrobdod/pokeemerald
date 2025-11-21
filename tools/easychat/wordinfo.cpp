#include "wordinfo.h"

#include "global.h"
#include "json11.h"
#include "util.h"
#include <algorithm>
#include <fstream>
#include <regex>

using std::string;
using json11::Json;

WordInfo::WordInfo(
        std::string id,
        std::string index,
        std::string text,
        std::string text_label,
        bool enabled) :
    id(id),
    index(index),
    text(text),
    text_label(text_label),
    enabled(enabled)
{}

WordInfoList::WordInfoList(
        std::string label,
        std::vector<WordInfo> words) :
    label(label),
    words(words)
{}

DoNotModifyHeader::DoNotModifyHeader(const std::filesystem::path& from_file) : from_files({from_file})
{}

DoNotModifyHeader::DoNotModifyHeader(const std::initializer_list<std::filesystem::path>& from_files) : from_files(from_files)
{}


WordInfoList parseJson(const std::filesystem::path& input_file)
{
    string input_err;

    string input_str = read_text_file(input_file);
    Json input_data = Json::parse(input_str, input_err);
    if (input_data.is_null())
        FATAL_ERROR("%s\n", input_err.c_str());

    if (! input_data["label"].is_string())
        FATAL_ERROR("malformed input: `.label` is not a string\n");

    if (! input_data["words"].is_array())
        FATAL_ERROR("malformed input: `.words` is not an array\n");

    const std::vector<Json> words_in = input_data["words"].array_items();

    std::vector<WordInfo> words_out;
    for (Json word : words_in)
    {
        const std::map<std::string, Json>& word_as_map = word.object_items();
        auto enabled_ptr = word_as_map.find("enabled");
        bool enabled = true;
        if (enabled_ptr != word_as_map.end())
            enabled = enabled_ptr->second.bool_value();

        string index;
        index += "EC_INDEX(";
        index += word["id"].string_value();
        index += ")";

        words_out.emplace_back(
               word["id"].string_value(),
               index,
               word["text"].string_value(),
               word["text_label"].string_value(),
               enabled);
    }

    WordInfoList retval(
        input_data["label"].string_value(),
        words_out);
    return retval;
}


static string pokemonGroupLabel(DexGroup dex_group)
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

static std::regex pokemonEnumEntryRegex(DexGroup dex_group)
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

static std::regex pokemonCountRegex(DexGroup dex_group)
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

static std::string pokemonCountHint(DexGroup dex_group)
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

static std::string pokemonGroupMacro(DexGroup dex_group)
{
    switch (dex_group)
    {
    case DexGroup::HOENN:
        return "EC_POKEMON";
    case DexGroup::KANTOJOHTO:
        return "EC_POKEMON_NATIONAL";
    }
    FATAL_ERROR("illegal argument\n");
}

WordInfoList parsePokemon(
        DexGroup dex_group,
        const std::filesystem::path& species_names_file,
        const std::filesystem::path& pokedex_file)
{
    string pokedex_str = read_text_file(pokedex_file);
    std::vector<string> full_dex;
    {
        std::regex entry_regex(pokemonEnumEntryRegex(dex_group));
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
        std::regex dex_count_regex(pokemonCountRegex(dex_group));
        std::smatch m;
        std::regex_search(pokedex_str.cbegin(), pokedex_str.cend(), m, dex_count_regex);
        if (m.empty())
            FATAL_ERROR("malformed input: did not find %s\n", pokemonCountHint(dex_group).c_str());
        dex_count = m.str(1);
    }

    std::vector<string>::iterator split = std::find(full_dex.begin(), full_dex.end(), dex_count) + 1;
    std::vector<string> dex(full_dex.begin(), split);

    string species_names_str = read_text_file(species_names_file);
    std::vector<WordInfo> words_out;
    {
        std::regex entry_regex("\\[SPECIES_(\\w+)\\]\\s*=\\s*_\\(\"([^\"]+)\"\\),");
        auto entries_begin = std::sregex_iterator(species_names_str.cbegin(), species_names_str.cend(), entry_regex);
        auto entries_end = std::sregex_iterator();

        for (std::sregex_iterator i = entries_begin; i != entries_end; ++i)
        {
            if (dex.end() != std::find(dex.begin(), dex.end(), i->str(1)))
            {
                string index("SPECIES_");
                index += i->str(1);

                string id;
                id += pokemonGroupMacro(dex_group);
                id += "(";
                id += index;
                id += ")";

                words_out.emplace_back(
                    id,
                    index,
                    i->str(2),
                    "",
                    true);
            }
        }
    }

    WordInfoList retval(
        pokemonGroupLabel(dex_group),
        words_out);
    return retval;
}

void writeWordInfo(
        const std::filesystem::path& output_file,
        const WordInfoList& data,
        const DoNotModifyHeader& header)
{
    std::vector<string> texts;
    for (WordInfo word : data.words)
    {
        string text = word.text;
        search_replace_all(text, "…", "...");
        search_replace_all(text, "{", "");
        search_replace_all(text, "}", "");
        texts.push_back(text);
    }
    std::vector<string> sorted_texts(texts);
    std::sort(sorted_texts.begin(), sorted_texts.end());


    std::ofstream output_stream(output_file);

    output_stream << header;

    for (WordInfo word : data.words)
    {
        output_stream
            << "const u8 "
            << word.text_label
            << "[] = _(\""
            << word.text
            << "\");"
            << std::endl;
    }

    output_stream
        << std::endl
        << "const struct EasyChatWordInfo "
        << data.label
        << "[] = {"
        << std::endl;

    for (size_t i = 0; i < data.words.size(); i++)
    {
        WordInfo word = data.words[i];
        size_t order = std::find(texts.begin(), texts.end(), sorted_texts[i]) - texts.begin();
        WordInfo order_word = data.words[order];

        output_stream
            << "    ["
            << word.index
            << "] ="
            << std::endl
            << "    {"
            << std::endl
            << "        .text = "
            << word.text_label
            << ","
            << std::endl
            << "        .alphabeticalOrder = "
            << order_word.index
            << ","
            << std::endl
            << "        .enabled = "
            << (word.enabled ? "TRUE" : "FALSE")
            << ","
            << std::endl
            << "    },"
            << std::endl;
    }

    output_stream
        << "};"
        << std::endl;
}

void writeValueList(const std::filesystem::path& output_file, const WordInfoList& data, const DoNotModifyHeader& header)
{
    std::map<string, string> text_indexes;
    for (WordInfo word : data.words)
    {
        string text = word.text;
        search_replace_all(text, "…", "...");
        search_replace_all(text, "{", "");
        search_replace_all(text, "}", "");

        text_indexes.emplace(text, word.index);
    }

    std::ofstream output_stream(output_file);

    output_stream << header;

    output_stream
        << "const u16 "
        << data.label
        << "[] = {"
        << std::endl;

    for (std::pair<string, string> valueKey : text_indexes)
    {
        output_stream
            << "    "
            << valueKey.second
            << ","
            << std::endl;
    }

    output_stream
        << "};"
        << std::endl;
}
