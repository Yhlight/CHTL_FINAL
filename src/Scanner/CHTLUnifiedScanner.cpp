#include "CHTLUnifiedScanner.h"
#include <stdexcept>
#include <cctype>
#include <algorithm>
#include <iostream>

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
    "{{", "listen", "delegate", "animate", "vir", "router", "&->", "->"
};

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
    if (!content.empty()) {
        fragments_.push_back({content, type});
    }
}

std::string CHTLUnifiedScanner::createPlaceholder(const std::string& content) {
    if (content.empty() || content.find_first_not_of(" \t\n\r") == std::string::npos) {
        return "";
    }
    std::string placeholder = "__CHTL_JS_PLACEHOLDER_" + std::to_string(placeholder_id_++) + "__";
    placeholder_map_[placeholder] = content;
    return placeholder;
}

size_t CHTLUnifiedScanner::findEndOfBlock(size_t start_pos, char open_brace, char close_brace) {
    int brace_level = 1; // Start at 1 because the scan starts after the opening brace.
    bool in_string = false;
    char string_char = '\0';

    for (size_t i = start_pos; i < source_.length(); ++i) {
        char current_char = source_[i];
        char next_char = (i + 1 < source_.length()) ? source_[i + 1] : '\0';

        if (in_string) {
            if (current_char == '\\') {
                i++; // Skip escaped character
            } else if (current_char == string_char) {
                in_string = false;
            }
            continue;
        }

        if (current_char == '"' || current_char == '\'' || current_char == '`') {
            in_string = true;
            string_char = current_char;
            continue;
        }

        if (current_char == '/' && next_char == '/') { // Skip single-line comment
            i = source_.find('\n', i);
            if (i == std::string::npos) return std::string::npos;
            continue;
        }

        if (current_char == '/' && next_char == '*') { // Skip multi-line comment
            i = source_.find("*/", i + 2);
            if (i == std::string::npos) return std::string::npos;
            i++; // move past the '/'
            continue;
        }

        if (current_char == open_brace) {
            brace_level++;
        } else if (current_char == close_brace) {
            brace_level--;
            if (brace_level == 0) {
                return i + 1; // Return position *after* the closing brace
            }
        }
    }

    return std::string::npos; // Unbalanced braces
}

void CHTLUnifiedScanner::scanJsAndChtlJs(const std::string& script_content) {
    size_t current_pos = 0;
    size_t last_flush_pos = 0;

    while (current_pos < script_content.length()) {
        size_t next_keyword_pos = std::string::npos;
        std::string found_keyword;

        // Find the earliest CHTL JS keyword
        for (const auto& keyword : CHTLJS_KEYWORDS) {
            size_t pos = script_content.find(keyword, current_pos);
            if (pos != std::string::npos && pos < next_keyword_pos) {
                // Basic word boundary check for alphabetic keywords
                if (isalpha(keyword[0])) {
                    if (pos > 0 && isalnum(script_content[pos - 1])) continue;
                    if (pos + keyword.length() < script_content.length() && isalnum(script_content[pos + keyword.length()])) continue;
                }
                next_keyword_pos = pos;
                found_keyword = keyword;
            }
        }

        if (next_keyword_pos == std::string::npos) {
            // No more keywords, the rest is pure JS
            break;
        }

        // Flush the JS before the keyword as a JS fragment
        addFragment(script_content.substr(last_flush_pos, next_keyword_pos - last_flush_pos), FragmentType::JS);

        // Now, extract the CHTL JS construct
        size_t construct_start = next_keyword_pos;
        size_t construct_end;

        if (found_keyword == "{{") {
            construct_end = script_content.find("}}", construct_start);
            if (construct_end == std::string::npos) throw std::runtime_error("Unmatched '{{'");
            construct_end += 2; // include the "}}"
        } else if (found_keyword == "->" || found_keyword == "&->") {
             construct_end = construct_start + found_keyword.length();
        } else { // Assumes block-based constructs like Listen, Animate, etc.
            size_t block_start = script_content.find('{', construct_start);
            if (block_start == std::string::npos) throw std::runtime_error("Expected '{' after CHTL JS keyword: " + found_keyword);

            int brace_level = 1;
            size_t temp_cursor = block_start + 1;
            while (temp_cursor < script_content.length()) {
                if (script_content[temp_cursor] == '{') brace_level++;
                if (script_content[temp_cursor] == '}') brace_level--;
                if (brace_level == 0) break;
                temp_cursor++;
            }
            if (brace_level != 0) throw std::runtime_error("Unmatched '{' in CHTL JS block for: " + found_keyword);
            construct_end = temp_cursor + 1;
        }

        std::string chtl_js_construct = script_content.substr(construct_start, construct_end - construct_start);
        addFragment(chtl_js_construct, FragmentType::CHTL_JS);

        current_pos = construct_end;
        last_flush_pos = current_pos;
    }

    // Flush any remaining JS at the end
    addFragment(script_content.substr(last_flush_pos), FragmentType::JS);
}

bool CHTLUnifiedScanner::isKeywordAt(size_t pos, const std::string& keyword) {
    // Check if keyword exists at position
    if (pos + keyword.length() > source_.length() || source_.substr(pos, keyword.length()) != keyword) {
        return false;
    }
    // Check for word boundary after the keyword
    if (pos + keyword.length() < source_.length() && isalnum(source_[pos + keyword.length()])) {
        return false;
    }
    // Check for word boundary before the keyword
    if (pos > 0 && isalnum(source_[pos - 1])) {
        return false;
    }

    // Find the next non-whitespace character
    size_t temp_cursor = pos + keyword.length();
    while (temp_cursor < source_.length() && isspace(source_[temp_cursor])) {
        temp_cursor++;
    }

    // Check if it's an opening brace
    return temp_cursor < source_.length() && source_[temp_cursor] == '{';
}


void CHTLUnifiedScanner::process() {
    size_t last_flush_pos = 0;
    cursor_ = 0;

    while (cursor_ < source_.length()) {
        if (isKeywordAt(cursor_, "style")) {
            // Flush preceding CHTL
            addFragment(source_.substr(last_flush_pos, cursor_ - last_flush_pos), FragmentType::CHTL);

            // Find the opening brace
            size_t block_start = source_.find('{', cursor_);
            cursor_ = block_start + 1;

            size_t block_end = findEndOfBlock(cursor_, '{', '}');
            if (block_end == std::string::npos) {
                throw std::runtime_error("Unmatched '{' in style block.");
            }

            std::string block_content = source_.substr(cursor_, block_end - cursor_ -1);
            addFragment(block_content, FragmentType::CSS);

            cursor_ = block_end;
            last_flush_pos = cursor_;

        } else if (isKeywordAt(cursor_, "script")) {
             // Flush preceding CHTL
            addFragment(source_.substr(last_flush_pos, cursor_ - last_flush_pos), FragmentType::CHTL);

            // Find the opening brace
            size_t block_start = source_.find('{', cursor_);
            cursor_ = block_start + 1;

            size_t block_end = findEndOfBlock(cursor_, '{', '}');
            if (block_end == std::string::npos) {
                throw std::runtime_error("Unmatched '{' in script block.");
            }

            std::string block_content = source_.substr(cursor_, block_end - cursor_ -1);
            scanJsAndChtlJs(block_content);

            cursor_ = block_end;
            last_flush_pos = cursor_;
        } else {
            cursor_++;
        }
    }

    // Add any remaining CHTL content
    if (last_flush_pos < source_.length()) {
        addFragment(source_.substr(last_flush_pos), FragmentType::CHTL);
    }
}


} // namespace CHTL
