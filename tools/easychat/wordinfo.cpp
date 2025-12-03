#include "wordinfo.h"

#include "global.h"
#include "json11.h"
#include "util.h"
#include <algorithm>
#include <fstream>
#include <regex>
#include <set>
#include <iostream>

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

string WordInfo::clean_text() const
{
    string retval = this->text;
    search_replace_all(retval, "…", "...");
    search_replace_all(retval, "{", "");
    search_replace_all(retval, "}", "");
    return retval;
}

WordInfoList::WordInfoList(
        std::string id,
        std::string label,
        WordInfoListType type,
        std::vector<WordInfo> words) :
    id(id),
    label(label),
    type(type),
    words(words)
{}

DoNotModifyHeader::DoNotModifyHeader(const std::filesystem::path& from_file) : from_files({from_file})
{}

DoNotModifyHeader::DoNotModifyHeader(const std::initializer_list<std::filesystem::path>& from_files) : from_files(from_files)
{}

struct WordInfoCmpByText
{
    bool operator()(const WordInfo& lhs, const WordInfo& rhs) const
    {
        if (lhs.clean_text() != rhs.clean_text())
        {
            return lhs.clean_text() < rhs.clean_text();
        }
        else if (lhs.id != rhs.id)
        {
            if (lhs.id == "EC_WORD_PSYCHIC")
                return true;
            if (rhs.id == "EC_WORD_PSYCHIC")
                return false;
            return lhs.id < rhs.id;
        }
        else
        {
            return false;
        }
    }
};

static string text_to_label_suffix(const string& text)
{
    string buffer(text);
    search_replace_all(buffer, "!", "_EXCL_");
    search_replace_all(buffer, "?", "_QUES_");
    search_replace_all(buffer, "…", "_ELLIPSIS_");
    search_replace_all(buffer, "-", "_DASH_");
    search_replace_all(buffer, " ", "_");
    search_replace_all(buffer, "'", "_");
    search_replace_all(buffer, "___", "_");
    search_replace_all(buffer, "__", "_");
    if ('_' == buffer[0])
        buffer = buffer.substr(1);
    if ('_' == buffer[buffer.size() - 1])
        buffer = buffer.substr(0, buffer.size() - 1);

    string retval;
    retval.reserve(buffer.size());
    for (char c : buffer)
    {
        if (('A' <= c && c <= 'Z') || '_' == c)
            retval.push_back(c);
    }

    return retval;
}

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

        string label = word["text_label"].string_value();
        if (label.empty())
        {
            label = "gEasyChatWord_";
            label += text_to_label_suffix(word["text"].string_value());
        }

        string index;
        index += "EC_INDEX(";
        index += word["id"].string_value();
        index += ")";

        words_out.emplace_back(
               word["id"].string_value(),
               index,
               word["text"].string_value(),
               label,
               enabled);
    }

    WordInfoList retval(
        input_data["id"].string_value(),
        input_data["label"].string_value(),
        WordInfoListType::WORDS,
        words_out);
    return retval;
}


struct PokemonGroupData
{
    std::string id;
    std::string label;
    std::regex pokemonEnumEntryRegex;
    std::regex pokemonCountRegex;
    std::string pokemonCountHint;
    std::string macro;
};

static const struct PokemonGroupData hoennPokemonGroupData
{
    .id = "EC_GROUP_POKEMON",
    .label = "gEasyChatGroup_Pokemon",
    .pokemonEnumEntryRegex = std::regex("HOENN_DEX_(\\w+),"),
    .pokemonCountRegex = std::regex("#define HOENN_DEX_COUNT +HOENN_DEX_(\\w+)"),
    .pokemonCountHint = "HOENN_DEX_COUNT",
    .macro = "EC_POKEMON",
};
static const struct PokemonGroupData kantoJohtoPokemonGroupData
{
    .id = "EC_GROUP_POKEMON_NATIONAL",
    .label = "gEasyChatGroup_Pokemon2",
    .pokemonEnumEntryRegex = std::regex("NATIONAL_DEX_(\\w+),"),
    .pokemonCountRegex = std::regex("#define JOHTO_DEX_COUNT +NATIONAL_DEX_(\\w+)"),
    .pokemonCountHint = "JOHTO_DEX_COUNT",
    .macro = "EC_POKEMON_NATIONAL",
};

