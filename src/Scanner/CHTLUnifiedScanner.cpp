#include "CHTLUnifiedScanner.h"
#include <stdexcept>
#include <cctype>
#include <algorithm>
#include <iostream>
#include <map>

namespace CHTL {

// Helper to check if a substring matches at a certain position, with word boundary checks.
bool isKeywordAt(const std::string& haystack, const std::string& needle, size_t pos) {
    if (pos + needle.length() > haystack.length()) {
        return false;
    }
    if (haystack.substr(pos, needle.length()) != needle) {
        return false;
    }
    // For alphabetic keywords, check word boundaries
    if (isalpha(needle[0]) || needle[0] == '[') {
        if (pos > 0 && (isalnum(haystack[pos - 1]) || haystack[pos - 1] == '_')) {
            return false;
        }
        if (pos + needle.length() < haystack.length() && (isalnum(haystack[pos + needle.length()]) || haystack[pos + needle.length()] == '_')) {
            return false;
        }
    }
    return true;
}

// Keywords that start a CHTL JS construct. Ordered by length to prioritize longer matches (e.g., "&->" over "->").
const std::vector<std::string> CHTLJS_KEYWORDS = {
    "ScriptLoader", "iNeverAway", "Delegate", "Animate", "Listen", "Router", "util", "Vir",
    "&->", "->", "{{"
};

enum class BlockType { CHTL_PASSTHROUGH, SCRIPT_BLOCK, RAW_JS_BLOCK, RAW_CSS_BLOCK };
struct BlockDefinition {
    std::string start_sequence;
    BlockType type;
};

const std::vector<BlockDefinition> BLOCK_DEFINITIONS = {
    {"script", BlockType::SCRIPT_BLOCK},
    {"style", BlockType::CHTL_PASSTHROUGH},
    {"[Origin] @JavaScript", BlockType::RAW_JS_BLOCK},
    {"[Origin] @Style", BlockType::RAW_CSS_BLOCK},
    {"[Origin] @Javascript", BlockType::RAW_JS_BLOCK} // Alias
};


// --- CHTLUnifiedScanner Implementation ---

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
    fragments_.push_back({content, type});
}

std::string CHTLUnifiedScanner::createPlaceholder(const std::string& content) {
    if (content.find_first_not_of(" \t\n\r") == std::string::npos) {
        return content;
    }
    std::string placeholder = "__JS_PLACEHOLDER_" + std::to_string(placeholder_id_++) + "__";
    placeholder_map_[placeholder] = content;
    return placeholder;
}

// A robust, state-machine based function to find the end of a block.
size_t findConstructEnd(const std::string& content, size_t start_pos, char open_char, char close_char) {
    enum class FindState { NORMAL, IN_STRING, IN_LINE_COMMENT, IN_BLOCK_COMMENT };
    FindState state = FindState::NORMAL;
    char string_char = '\0';
    int level = 1;

    for (size_t i = start_pos; i < content.length(); ++i) {
        char current_char = content[i];
        char next_char = (i + 1 < content.length()) ? content[i + 1] : '\0';

        switch (state) {
            case FindState::NORMAL:
                if (current_char == '"' || current_char == '\'' || current_char == '`') {
                    state = FindState::IN_STRING;
                    string_char = current_char;
                } else if (current_char == '/' && next_char == '/') {
                    state = FindState::IN_LINE_COMMENT;
                } else if (current_char == '/' && next_char == '*') {
                    state = FindState::IN_BLOCK_COMMENT;
                } else if (current_char == open_char) {
                    level++;
                } else if (current_char == close_char) {
                    level--;
                    if (level == 0) {
                        return i + 1; // Return position *after* the closing character
                    }
                }
                break;

            case FindState::IN_STRING:
                if (current_char == '\\') { i++; }
                else if (current_char == string_char) { state = FindState::NORMAL; }
                break;

            case FindState::IN_LINE_COMMENT:
                if (current_char == '\n') { state = FindState::NORMAL; }
                break;

            case FindState::IN_BLOCK_COMMENT:
                if (current_char == '*' && next_char == '/') {
                    state = FindState::NORMAL;
                    i++; // Skip the '/'
                }
                break;
        }
    }
    return std::string::npos; // Unmatched
}


