#include "global.h"
#include "util.h"
#include <algorithm>
#include <fstream>

using std::string;

string read_text_file(const std::filesystem::path filepath) {
    std::ifstream in_file(filepath);

    if (!in_file.is_open())
        FATAL_ERROR("Cannot open file %s for reading.\n", filepath.c_str());

    string text;

    in_file.seekg(0, std::ios::end);
    text.resize(in_file.tellg());

    in_file.seekg(0, std::ios::beg);
    in_file.read(&text[0], text.size());

    in_file.close();

    return text;
}

void search_replace_all(string& haystack, const string needle, const string replacement) {
    auto position = std::search(haystack.begin(), haystack.end(), needle.begin(), needle.end());
    while (haystack.end() != position)
    {
        haystack.replace(position, position + needle.size(), replacement);
        position = std::search(haystack.begin(), haystack.end(), needle.begin(), needle.end());
    }
}
