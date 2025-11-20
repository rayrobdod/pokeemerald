#include "mode.h"

#include "global.h"
#include "json11.h"
#include "util.h"
#include <algorithm>
#include <filesystem>
#include <fstream>

using std::string;
using json11::Json;

void mode_wordinfo(
        std::filesystem::path input_file,
        std::filesystem::path output_file)
{
        string input_err;

        string input_str = read_text_file(input_file);
        Json input_data = Json::parse(input_str, input_err);
        if (input_data.is_null())
            FATAL_ERROR("%s\n", input_err.c_str());

        if (! input_data["words"].is_array())
            FATAL_ERROR("malformed input: `.words` is not an array\n");

        const std::vector<Json> words = input_data["words"].array_items();

        std::vector<string> texts;
        for (Json word : words)
        {
            string text = word["text"].string_value();
            search_replace_all(text, "…", "...");
            search_replace_all(text, "{", "");
            search_replace_all(text, "}", "");
            texts.push_back(text);
        }
        std::vector<string> sorted_texts(texts);
        std::sort(sorted_texts.begin(), sorted_texts.end());

        std::ofstream output_stream(output_file);

        output_stream
            << "//"
            << std::endl
            << "// DO NOT MODIFY THIS FILE! It is auto-generated from "
            << input_file
            << " by tools/easychat/easychat"
            << std::endl
            << "//"
            << std::endl << std::endl;

        for (Json word : words)
        {
            output_stream
                << "const u8 "
                << word["text_label"].string_value()
                << "[] = _(\""
                << word["text"].string_value()
                << "\");"
                << std::endl;
        }

        output_stream
            << std::endl
            << "const struct EasyChatWordInfo "
            << input_data["label"].string_value()
            << "[] = {"
            << std::endl;

        for (size_t i = 0; i < words.size(); i++)
        {
            Json word = words[i];
            size_t order = std::find(texts.begin(), texts.end(), sorted_texts[i]) - texts.begin();
            Json order_word = words[order];

            const std::map<std::string, Json>& word_as_map = word.object_items();
            auto enabled_ptr = word_as_map.find("enabled");
            bool enabled = true;
            if (enabled_ptr != word_as_map.end())
                enabled = enabled_ptr->second.bool_value();

            output_stream
                << "    [EC_INDEX("
                << word["id"].string_value()
                << ")] ="
                << std::endl
                << "    {"
                << std::endl
                << "        .text = "
                << word["text_label"].string_value()
                << ","
                << std::endl
                << "        .alphabeticalOrder = EC_INDEX("
                << order_word["id"].string_value()
                << "),"
                << std::endl
                << "        .enabled = "
                << (enabled ? "TRUE" : "FALSE")
                << ","
                << std::endl
                << "    },"
                << std::endl;
        }

        output_stream
            << "};"
            << std::endl;

}
