#include "Preprocessor.h"
#include <regex>

std::string CHTLJS_Preprocessor::preprocess(std::string& source) {
    std::string result = source;
    std::regex pattern(R"(\{\{.*?\}\})");
    std::smatch match;

    std::string temp_result;
    auto search_start = result.cbegin();

    while (std::regex_search(search_start, result.cend(), match, pattern))
    {
        std::string placeholder = "__CHTLJS_PLACEHOLDER_" + std::to_string(placeholder_index++) + "__";
        temp_result += match.prefix().str();
        temp_result += placeholder;
        search_start = match.suffix().first;
    }
    temp_result += std::string(search_start, result.cend());

    return temp_result;
}
