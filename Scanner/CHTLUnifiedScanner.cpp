#include "CHTLUnifiedScanner.h"
#include <iostream>
#include <cctype>

size_t find_valid_block(const std::string& source, const std::string& keyword, size_t search_pos, size_t& open_brace_pos) {
    size_t current_pos = search_pos;
    while (true) {
        size_t keyword_pos = source.find(keyword, current_pos);
        if (keyword_pos == std::string::npos) {
            return std::string::npos;
        }

        size_t after_keyword = keyword_pos + keyword.length();
        size_t brace_search_pos = after_keyword;

        while (brace_search_pos < source.length() && isspace(source[brace_search_pos])) {
            brace_search_pos++;
        }

        if (brace_search_pos < source.length() && source[brace_search_pos] == '{') {
            open_brace_pos = brace_search_pos;
            return keyword_pos;
        }

        current_pos = keyword_pos + 1;
    }
    return std::string::npos;
}

std::vector<CodeFragment> CHTLUnifiedScanner::scan(const std::string& source) {
    std::vector<CodeFragment> fragments;
    size_t pos = 0;

    while (pos < source.length()) {
        size_t style_brace_pos, script_brace_pos;
        size_t style_keyword_pos = find_valid_block(source, "style", pos, style_brace_pos);
        size_t script_keyword_pos = find_valid_block(source, "script", pos, script_brace_pos);

        size_t next_keyword_pos = std::string::npos;
        size_t next_brace_pos = std::string::npos;
        FragmentType block_type = FragmentType::CHTL;

        if (style_keyword_pos != std::string::npos && (script_keyword_pos == std::string::npos || style_keyword_pos < script_keyword_pos)) {
            next_keyword_pos = style_keyword_pos;
            next_brace_pos = style_brace_pos;
            block_type = FragmentType::CSS;
        } else if (script_keyword_pos != std::string::npos) {
            next_keyword_pos = script_keyword_pos;
            next_brace_pos = script_brace_pos;
            block_type = FragmentType::JS;
        }

        if (next_keyword_pos == std::string::npos) {
            std::string remaining_chunk = source.substr(pos);
            if (!remaining_chunk.empty()) {
                fragments.push_back({remaining_chunk, FragmentType::CHTL});
            }
            break;
        }

        std::string chtl_chunk = source.substr(pos, next_keyword_pos - pos);
        if (!chtl_chunk.empty()) {
            fragments.push_back({chtl_chunk, FragmentType::CHTL});
        }

        int brace_count = 1;
        size_t close_brace_pos = next_brace_pos + 1;
        while (close_brace_pos < source.length() && brace_count > 0) {
            if (source[close_brace_pos] == '{') {
                brace_count++;
            } else if (source[close_brace_pos] == '}') {
                brace_count--;
            }
            close_brace_pos++;
        }

        if (brace_count != 0) {
            std::string remaining_chunk = source.substr(next_keyword_pos);
            if (!remaining_chunk.empty()) {
                fragments.push_back({remaining_chunk, FragmentType::CHTL});
            }
            break;
        }

        std::string block_content = source.substr(next_brace_pos + 1, close_brace_pos - next_brace_pos - 2);
        fragments.push_back({block_content, block_type});

        pos = close_brace_pos;
    }

    return fragments;
}
