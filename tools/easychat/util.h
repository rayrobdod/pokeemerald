#ifndef UTIL_H
#define UTIL_H

#include <filesystem>
#include <string>

std::string read_text_file(const std::filesystem::path);

void search_replace_all(
        std::string& haystack,
        const std::string needle,
        const std::string replacement);

#endif // UTIL_H
