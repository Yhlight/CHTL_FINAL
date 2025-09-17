#include "CHTLUnifiedScanner.h"
#include <stdexcept>
#include <cctype>
#include <algorithm>
#include <iostream>

namespace CHTL {

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

// This new static helper contains the robust state machine for finding a closing brace.
// It can be used on any string, which is crucial for reusability.
static size_t find_block_end_robustly(const std::string& text, size_t start_pos, char open_brace, char close_brace) {
    int brace_level = 1;
    CHTLUnifiedScanner::ParsingState state = CHTLUnifiedScanner::ParsingState::DEFAULT;
    char string_delimiter = '\0';

    for (size_t i = start_pos; i < text.length(); ++i) {
        char current_char = text[i];
        char next_char = (i + 1 < text.length()) ? text[i + 1] : '\0';

        switch (state) {
            case CHTLUnifiedScanner::ParsingState::DEFAULT:
                if (current_char == '"' || current_char == '\'' || current_char == '`') {
                    state = CHTLUnifiedScanner::ParsingState::IN_STRING;
                    string_delimiter = current_char;
                } else if (current_char == '/' && next_char == '/') {
                    state = CHTLUnifiedScanner::ParsingState::IN_SINGLE_LINE_COMMENT;
                    i++;
                } else if (current_char == '/' && next_char == '*') {
                    state = CHTLUnifiedScanner::ParsingState::IN_MULTI_LINE_COMMENT;
                    i++;
                } else if (current_char == open_brace) {
                    brace_level++;
                } else if (current_char == close_brace) {
                    brace_level--;
                    if (brace_level == 0) {
                        return i + 1; // Return position *after* the closing brace
                    }
                }
                break;

            case CHTLUnifiedScanner::ParsingState::IN_STRING:
                if (current_char == '\\') {
                    i++;
                } else if (current_char == string_delimiter) {
                    state = CHTLUnifiedScanner::ParsingState::DEFAULT;
                }
                break;

            case CHTLUnifiedScanner::ParsingState::IN_SINGLE_LINE_COMMENT:
                if (current_char == '\n') {
                    state = CHTLUnifiedScanner::ParsingState::DEFAULT;
                }
                break;

            case CHTLUnifiedScanner::ParsingState::IN_MULTI_LINE_COMMENT:
                if (current_char == '*' && next_char == '/') {
                    state = CHTLUnifiedScanner::ParsingState::DEFAULT;
                    i++;
                }
                break;
        }
    }
    return std::string::npos; // Unbalanced braces
}

size_t CHTLUnifiedScanner::findEndOfBlock(size_t start_pos, char open_brace, char close_brace) {
    // The member function now calls the static helper on the source string.
    return find_block_end_robustly(source_, start_pos, open_brace, close_brace);
}

std::string CHTLUnifiedScanner::scanJsAndChtlJs(const std::string& script_content) {
    std::stringstream result;
    size_t current_pos = 0;
    size_t last_flush_pos = 0;

    while (current_pos < script_content.length()) {
        size_t next_keyword_pos = std::string::npos;
        std::string found_keyword;

        // Find the earliest CHTL JS keyword
        for (const auto& keyword : CHTLJS_KEYWORDS) {
            size_t pos = script_content.find(keyword, current_pos);
            if (pos != std::string::npos) {
                if (isalpha(keyword[0])) {
                    if (pos > 0 && isalnum(script_content[pos - 1])) continue;
                    if (pos + keyword.length() < script_content.length() && isalnum(script_content[pos + keyword.length()])) continue;
                }
                if (pos < next_keyword_pos) {
                    next_keyword_pos = pos;
                    found_keyword = keyword;
                }
            }
        }

        if (next_keyword_pos == std::string::npos) {
            break;
        }

        std::string js_block = script_content.substr(last_flush_pos, next_keyword_pos - last_flush_pos);
        if (!js_block.empty()) {
            result << createPlaceholder(js_block);
        }

        size_t construct_start = next_keyword_pos;
        size_t construct_end;

        if (found_keyword == "{{") {
            size_t end_pos = script_content.find("}}", construct_start + 2);
            if (end_pos == std::string::npos) throw std::runtime_error("Unmatched '{{' in script block.");
            construct_end = end_pos + 2;
        } else if (found_keyword == "->" || found_keyword == "&->") {
             construct_end = construct_start + found_keyword.length();
        } else {
            size_t block_start = script_content.find('{', construct_start);
            if (block_start == std::string::npos) throw std::runtime_error("Expected '{' after CHTL JS keyword: " + found_keyword);

            // Using the robust helper function now
            construct_end = find_block_end_robustly(script_content, block_start + 1, '{', '}');
            if (construct_end == std::string::npos) throw std::runtime_error("Unmatched '{' in CHTL JS block for: " + found_keyword);
        }

        std::string chtl_js_construct = script_content.substr(construct_start, construct_end - construct_start);
        result << chtl_js_construct;

        current_pos = construct_end;
        last_flush_pos = current_pos;
    }

    std::string final_js_block = script_content.substr(last_flush_pos);
    if (!final_js_block.empty()) {
        result << createPlaceholder(final_js_block);
    }

    return result.str();
}

bool CHTLUnifiedScanner::isKeywordAt(size_t pos, const std::string& keyword) {
    if (pos + keyword.length() > source_.length() || source_.substr(pos, keyword.length()) != keyword) {
        return false;
    }
    if (pos + keyword.length() < source_.length() && isalnum(source_[pos + keyword.length()])) {
        return false;
    }
    if (pos > 0 && isalnum(source_[pos - 1])) {
        return false;
    }
    size_t temp_cursor = pos + keyword.length();
    while (temp_cursor < source_.length() && isspace(source_[temp_cursor])) {
        temp_cursor++;
    }
    return temp_cursor < source_.length() && source_[temp_cursor] == '{';
}


void CHTLUnifiedScanner::process() {
    size_t last_flush_pos = 0;
    cursor_ = 0;
    state_ = ParsingState::DEFAULT;

    while (cursor_ < source_.length()) {
        char current_char = source_[cursor_];
        char next_char = (cursor_ + 1 < source_.length()) ? source_[cursor_ + 1] : '\0';

        bool processed = false;

        // The main state machine for scanning the top-level source
        switch (state_) {
            case ParsingState::DEFAULT:
                if (current_char == '"' || current_char == '\'' || current_char == '`') {
                    state_ = ParsingState::IN_STRING;
                    string_delimiter_ = current_char;
                } else if (current_char == '/' && next_char == '/') {
                    state_ = ParsingState::IN_SINGLE_LINE_COMMENT;
                    cursor_++; // Skip the second '/'
                } else if (current_char == '/' && next_char == '*') {
                    state_ = ParsingState::IN_MULTI_LINE_COMMENT;
                    cursor_++; // Skip the '*'
                } else if (isKeywordAt(cursor_, "style")) {
                    addFragment(source_.substr(last_flush_pos, cursor_ - last_flush_pos), FragmentType::CHTL);

                    size_t block_start = source_.find('{', cursor_);
                    if (block_start == std::string::npos) throw std::runtime_error("Expected '{' after style keyword.");

                    size_t block_end = findEndOfBlock(block_start + 1, '{', '}');
                    if (block_end == std::string::npos) throw std::runtime_error("Unmatched '{' in style block.");

                    std::string block_content = source_.substr(block_start + 1, block_end - (block_start + 2));
                    addFragment(block_content, FragmentType::CSS);

                    cursor_ = block_end -1; // -1 because cursor will be incremented at the end of the loop
                    last_flush_pos = block_end;
                    processed = true;
                } else if (isKeywordAt(cursor_, "script")) {
                    addFragment(source_.substr(last_flush_pos, cursor_ - last_flush_pos), FragmentType::CHTL);

                    size_t block_start = source_.find('{', cursor_);
                    if (block_start == std::string::npos) throw std::runtime_error("Expected '{' after script keyword.");

                    size_t block_end = findEndOfBlock(block_start + 1, '{', '}');
                    if (block_end == std::string::npos) throw std::runtime_error("Unmatched '{' in script block.");

                    std::string block_content = source_.substr(block_start + 1, block_end - (block_start + 2));
                    std::string chtl_js_with_placeholders = scanJsAndChtlJs(block_content);
                    addFragment(chtl_js_with_placeholders, FragmentType::CHTL_JS);

                    cursor_ = block_end - 1; // -1 because cursor will be incremented at the end of the loop
                    last_flush_pos = block_end;
                    processed = true;
                }
                break;

            case ParsingState::IN_STRING:
                if (current_char == '\\') {
                    cursor_++; // Skip escaped character
                } else if (current_char == string_delimiter_) {
                    state_ = ParsingState::DEFAULT;
                }
                break;

            case ParsingState::IN_SINGLE_LINE_COMMENT:
                if (current_char == '\n') {
                    state_ = ParsingState::DEFAULT;
                }
                break;

            case ParsingState::IN_MULTI_LINE_COMMENT:
                if (current_char == '*' && next_char == '/') {
                    state_ = ParsingState::DEFAULT;
                    cursor_++; // Skip the '/'
                }
                break;
        }

        if (!processed) {
            cursor_++;
        }
    }

    if (last_flush_pos < source_.length()) {
        addFragment(source_.substr(last_flush_pos), FragmentType::CHTL);
    }
}


} // namespace CHTL
