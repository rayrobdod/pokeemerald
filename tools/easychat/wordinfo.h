#ifndef WORDINFO_H
#define WORDINFO_H

#include <filesystem>
#include <initializer_list>
#include <map>
#include <ostream>
#include <string>
#include <vector>

struct WordInfo {
public:
    std::string id;
    std::string index;
    std::string text;
    std::string text_label;
    bool enabled;

    WordInfo(
        std::string id,
        std::string index,
        std::string text,
        std::string text_label,
        bool enabled);

    std::string clean_text() const;
};

enum class WordInfoListType {
	VALUE_LIST,
	WORDS,
};

struct WordInfoList {
public:
    std::string id;
    std::string label;
    WordInfoListType type;
    std::vector<WordInfo> words;

    WordInfoList(
        std::string id,
        std::string label,
        WordInfoListType type,
        std::vector<WordInfo> words);
};

class DoNotModifyHeader {
private:
    std::vector<std::filesystem::path> from_files;

public:
    DoNotModifyHeader(const std::filesystem::path& from_file);
    DoNotModifyHeader(const std::initializer_list<std::filesystem::path>& from_files);

    template<class Traits>
    void write_to(std::basic_ostream<char, Traits>& os) const
    {
        os
            << "//" << std::endl
            << "// DO NOT MODIFY THIS FILE! It is auto-generated ";
        if (! this->from_files.empty())
        {
            os << "from ";
            for (auto f : this->from_files)
            {
                os << f << ", ";
            }
        }
        os
            << "by tools/easychat/easychat"
            << std::endl
            << "//" << std::endl
            << std::endl;
    }
};

template<class Traits>
std::basic_ostream<char, Traits>& operator<<(
    std::basic_ostream<char, Traits>& os, const DoNotModifyHeader& value )
{
    value.write_to(os);
    return os;
}

enum class DexGroup {
    HOENN,
    KANTOJOHTO,
};


WordInfoList parseJson(const std::filesystem::path& input_file);

WordInfoList parsePokemon(
        DexGroup dex_group,
        const std::filesystem::path& species_names_file,
        const std::filesystem::path& pokedex_file);

WordInfoList parseMoves(
        const std::filesystem::path& move_names_file,
        const std::filesystem::path& move_order_file);

std::vector<std::string> parseUnusedLettersFile(
        const std::filesystem::path& unused_letters_file);

std::map<std::string, int> parseGroupConstants(
        const std::filesystem::path& easy_chat_constants_file);

void writeWordInfo(const std::filesystem::path& output_file, const WordInfoList&, const DoNotModifyHeader&);

void writeValueList(const std::filesystem::path& output_file, const WordInfoList&, const DoNotModifyHeader&);

void writeGroups(
        const std::filesystem::path& output_file,
        const std::vector<WordInfoList>&,
        const DoNotModifyHeader&);

void writeByLetter(
        const std::filesystem::path& output_file,
        const std::vector<WordInfoList>&,
        const std::vector<std::string>& unused_letter_suffixes,
        const DoNotModifyHeader&);

#endif // WORDINFO_H