// This is the new state-machine based implementation.
void CHTLUnifiedScanner::scanJsAndChtlJs(const std::string& script_content) {
    std::stringstream result_stream;
    size_t cursor = 0;
    size_t last_flush_pos = 0;

    enum class ScanState { NORMAL, IN_STRING, IN_LINE_COMMENT, IN_BLOCK_COMMENT };
    ScanState state = ScanState::NORMAL;
    char string_char = '\0';

    while (cursor < script_content.length()) {
        char current_char = script_content[cursor];
        char next_char = (cursor + 1 < script_content.length()) ? script_content[cursor + 1] : '\0';

        switch (state) {
            case ScanState::NORMAL: {
                if (current_char == '"' || current_char == '\'' || current_char == '`') {
                    state = ScanState::IN_STRING;
                    string_char = current_char;
                } else if (current_char == '/' && next_char == '/') {
                    state = ScanState::IN_LINE_COMMENT;
                } else if (current_char == '/' && next_char == '*') {
                    state = ScanState::IN_BLOCK_COMMENT;
                } else {
                    std::string found_keyword;
                    for (const auto& keyword : CHTLJS_KEYWORDS) {
                        if (isKeywordAt(script_content, keyword, cursor)) {
                            found_keyword = keyword;
                            break;
                        }
                    }

                    if (!found_keyword.empty()) {
                        result_stream << createPlaceholder(script_content.substr(last_flush_pos, cursor - last_flush_pos));

                        size_t construct_start = cursor;
                        size_t construct_end;

                        if (found_keyword == "{{") {
                            construct_end = script_content.find("}}", construct_start);
                            if (construct_end == std::string::npos) throw std::runtime_error("Unmatched '{{'");
                            construct_end += 2;
                        } else if (found_keyword == "->" || found_keyword == "&->") {
                            construct_end = construct_start + found_keyword.length();
                        } else {
                            size_t block_start = script_content.find('{', construct_start);
                            if (block_start == std::string::npos) throw std::runtime_error("Expected '{' after CHTL JS keyword: " + found_keyword);
                            construct_end = findConstructEnd(script_content, block_start + 1, '{', '}');
                            if (construct_end == std::string::npos) throw std::runtime_error("Unmatched '{' in CHTL JS block for: " + found_keyword);
                        }

                        result_stream << script_content.substr(construct_start, construct_end - construct_start);
                        cursor = construct_end -1;
                        last_flush_pos = construct_end;
                    }
                }
                break;
            }
            case ScanState::IN_STRING: {
                if (current_char == '\\') { cursor++; }
                else if (current_char == string_char) { state = ScanState::NORMAL; }
                break;
            }
            case ScanState::IN_LINE_COMMENT: {
                if (current_char == '\n') { state = ScanState::NORMAL; }
                break;
            }
            case ScanState::IN_BLOCK_COMMENT: {
                if (current_char == '*' && next_char == '/') {
                    state = ScanState::NORMAL;
                    cursor++;
                }
                break;
            }
        }
        cursor++;
    }

    result_stream << createPlaceholder(script_content.substr(last_flush_pos));
    addFragment(result_stream.str(), FragmentType::CHTL_JS);
}

void CHTLUnifiedScanner::process() {
    cursor_ = 0;
    size_t last_flush_pos = 0;

    while (cursor_ < source_.length()) {
        size_t next_block_pos = std::string::npos;
        const BlockDefinition* found_block_def = nullptr;

        for (const auto& def : BLOCK_DEFINITIONS) {
            size_t pos = source_.find(def.start_sequence, cursor_);
            if (pos != std::string::npos && isKeywordAt(source_, def.start_sequence, pos)) {
                if (pos < next_block_pos) {
                    next_block_pos = pos;
                    found_block_def = &def;
                }
            }
        }

        if (next_block_pos == std::string::npos) { break; }

        addFragment(source_.substr(last_flush_pos, next_block_pos - last_flush_pos), FragmentType::CHTL);

        size_t block_header_end = next_block_pos + found_block_def->start_sequence.length();
        size_t block_start = source_.find('{', block_header_end);
        if (block_start == std::string::npos) { throw std::runtime_error("Expected '{' for block: " + found_block_def->start_sequence); }

        size_t block_end = findConstructEnd(source_, block_start + 1, '{', '}');
        if (block_end == std::string::npos) { throw std::runtime_error("Unmatched '{' in block: " + found_block_def->start_sequence); }

        switch (found_block_def->type) {
            case BlockType::CHTL_PASSTHROUGH: {
                addFragment(source_.substr(next_block_pos, block_end - next_block_pos), FragmentType::CHTL);
                break;
            }
            case BlockType::SCRIPT_BLOCK: {
                scanJsAndChtlJs(source_.substr(block_start + 1, block_end - block_start - 2));
                break;
            }
            case BlockType::RAW_JS_BLOCK: {
                addFragment(source_.substr(block_start + 1, block_end - block_start - 2), FragmentType::JS);
                break;
            }
            case BlockType::RAW_CSS_BLOCK: {
                addFragment(source_.substr(block_start + 1, block_end - block_start - 2), FragmentType::CSS);
                break;
            }
        }

        cursor_ = block_end;
        last_flush_pos = cursor_;
    }

    if (last_flush_pos < source_.length()) {
        addFragment(source_.substr(last_flush_pos), FragmentType::CHTL);
    }
}

} // namespace CHTL
