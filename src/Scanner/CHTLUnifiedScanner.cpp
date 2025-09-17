#include "CHTLUnifiedScanner.h"
#include <stdexcept>
#include <cctype>
#include <algorithm>
#include <iostream>
#include <vector>
#include <sstream>

namespace CHTL {

// Helper to check if a substring matches at a certain position
bool startsWith(const std::string& haystack, const std::string& needle, size_t pos) {
    if (pos + needle.length() > haystack.length()) {
        return false;
    }
    return haystack.substr(pos, needle.length()) == needle;
}

// CHTL JS Keywords
const std::vector<std::string> CHTLJS_KEYWORDS = {
    "{{", "Listen", "listen", "delegate", "animate", "vir", "router", "&->", "->"
};

// Skips whitespace and comments
size_t skipWhitespaceAndComments(const std::string& text, size_t pos) {
    while (pos < text.length()) {
        if (isspace(text[pos])) {
            pos++;
            continue;
        }
        if (startsWith(text, "//", pos)) {
            pos = text.find('\n', pos);
            if (pos == std::string::npos) return text.length();
            continue;
        }
        if (startsWith(text, "/*", pos)) {
            pos = text.find("*/", pos);
            if (pos == std::string::npos) return text.length();
            pos += 2;
            continue;
        }
        break;
    }
    return pos;
}

CHTLUnifiedScanner::CHTLUnifiedScanner(const std::string& source)
    : source_(source) {}

const std::map<std::string, std::string>& CHTLUnifiedScanner::getPlaceholderMap() const {
    return placeholder_map_;
}

std::vector<CodeFragment> CHTLUnifiedScanner::scan() {
    process();
    return fragments_;
}

void CHTLUnifiedScanner::addFragment(const std::string& content, FragmentType type) {
    if (!content.empty() && content.find_first_not_of(" \t\n\r") != std::string::npos) {
        fragments_.push_back({content, type});
    }
}

std::string CHTLUnifiedScanner::createPlaceholder(const std::string& content) {
    if (content.empty() || content.find_first_not_of(" \t\n\r") == std::string::npos) {
        return content; // Return original whitespace/empty string
    }
    std::string placeholder = "_JS_CODE_PLACEHOLDER_" + std::to_string(placeholder_id_++) + "_";
    placeholder_map_[placeholder] = content;
    return placeholder;
}

size_t CHTLUnifiedScanner::findEndOfBlock(const std::string& text, size_t start_pos, char open_brace, char close_brace) {
    int brace_level = 1;
    bool in_string = false;
    char string_char = '\0';
    for (size_t i = start_pos; i < text.length(); ++i) {
        char current_char = text[i];
        char next_char = (i + 1 < text.length()) ? text[i + 1] : '\0';
        if (in_string) {
            if (current_char == '\\') { i++; }
            else if (current_char == string_char) { in_string = false; }
            continue;
        }
        if (current_char == '"' || current_char == '\'' || current_char == '`') {
            in_string = true;
            string_char = current_char;
            continue;
        }
        if (current_char == '/' && next_char == '/') {
            i = text.find('\n', i);
            if (i == std::string::npos) return std::string::npos;
            continue;
        }
        if (current_char == '/' && next_char == '*') {
            i = text.find("*/", i + 2);
            if (i == std::string::npos) return std::string::npos;
            i++;
            continue;
        }
        if (current_char == open_brace) { brace_level++; }
        else if (current_char == close_brace) {
            brace_level--;
            if (brace_level == 0) return i;
        }
    }
    return std::string::npos;
}

bool isKeywordCandidate(const std::string& text, size_t pos, const std::string& keyword) {
    if (!startsWith(text, keyword, pos)) return false;
    if (isalpha(keyword[0])) {
        if (pos > 0 && isalnum(text[pos - 1])) return false;
        size_t end = pos + keyword.length();
        if (end < text.length() && isalnum(text[end])) return false;
    }
    return true;
}

std::string CHTLUnifiedScanner::scanJsAndChtlJs(const std::string& script_content) {
    std::stringstream result_stream;
    size_t current_pos = 0;
    size_t last_flush_pos = 0;
    while (current_pos < script_content.length()) {
        size_t next_keyword_pos = std::string::npos;
        std::string found_keyword;
        for (const auto& keyword : CHTLJS_KEYWORDS) {
            size_t pos = script_content.find(keyword, current_pos);
            if (pos != std::string::npos && isKeywordCandidate(script_content, pos, keyword)) {
                if (pos < next_keyword_pos) {
                    next_keyword_pos = pos;
                    found_keyword = keyword;
                }
            }
        }
        if (next_keyword_pos == std::string::npos) break;
        result_stream << createPlaceholder(script_content.substr(last_flush_pos, next_keyword_pos - last_flush_pos));
        size_t construct_start = next_keyword_pos;
        size_t construct_end;
        if (found_keyword == "{{") {
            construct_end = script_content.find("}}", construct_start);
            if (construct_end == std::string::npos) throw std::runtime_error("Unmatched '{{' in script block");
            construct_end += 2;
        } else if (found_keyword == "->" || found_keyword == "&->") {
             construct_end = construct_start + found_keyword.length();
        } else {
            size_t block_start = script_content.find('{', construct_start);
            if (block_start == std::string::npos) throw std::runtime_error("Expected '{' after CHTL JS keyword: " + found_keyword);
            size_t block_end = findEndOfBlock(script_content, block_start + 1, '{', '}');
            if (block_end == std::string::npos) throw std::runtime_error("Unmatched '{' in CHTL JS block for: " + found_keyword);
            construct_end = block_end + 1;
        }
        result_stream << script_content.substr(construct_start, construct_end - construct_start);
        current_pos = construct_end;
        last_flush_pos = current_pos;
    }
    result_stream << createPlaceholder(script_content.substr(last_flush_pos));
    return result_stream.str();
}


void CHTLUnifiedScanner::process() {
    cursor_ = 0;
    while (cursor_ < source_.length()) {
        cursor_ = skipWhitespaceAndComments(source_, cursor_);
        if (cursor_ >= source_.length()) break;

        size_t next_brace = source_.find('{', cursor_);
        if (next_brace == std::string::npos) {
            // If no more blocks, treat the rest as a CHTL fragment (e.g. trailing comments)
            addFragment(source_.substr(cursor_), FragmentType::CHTL);
            break;
        }

        std::string keyword_part = source_.substr(cursor_, next_brace - cursor_);

        // Trim trailing whitespace from keyword part
        size_t last_char = keyword_part.find_last_not_of(" \t\n\r");
        if (last_char != std::string::npos) {
            keyword_part = keyword_part.substr(0, last_char + 1);
        } else {
            keyword_part = "";
        }

        size_t block_end = findEndOfBlock(source_, next_brace + 1, '{', '}');
        if (block_end == std::string::npos) {
            throw std::runtime_error("Unmatched '{' found at position " + std::to_string(next_brace));
        }

        if (keyword_part == "style") {
            std::string block_content = source_.substr(next_brace + 1, block_end - (next_brace + 1));
            addFragment(block_content, FragmentType::CSS);
        } else if (keyword_part == "script") {
            std::string block_content = source_.substr(next_brace + 1, block_end - (next_brace + 1));
            std::string processed_script = scanJsAndChtlJs(block_content);
            addFragment(processed_script, FragmentType::CHTL_JS);
        } else {
            // Everything else is a CHTL fragment, including the keyword and the block
            std::string full_construct = source_.substr(cursor_, (block_end + 1) - cursor_);
            addFragment(full_construct, FragmentType::CHTL);
        }

        cursor_ = block_end + 1;
    }
}

} // namespace CHTL
