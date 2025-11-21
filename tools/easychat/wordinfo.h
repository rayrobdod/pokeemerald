#ifndef WORDINFO_H
#define WORDINFO_H

#include <filesystem>
#include <initializer_list>
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
};

struct WordInfoList {
public:
    std::string label;
    std::vector<WordInfo> words;

    WordInfoList(
        std::string label,
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
            << "// DO NOT MODIFY THIS FILE! It is auto-generated from ";
        for (auto f : this->from_files) {
            os << f << ", ";
        }
        os
            << " by tools/easychat/easychat"
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

void writeWordInfo(const std::filesystem::path& output_file, const WordInfoList&, const DoNotModifyHeader&);

void writeValueList(const std::filesystem::path& output_file, const WordInfoList&, const DoNotModifyHeader&);

#endif // WORDINFO_H
