#include "CHTLUnifiedScanner.h"
#include <stdexcept>
#include <cctype>
#include <algorithm>
#include <iostream>
#include <sstream>

namespace CHTL {

// --- Keyword Definitions ---
const std::vector<std::string> CHTLJS_KEYWORDS = {
    "{{", "listen", "delegate", "animate", "vir", "router", "&->", "->",
    "ScriptLoader", "util", "iNeverAway", "printMylove"
};
const std::vector<std::string> BRACKET_KEYWORDS = {
    "[Template]", "[Custom]", "[Origin]", "[Import]", "[Namespace]", "[Configuration]", "[Info]"
};

// --- Recognizer List ---
const std::vector<CHTLUnifiedScanner::BlockRecognizer> CHTLUnifiedScanner::RECOGNIZERS = {
    &CHTLUnifiedScanner::recognizeStyleBlock,
    &CHTLUnifiedScanner::recognizeScriptBlock,
    &CHTLUnifiedScanner::recognizeBracketBlock,
};


// --- Constructor and Public Methods ---
CHTLUnifiedScanner::CHTLUnifiedScanner(const std::string& source)
    : source_(source) {}

const std::map<std::string, std::string>& CHTLUnifiedScanner::getPlaceholderMap() const {
    return placeholder_map_;
}

std::vector<CodeFragment> CHTLUnifiedScanner::scan() {
    process();
    return fragments_;
}

// --- Core Processing Loop ---
void CHTLUnifiedScanner::process() {
    size_t last_flush_pos = 0;
    cursor_ = 0;
    brace_level_ = 0;
    state_ = ParsingState::DEFAULT;

    while (cursor_ < source_.length()) {
        char current_char = source_[cursor_];
        char next_char = (cursor_ + 1 < source_.length()) ? source_[cursor_ + 1] : '\0';

        // First, check if we are in a special state (string or comment)
        if (state_ != ParsingState::DEFAULT) {
            switch (state_) {
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
                default: break; // Should not happen
            }
            cursor_++;
            continue;
        }

        // If we are in the DEFAULT state, try to run recognizers.
        bool recognized = false;
        for (const auto& recognizer : RECOGNIZERS) {
            if ((this->*recognizer)(last_flush_pos)) {
                recognized = true;
                break; // The recognizer advanced the cursor.
            }
        }

        if (recognized) {
            continue; // Start the next loop iteration from the new cursor position.
        }

        // If no recognizer matched, process the character normally.
        if (current_char == '"' || current_char == '\'' || current_char == '`') {
            state_ = ParsingState::IN_STRING;
            string_delimiter_ = current_char;
        } else if (current_char == '/' && next_char == '/') {
            state_ = ParsingState::IN_SINGLE_LINE_COMMENT;
            cursor_++;
        } else if (current_char == '/' && next_char == '*') {
            state_ = ParsingState::IN_MULTI_LINE_COMMENT;
            cursor_++;
        } else if (current_char == '{') {
            brace_level_++;
        } else if (current_char == '}') {
            if (brace_level_ > 0) brace_level_--;
        }

        cursor_++;
    }

    // Flush any remaining CHTL content at the end of the file.
    if (last_flush_pos < source_.length()) {
        addFragment(source_.substr(last_flush_pos), FragmentType::CHTL, last_flush_pos);
    }
}


// --- Recognizer Implementations ---

bool CHTLUnifiedScanner::recognizeStyleBlock(size_t& last_flush_pos) {
    if (!isKeywordAt(cursor_, "style")) return false;

    size_t temp_cursor = cursor_ + 5;
    while(temp_cursor < source_.length() && isspace(source_[temp_cursor])) {
        temp_cursor++;
    }
    if (temp_cursor == source_.length() || source_[temp_cursor] != '{') {
        return false;
    }

    addFragment(source_.substr(last_flush_pos, cursor_ - last_flush_pos), FragmentType::CHTL, last_flush_pos);

    size_t block_start = temp_cursor;
    size_t block_end = findEndOfBlock(block_start + 1, '{', '}');
    if (block_end == std::string::npos) throw std::runtime_error("Unmatched '{' in style block.");

    std::string content = source_.substr(block_start + 1, block_end - (block_start + 2));
    addFragment(content, FragmentType::CHTL_STYLE_BLOCK, last_flush_pos);

    cursor_ = block_end;
    last_flush_pos = cursor_;
    return true;
}

bool CHTLUnifiedScanner::recognizeScriptBlock(size_t& last_flush_pos) {
    if (!isKeywordAt(cursor_, "script")) return false;

    size_t temp_cursor = cursor_ + 6;
    while(temp_cursor < source_.length() && isspace(source_[temp_cursor])) {
        temp_cursor++;
    }
    if (temp_cursor == source_.length() || source_[temp_cursor] != '{') {
        return false;
    }

    addFragment(source_.substr(last_flush_pos, cursor_ - last_flush_pos), FragmentType::CHTL, last_flush_pos);

    size_t block_start = temp_cursor;
    size_t block_end = findEndOfBlock(block_start + 1, '{', '}');
    if (block_end == std::string::npos) throw std::runtime_error("Unmatched '{' in script block.");

    std::string content = source_.substr(block_start + 1, block_end - (block_start + 2));
    FragmentType type = (brace_level_ > 0) ? FragmentType::CHTL_SCRIPT_BLOCK : FragmentType::CHTL_JS;

    std::string processed_content = scanJsAndChtlJs(content);
    addFragment(processed_content, type, last_flush_pos);

    cursor_ = block_end;
    last_flush_pos = cursor_;
    return true;
}

bool CHTLUnifiedScanner::recognizeBracketBlock(size_t& last_flush_pos) {
    std::string found_keyword;
    for (const auto& keyword : BRACKET_KEYWORDS) {
        if (isKeywordAt(cursor_, keyword)) {
            found_keyword = keyword;
            break;
        }
    }
    if (found_keyword.empty()) return false;

    addFragment(source_.substr(last_flush_pos, cursor_ - last_flush_pos), FragmentType::CHTL, last_flush_pos);

    size_t search_pos = cursor_ + found_keyword.length();
    size_t construct_end = std::string::npos;

    size_t brace_pos = source_.find('{', search_pos);
    size_t semi_pos = source_.find(';', search_pos);

    if (brace_pos != std::string::npos && (brace_pos < semi_pos || semi_pos == std::string::npos)) {
        construct_end = findEndOfBlock(brace_pos + 1, '{', '}');
    } else if (semi_pos != std::string::npos) {
        construct_end = semi_pos + 1;
    } else {
        construct_end = source_.find('\n', search_pos);
        if (construct_end != std::string::npos) construct_end++;
        else construct_end = source_.length();
    }

    std::string content = source_.substr(cursor_, construct_end - cursor_);
    addFragment(content, FragmentType::CHTL, last_flush_pos);

    cursor_ = construct_end;
    last_flush_pos = cursor_;
    return true;
}


// --- Helper Functions ---

void CHTLUnifiedScanner::addFragment(const std::string& content, FragmentType type, size_t& last_flush_pos) {
    if (!content.empty()) {
        fragments_.push_back({content, type});
    }
    last_flush_pos = cursor_;
}

std::string CHTLUnifiedScanner::createPlaceholder(const std::string& content) {
    // We create placeholders even for whitespace, but not for genuinely empty strings.
    if (content.empty()) {
        return "";
    }
    std::string placeholder = "__CHTL_JS_PLACEHOLDER_" + std::to_string(placeholder_id_++) + "__";
    placeholder_map_[placeholder] = content;
    return placeholder;
}

size_t CHTLUnifiedScanner::findEndOfBlock(size_t start_pos, char open_brace, char close_brace) {
    int brace_level = 1;
    ParsingState block_state = ParsingState::DEFAULT;
    char string_delimiter = '\0';

    for (size_t i = start_pos; i < source_.length(); ++i) {
        char current_char = source_[i];
        char next_char = (i + 1 < source_.length()) ? source_[i + 1] : '\0';

        switch (block_state) {
            case ParsingState::DEFAULT:
                if (current_char == '"' || current_char == '\'' || current_char == '`') {
                    block_state = ParsingState::IN_STRING;
                    string_delimiter = current_char;
                } else if (current_char == '/' && next_char == '/') {
                    block_state = ParsingState::IN_SINGLE_LINE_COMMENT;
                    i++;
                } else if (current_char == '/' && next_char == '*') {
                    block_state = ParsingState::IN_MULTI_LINE_COMMENT;
                    i++;
                } else if (current_char == open_brace) {
                    brace_level++;
                } else if (current_char == close_brace) {
                    brace_level--;
                    if (brace_level == 0) {
                        return i + 1;
                    }
                }
                break;
            case ParsingState::IN_STRING:
                if (current_char == '\\') {
                    i++;
                } else if (current_char == string_delimiter) {
                    block_state = ParsingState::DEFAULT;
                }
                break;
            case ParsingState::IN_SINGLE_LINE_COMMENT:
                if (current_char == '\n') {
                    block_state = ParsingState::DEFAULT;
                }
                break;
            case ParsingState::IN_MULTI_LINE_COMMENT:
                if (current_char == '*' && next_char == '/') {
                    block_state = ParsingState::DEFAULT;
                    i++;
                }
                break;
        }
    }
    return std::string::npos;
}

bool CHTLUnifiedScanner::isKeywordAt(size_t pos, const std::string& keyword) {
    if (pos + keyword.length() > source_.length() || source_.substr(pos, keyword.length()) != keyword) {
        return false;
    }
    if (pos > 0 && isalnum(source_[pos - 1])) {
        return false;
    }
    if (pos + keyword.length() < source_.length() && isalnum(source_[pos + keyword.length()])) {
        return false;
    }
    return true;
}

std::string CHTLUnifiedScanner::scanJsAndChtlJs(const std::string& script_content) {
    std::stringstream result;
    size_t current_pos = 0;
    size_t last_flush_pos = 0;

    while (current_pos < script_content.length()) {
        size_t next_keyword_pos = std::string::npos;
        std::string found_keyword;

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

        result << createPlaceholder(script_content.substr(last_flush_pos, next_keyword_pos - last_flush_pos));

        size_t construct_start = next_keyword_pos;
        size_t construct_end;

        if (found_keyword == "{{") {
            size_t end_pos = script_content.find("}}", construct_start + 2);
            if (end_pos == std::string::npos) throw std::runtime_error("Unmatched '{{' in script block.");
            construct_end = end_pos + 2;
        } else if (found_keyword == "->" || found_keyword == "&->") {
             construct_end = construct_start + found_keyword.length();
        } else if (found_keyword == "util") {
            size_t end_pos = script_content.find(';', construct_start);
            if (end_pos == std::string::npos) throw std::runtime_error("Unmatched 'util' statement; expected a semicolon ';'.");
            construct_end = end_pos + 1;
        } else {
            size_t block_start = script_content.find('{', construct_start);
            if (block_start == std::string::npos) throw std::runtime_error("Expected '{' after CHTL JS keyword: " + found_keyword);

            int brace_level = 1;
            size_t end_pos = block_start + 1;
            while(end_pos < script_content.length() && brace_level > 0) {
                if (script_content[end_pos] == '{') brace_level++;
                if (script_content[end_pos] == '}') brace_level--;
                end_pos++;
            }
            if (brace_level > 0) throw std::runtime_error("Unmatched '{' in CHTL JS block for: " + found_keyword);
            construct_end = end_pos;
        }

        result << script_content.substr(construct_start, construct_end - construct_start);
        current_pos = construct_end;
        last_flush_pos = current_pos;
    }

    result << createPlaceholder(script_content.substr(last_flush_pos));

    return result.str();
}

}