static const PokemonGroupData& pokemonGroupData(DexGroup dex_group)
{
    switch (dex_group)
    {
    case DexGroup::HOENN:
        return hoennPokemonGroupData;
    case DexGroup::KANTOJOHTO:
        return kantoJohtoPokemonGroupData;
    }
    FATAL_ERROR("illegal argument\n");
}


WordInfoList parsePokemon(
        DexGroup dex_group,
        const std::filesystem::path& species_names_file,
        const std::filesystem::path& pokedex_file)
{
    const PokemonGroupData& dex_group_data = pokemonGroupData(dex_group);

    string pokedex_str = read_text_file(pokedex_file);
    std::vector<string> full_dex;
    {
        std::regex entry_regex(dex_group_data.pokemonEnumEntryRegex);
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
        std::regex dex_count_regex(dex_group_data.pokemonCountRegex);
        std::smatch m;
        std::regex_search(pokedex_str.cbegin(), pokedex_str.cend(), m, dex_count_regex);
        if (m.empty())
            FATAL_ERROR("malformed input: did not find %s\n", dex_group_data.pokemonCountHint.c_str());
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
                id += dex_group_data.macro;
                id += "(";
                id += i->str(1);
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
        dex_group_data.id,
        dex_group_data.label,
        WordInfoListType::VALUE_LIST,
        words_out);
    return retval;
}

