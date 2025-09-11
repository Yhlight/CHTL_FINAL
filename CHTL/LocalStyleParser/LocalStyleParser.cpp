#include "LocalStyleParser.h"
#include "../../Util/StringUtil/StringUtil.h"
#include <algorithm>

// Pre-processes the style block content to remove comments.
static std::string removeComments(const std::string& input) {
    std::string output;
    output.reserve(input.length());
    for (size_t i = 0; i < input.length(); ++i) {
        if (i + 1 < input.length()) {
            if (input[i] == '/' && input[i+1] == '/') {
                // Line comment, skip to newline
                size_t end_of_line = input.find('\n', i);
                if (end_of_line == std::string::npos) break;
                i = end_of_line -1; // loop will increment to end_of_line
                continue;
            }
            if (input[i] == '/' && input[i+1] == '*') {
                // Block comment, skip to */
                size_t end_of_comment = input.find("*/", i + 2);
                if (end_of_comment == std::string::npos) break;
                i = end_of_comment + 1; // position on the '/'
                continue;
            }
        }
        output += input[i];
    }
    return output;
}

ParsedStyleBlock LocalStyleParser::parse(const std::string& rawContent) {
    ParsedStyleBlock result;
    std::string content = removeComments(rawContent);
    size_t pos = 0;

    while (pos < content.length()) {
        size_t colon_pos = content.find(':', pos);
        size_t brace_pos = content.find('{', pos);

        size_t start_of_meaningful_content = content.find_first_not_of(" \t\n\r", pos);
        if (start_of_meaningful_content == std::string::npos) {
            break;
        }
        pos = start_of_meaningful_content;

        if (colon_pos != std::string::npos && (brace_pos == std::string::npos || colon_pos < brace_pos)) {
            size_t semi_pos = content.find(';', colon_pos);
            if (semi_pos == std::string::npos) {
                semi_pos = content.length();
            }

            std::string key = trim(std::string_view(content).substr(pos, colon_pos - pos));
            std::string value = trim(std::string_view(content).substr(colon_pos + 1, semi_pos - (colon_pos + 1)));

            if (!key.empty() && !value.empty()) {
                result.inlineStyles[key] = value;
            }
            pos = semi_pos + 1;
        } else if (brace_pos != std::string::npos) {
            std::string selector = trim(std::string_view(content).substr(pos, brace_pos - pos));

            int brace_count = 1;
            size_t content_end_pos = brace_pos + 1;
            while (content_end_pos < content.length() && brace_count > 0) {
                if (content[content_end_pos] == '{') brace_count++;
                else if (content[content_end_pos] == '}') brace_count--;
                content_end_pos++;
            }

            std::string rule_content;
            if (brace_count == 0) {
                 rule_content = trim(std::string_view(content).substr(brace_pos + 1, content_end_pos - brace_pos - 2));
            } else {
                content_end_pos = content.length();
            }

            if (!selector.empty() && !rule_content.empty()) {
                result.globalRules.push_back({selector, rule_content});
            }
            pos = content_end_pos;
        } else {
            break;
        }
    }

    return result;
}
