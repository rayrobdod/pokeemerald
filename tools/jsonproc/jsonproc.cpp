// jsonproc.cpp
// jsonproc converts JSON data to an output file based on an Inja template. 
// https://github.com/pantor/inja

#include "jsonproc.h"

#include <map>

#include <string>
using std::string; using std::to_string;

#include <algorithm>
using std::replace_if;

#include <inja.hpp>
using namespace inja;
using json = nlohmann::json;

std::map<string, string> customVars;

void set_custom_var(string key, string value)
{
    customVars[key] = value;
}

string get_custom_var(string key)
{
    return customVars[key];
}

int main(int argc, char *argv[])
{
    if (argc != 4)
        FATAL_ERROR("USAGE: jsonproc <json-filepath> <template-filepath> <output-filepath>\n");

    string jsonfilepath = argv[1];
    string templateFilepath = argv[2];
    string outputFilepath = argv[3];

    Environment env;
    env.set_trim_blocks(true);

    // Add custom command callbacks.
    env.add_callback("doNotModifyHeader", 0, [jsonfilepath, templateFilepath](Arguments& args) {
        return "//\n// DO NOT MODIFY THIS FILE! It is auto-generated from " + jsonfilepath +" and Inja template " + templateFilepath + "\n//\n";
    });

    env.add_callback("subtract", 2, [](Arguments& args) {
        int minuend = args.at(0)->get<int>();
        int subtrahend = args.at(1)->get<int>();

        return minuend - subtrahend;
    });

    env.add_callback("setVar", 2, [=](Arguments& args) {
        string key = args.at(0)->get<string>();
        string value = args.at(1)->get<string>();
        set_custom_var(key, value);
        return "";
    });

    env.add_callback("setVarInt", 2, [=](Arguments& args) {
        string key = args.at(0)->get<string>();
        string value = to_string(args.at(1)->get<int>());
        set_custom_var(key, value);
        return "";
    });

    env.add_callback("getVar", 1, [=](Arguments& args) {
        string key = args.at(0)->get<string>();
        return get_custom_var(key);
    });

    env.add_callback("concat", 2, [](Arguments& args) {
        string first = args.at(0)->get<string>();
        string second = args.at(1)->get<string>();
        return first + second;
    });

    env.add_callback("removePrefix", 2, [](Arguments& args) {
        string rawValue = args.at(0)->get<string>();
        string prefix = args.at(1)->get<string>();
        string::size_type i = rawValue.find(prefix);
        if (i != 0)
            return rawValue;

        return rawValue.erase(0, prefix.length());
    });

    env.add_callback("removeSuffix", 2, [](Arguments& args) {
        string rawValue = args.at(0)->get<string>();
        string suffix = args.at(1)->get<string>();
        string::size_type i = rawValue.rfind(suffix);
        if (i == string::npos)
            return rawValue;

        return rawValue.substr(0, i);
    });

    // single argument is a json object
    env.add_callback("isEmpty", 1, [](Arguments& args) {
        return args.at(0)->empty();
    });

    env.add_callback("isEmptyString", 1, [](Arguments& args) {
        return args.at(0)->get<string>().empty();
    });

    env.add_callback("cleanString", 1, [](Arguments& args) {
        string str = args.at(0)->get<string>();
        for (unsigned int i = 0; i < str.length(); i++) {
            // This code is not Unicode aware, so UTF-8 is not easily parsable without introducing
            // another library. Just filter out any non-alphanumeric characters for now.
            // TODO: proper Unicode string normalization
            if ((i == 0 && isdigit(str[i]))
             || !isalnum(str[i])) {
                str[i] = '_';
            }
        }
        return str;
    });

    // returns an array with all the items of [1] followed by [2]
    env.add_callback("append", 2, [](Arguments& args) {
        auto collection = args.at(0)->get<std::vector<json>>();
        auto item = args.at(1)->get<json>();
        collection.push_back(item);
        return collection;
    });

    // returns the index of [2] in array [1]
    env.add_callback("indexOf", 2, [](Arguments& args) {
        auto haystack = args.at(0)->get<std::vector<json>>();
        auto needle = args.at(1)->get<json>();
        int index = 0;
        for (auto it = haystack.begin(); it != haystack.end(); ++it) {
            if (needle == it->get<json>())
                return index;
            ++index;
        }
        return -1;
    });

    try
    {
        env.write_with_json_file(templateFilepath, jsonfilepath, outputFilepath);
    }
    catch (const std::exception& e)
    {
        FATAL_ERROR("JSONPROC_ERROR: %s\n", e.what());
    }

    return 0;
}