WordInfoList parseMoves(
        const std::filesystem::path& move_names_file,
        const std::filesystem::path& move_order_file)
{
    string move_order_str = read_text_file(move_order_file);
    std::vector<string> moves;
    {
        std::regex entry_regex("MOVE_(\\w+),");
        auto entries_begin = std::sregex_iterator(move_order_str.cbegin(), move_order_str.cend(), entry_regex);
        auto entries_end = std::sregex_iterator();

        for (std::sregex_iterator i = entries_begin; i != entries_end; ++i)
        {
            moves.push_back(i->str(1));
        }
    }

    string id;
    string label;
    string label_number;
    {
        std::regex label_regex("const u16 (gEasyChatGroup_Move)(\\d)\\[\\] = \\{");
        std::smatch m;
        std::regex_search(move_order_str.cbegin(), move_order_str.cend(), m, label_regex);
        if (m.empty())
            FATAL_ERROR("malformed input: did not find declaration\n");
        label = m.str(1) + m.str(2);
        label_number = m.str(2);
        id = "EC_GROUP_MOVE_";
        id += label_number;
    }

    string move_names_str = read_text_file(move_names_file);
    std::vector<WordInfo> words_out;
    {
        std::regex entry_regex("\\[MOVE_(\\w+)\\]\\s*=\\s*_\\(\"([^\"]+)\"\\),");
        auto entries_begin = std::sregex_iterator(move_names_str.cbegin(), move_names_str.cend(), entry_regex);
        auto entries_end = std::sregex_iterator();

        for (std::sregex_iterator i = entries_begin; i != entries_end; ++i)
        {
            if (moves.end() != std::find(moves.begin(), moves.end(), i->str(1)))
            {
                string index("MOVE_");
                index += i->str(1);

                string id;
                id += "EC_MOVE";
                if ("1" != label_number)
                    id += label_number;
                id += "(";
                id += i->str(1);
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
        id,
        label,
        WordInfoListType::VALUE_LIST,
        words_out);
    return retval;
}

std::vector<std::string> parseUnusedLettersFile(
        const std::filesystem::path& unused_letters_file)
{
    std::vector<string> unused_letter_suffixes;
    if (std::filesystem::exists(unused_letters_file))
    {
        string unused_letters_str = read_text_file(unused_letters_file);
        {
            std::regex entry_regex("const u16 gEasyChatWordsByLetter_(\\w+)\\[\\] = \\{");
            auto entries_begin = std::sregex_iterator(unused_letters_str.cbegin(), unused_letters_str.cend(), entry_regex);
            auto entries_end = std::sregex_iterator();

            for (std::sregex_iterator i = entries_begin; i != entries_end; ++i)
            {
                unused_letter_suffixes.push_back(i->str(1));
            }
        }
    }
    return unused_letter_suffixes;
}

std::map<string, int> parseGroupConstants(
        const std::filesystem::path& easy_chat_constants_file)
{
    std::map<string, int> group_constants;
    if (std::filesystem::exists(easy_chat_constants_file))
    {
        string easy_chat_constants_str = read_text_file(easy_chat_constants_file);
        {
            std::regex entry_regex("#define (EC_GROUP_\\w+) +(\\d+)");
            auto entries_begin = std::sregex_iterator(easy_chat_constants_str.cbegin(), easy_chat_constants_str.cend(), entry_regex);
            auto entries_end = std::sregex_iterator();

            for (std::sregex_iterator i = entries_begin; i != entries_end; ++i)
            {
                group_constants.emplace(i->str(1), atoi(i->str(2).c_str()));
            }
        }
    }
    return group_constants;
}

void writeWordInfo(
        const std::filesystem::path& output_file,
        const WordInfoList& data,
        const DoNotModifyHeader& header)
{
    std::vector<string> texts;
    for (WordInfo word : data.words)
    {
        texts.push_back(word.clean_text());
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
    std::set<WordInfo, WordInfoCmpByText> sorted_words(data.words.begin(), data.words.end());

    std::ofstream output_stream(output_file);

    output_stream << header;

    output_stream
        << "const u16 "
        << data.label
        << "[] = {"
        << std::endl;

    for (WordInfo word : sorted_words)
    {
        output_stream
            << "    "
            << word.index
            << ","
            << std::endl;
    }

    output_stream
        << "};"
        << std::endl;
}

void writeGroups(
        const std::filesystem::path& output_file,
        const std::vector<WordInfoList>& datass,
        const DoNotModifyHeader& header)
{
    std::ofstream output_stream(output_file);

    output_stream << header
            << "#include \"easy_chat.h\"" << std::endl
            << std::endl
            << "#include \"easy_chat_group_pokemon.h\"" << std::endl
            << "#include \"easy_chat_group_trainer.h\"" << std::endl
            << "#include \"easy_chat_group_status.h\"" << std::endl
            << "#include \"easy_chat_group_battle.h\"" << std::endl
            << "#include \"easy_chat_group_greetings.h\"" << std::endl
            << "#include \"easy_chat_group_people.h\"" << std::endl
            << "#include \"easy_chat_group_voices.h\"" << std::endl
            << "#include \"easy_chat_group_speech.h\"" << std::endl
            << "#include \"easy_chat_group_endings.h\"" << std::endl
            << "#include \"easy_chat_group_feelings.h\"" << std::endl
            << "#include \"easy_chat_group_conditions.h\"" << std::endl
            << "#include \"easy_chat_group_actions.h\"" << std::endl
            << "#include \"easy_chat_group_lifestyle.h\"" << std::endl
            << "#include \"easy_chat_group_hobbies.h\"" << std::endl
            << "#include \"easy_chat_group_time.h\"" << std::endl
            << "#include \"easy_chat_group_misc.h\"" << std::endl
            << "#include \"easy_chat_group_adjectives.h\"" << std::endl
            << "#include \"easy_chat_group_events.h\"" << std::endl
            << "#include \"easy_chat_group_move_1.h\"" << std::endl
            << "#include \"easy_chat_group_move_2.h\"" << std::endl
            << "#include \"easy_chat_group_trendy_saying.h\"" << std::endl
            << "#include \"easy_chat_group_pokemon2.h\"" << std::endl
            << std::endl
            << "const struct EasyChatGroup gEasyChatGroups[] = {"
            << std::endl;

    for (WordInfoList datas : datass)
    {
        output_stream
            << "    [" << datas.id << "] =" << std::endl
            << "    {" << std::endl;

        switch (datas.type)
        {
        case WordInfoListType::VALUE_LIST:
            output_stream
                << "        .wordData = {.valueList = " << datas.label << "}," << std::endl;
            break;
        case WordInfoListType::WORDS:
            output_stream
                << "        .wordData = {.words = " << datas.label << "}," << std::endl;
            break;
        }

        output_stream
            << "        .numWords = ARRAY_COUNT(" << datas.label << ")," << std::endl;

        unsigned num_enabled_words = std::count_if(
            datas.words.begin(), datas.words.end(),
            [](WordInfo word) { return word.enabled; }
        );

        output_stream
            << "        .numEnabledWords = " << num_enabled_words << "," << std::endl;

        output_stream
            << "    }," << std::endl;
    }

    output_stream << "};" << std::endl;
}

void writeByLetter(
        const std::filesystem::path& output_file,
        const std::vector<WordInfoList>& datass,
        const std::vector<string>& unused_letter_suffixes,
        const DoNotModifyHeader& header)
{
    std::ofstream output_stream(output_file);

    std::set<WordInfo, WordInfoCmpByText> sorted_words;

    for (WordInfoList datas : datass)
    for (WordInfo data : datas.words)
    {
        sorted_words.insert(data);
    }

    output_stream << header;

    output_stream
        << "#include \"easy_chat.h\""
        << std::endl
        << std::endl
        << "#define DOUBLE_SPECIES_NAME EC_EMPTY_WORD, 2,"
        << std::endl
        << std::endl;

    {
        const char before_letter_str[2] = {'A', '\0'};
        const char after_letter_str[2] = {static_cast<char>('Z' + 1), '\0'};

        output_stream
            << "const u16 gEasyChatWordsByLetter_Others[] = {"
            << std::endl;

        WordInfo before_letter_key("", "", before_letter_str, "", false);
        WordInfo after_letter_key("", "", after_letter_str, "", false);

        auto before_letter_bound = sorted_words.lower_bound(before_letter_key);
        auto after_letter_bound = sorted_words.lower_bound(after_letter_key);

        for (auto wordi = sorted_words.begin(); wordi != before_letter_bound; ++wordi)
        {
            output_stream << "    " << wordi->id << "," << std::endl;
        }

        for (auto wordi = after_letter_bound; wordi != sorted_words.end(); ++wordi)
        {
            output_stream << "    " << wordi->id << "," << std::endl;
        }

        output_stream
            << "};"
            << std::endl
            << std::endl;
    }

    for (char letter_c = 'A'; letter_c <= 'Z'; ++letter_c)
    {
        const char letter[2] = {letter_c, '\0'};
        const char next_letter[2] = {static_cast<char>(letter_c + 1), '\0'};

        output_stream
            << "const u16 gEasyChatWordsByLetter_"
            << letter
            << "[] = {"
            << std::endl;

        WordInfo lower_key("", "", letter, "", false);
        WordInfo upper_key("", "", next_letter, "", false);

        auto lower_bound = sorted_words.lower_bound(lower_key);
        auto upper_bound = sorted_words.lower_bound(upper_key);

        for (auto wordi = lower_bound; wordi != upper_bound; ++wordi)
        {
            string word_text(wordi->text);
            ++wordi;
            if (wordi != upper_bound)
            {
                if (wordi->text == word_text) {
                    output_stream << "    DOUBLE_SPECIES_NAME" << std::endl;
                }
            }
            --wordi;

            output_stream
                << "    "
                << wordi->id
                << ","
                << std::endl;
        }

        output_stream
            << "};"
            << std::endl
            << std::endl;
    }

    if (0 != unused_letter_suffixes.size())
    {
        output_stream
                << "#include \"easy_chat_words_by_letter_unused.h\""
                << std::endl
                << std::endl;
    }

    std::vector<string> group_suffixes;
    group_suffixes.emplace_back("Others");
    for (char letter_c = 'A'; letter_c <= 'Z'; ++letter_c)
    {
        const char letter_s[2] = {letter_c, '\0'};
        group_suffixes.emplace_back(letter_s);
    }
    for (string unusedLetterSuffix : unused_letter_suffixes)
    {
        group_suffixes.emplace_back(unusedLetterSuffix);
    }

    output_stream
            << "const struct EasyChatWordsByLetter gEasyChatWordsByLetterPointers[EC_NUM_ALPHABET_GROUPS + " << unused_letter_suffixes.size() << "] = { // " << unused_letter_suffixes.size() << " unused JP groups"
            << std::endl;

    for (string group_suffix : group_suffixes)
    {
        string group_label("gEasyChatWordsByLetter_");
        group_label += group_suffix;

        output_stream
            << "    {" << std::endl
            << "        .words = " << group_label << "," << std::endl
            << "        .numWords = ARRAY_COUNT(" << group_label << ")," << std::endl
            << "    }," << std::endl
            ;
    }

    output_stream << "};" << std::endl;
}
