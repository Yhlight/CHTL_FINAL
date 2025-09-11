#include "LocalStyleParser.h"
#include <algorithm>

// Helper function to trim leading/trailing whitespace from a string
static void trim(std::string& s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    }));
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
    }).base(), s.end());
}

std::map<std::string, std::string> LocalStyleParser::parse(const std::string& rawContent) {
    std::map<std::string, std::string> styles;
    size_t pos = 0;

    while (pos < rawContent.length()) {
        size_t semi_pos = rawContent.find(';', pos);
        if (semi_pos == std::string::npos) {
            semi_pos = rawContent.length(); // Handle last property without a semicolon
        }

        std::string declaration = rawContent.substr(pos, semi_pos - pos);
        size_t colon_pos = declaration.find(':');

        if (colon_pos != std::string::npos) {
            std::string key = declaration.substr(0, colon_pos);
            std::string value = declaration.substr(colon_pos + 1);

            trim(key);
            trim(value);

            if (!key.empty() && !value.empty()) {
                styles[key] = value;
            }
        }

        pos = semi_pos + 1;
    }

    return styles;
}
