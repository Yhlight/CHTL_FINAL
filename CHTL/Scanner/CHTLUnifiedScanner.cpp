#include "CHTLUnifiedScanner.h"
#include <stdexcept>
#include <iostream>

namespace CHTL {

CHTLUnifiedScanner::CHTLUnifiedScanner(const std::string& source)
    : source(source) {}

std::vector<CodeFragment> CHTLUnifiedScanner::scan() {
    std::vector<CodeFragment> fragments;
    std::string buffer;
    int line_start = 1;
    int brace_level = 0;

    for (current = 0; current < source.length(); ++current) {
        if (source[current] == '\n') line++;

        if (source[current] == '/' && current + 1 < source.length()) {
            if (source[current + 1] == '/') {
                while(current < source.length() && source[current] != '\n') current++;
                if (current < source.length()) line++;
                continue;
            }
            if (source[current + 1] == '*') {
                 current += 2;
                 while(current + 1 < source.length() && (source[current] != '*' || source[current+1] != '/')) {
                     if (source[current] == '\n') line++;
                     current++;
                 }
                 current++;
                 continue;
            }
        }

        std::string current_substr = source.substr(current);
        bool is_style = current_substr.rfind("style", 0) == 0;
        bool is_script = current_substr.rfind("script", 0) == 0;

        int lookahead = current + (is_style ? 5 : (is_script ? 6 : 0));
        while(lookahead < source.length() && isspace(source[lookahead])) {
            lookahead++;
        }

        if (brace_level == 0 && (is_style || is_script) && lookahead < source.length() && source[lookahead] == '{') {
            if (!buffer.empty()) {
                fragments.push_back({FragmentType::CHTL, buffer, line_start});
                buffer.clear();
            }

            int content_start = lookahead + 1;
            int content_line_start = line;

            current = content_start;
            int inner_brace_level = 1;
            while(current < source.length() && inner_brace_level > 0) {
                if (source[current] == '\n') line++;
                if(source[current] == '{') inner_brace_level++;
                if(source[current] == '}') inner_brace_level--;
                current++;
            }

            int content_end = current - 1;
            std::string block_content = source.substr(content_start, content_end - content_start);

            FragmentType type = is_script ? FragmentType::CHTL_JS : FragmentType::CSS;
            fragments.push_back({type, block_content, content_line_start});

            current--;
            line_start = line;
            continue;
        }

        buffer += source[current];
    }

    if (!buffer.empty()) {
        fragments.push_back({FragmentType::CHTL, buffer, line_start});
    }

    return fragments;
}

} // namespace CHTL
