#include "CHTLUnifiedScanner.h"
#include <stdexcept>
#include <iostream>
#include <vector>
#include <string>
#include <unordered_set>
#include <cctype>

namespace CHTL {

// Forward declarations of helper methods
static std::vector<CodeFragment> processScriptBlock(const std::string& content, int line_start);
static std::vector<CodeFragment> processStyleBlock(const std::string& content, int line_start);

CHTLUnifiedScanner::CHTLUnifiedScanner(const std::string& source)
    : source(source) {}

std::vector<CodeFragment> CHTLUnifiedScanner::scan() {
    std::vector<CodeFragment> fragments;
    std::string buffer;
    int line_start = 1; // Tracks the start line of the current buffer
    int brace_level = 0;

    auto is_keyword_at = [&](const std::string& keyword, int pos) {
        if (pos + keyword.length() > source.length() || source.substr(pos, keyword.length()) != keyword) return false;
        if (pos > 0 && isalnum(source[pos - 1])) return false;
        int end_pos = pos + keyword.length();
        if (end_pos < source.length() && isalnum(source[end_pos])) return false;
        return true;
    };

    for (current = 0; current < source.length(); ++current) {
        if (source[current] == '/' && current + 1 < source.length()) {
            if (source[current + 1] == '/') {
                while (current < source.length() && source[current] != '\n') buffer += source[current++];
                if (current < source.length()) buffer += source[current];
                if (source[current] == '\n') line++;
                continue;
            }
            if (source[current + 1] == '*') {
                 int comment_start = current;
                 current += 2;
                 while(current + 1 < source.length() && (source[current] != '*' || source[current+1] != '/')) {
                     if (source[current] == '\n') line++;
                     current++;
                 }
                 current++;
                 buffer += source.substr(comment_start, current - comment_start + 1);
                 continue;
            }
        }

        if (source[current] == '\n') line++;

        if (brace_level == 0) {
            bool is_style = is_keyword_at("style", current);
            bool is_script = is_keyword_at("script", current);

            int lookahead = current + (is_style ? 5 : (is_script ? 6 : 0));
            int temp_line = line;
            while (lookahead < source.length() && isspace(source[lookahead])) {
                if (source[lookahead] == '\n') temp_line++;
                lookahead++;
            }

            if ((is_style || is_script) && lookahead < source.length() && source[lookahead] == '{') {
                if (!buffer.empty()) {
                    fragments.push_back({FragmentType::CHTL, buffer, line_start});
                    buffer.clear();
                }

                line = temp_line;
                int content_start = lookahead + 1;
                int block_brace_level = 1;
                int content_end = content_start;
                while (content_end < source.length()) {
                    if (source[content_end] == '\n') line++;
                    else if (source[content_end] == '{') block_brace_level++;
                    else if (source[content_end] == '}') block_brace_level--;
                    if (block_brace_level == 0) break;
                    content_end++;
                }

                if (block_brace_level != 0) {
                     buffer += source.substr(current, source.length() - current);
                     current = source.length();
                     continue;
                }

                std::string block_content = source.substr(content_start, content_end - content_start);
                int content_line_start = temp_line;

                std::vector<CodeFragment> block_fragments;
                if (is_style) {
                    block_fragments = processStyleBlock(block_content, content_line_start);
                } else {
                    block_fragments = processScriptBlock(block_content, content_line_start);
                }
                fragments.insert(fragments.end(), block_fragments.begin(), block_fragments.end());

                current = content_end;
                line_start = line;
                continue;
            }
        }

        if (source[current] == '{') brace_level++;
        else if (source[current] == '}') brace_level--;

        buffer += source[current];
    }

    if (!buffer.empty()) {
        fragments.push_back({FragmentType::CHTL, buffer, line_start});
    }

    return fragments;
}

static std::vector<CodeFragment> processStyleBlock(const std::string& content, int line_start) {
    std::vector<CodeFragment> fragments;
    std::string buffer;
    int current_pos = 0;
    int current_line = line_start;

    while (current_pos < content.length()) {
        char current_char = content[current_pos];
        if (current_char == '\n') current_line++;

        // Look for CHTL-in-CSS syntax triggers
        // This is a simplified first pass, focusing on @-rules and #-comments
        if (current_char == '@' || current_char == '#') {
             if (!buffer.empty()) {
                fragments.push_back({FragmentType::CSS, buffer, current_line});
                buffer.clear();
            }

            int chtl_start = current_pos;
            // Capture until the end of the statement (semicolon)
            while (current_pos < content.length() && content[current_pos] != ';') {
                 if (content[current_pos] == '\n') current_line++;
                 current_pos++;
            }
             if (current_pos < content.length()) { // include the semicolon
                current_pos++;
            }

            fragments.push_back({FragmentType::CHTL, content.substr(chtl_start, current_pos - chtl_start), current_line});
            continue;
        }

        buffer += current_char;
        current_pos++;
    }

    if (!buffer.empty()) {
        fragments.push_back({FragmentType::CSS, buffer, current_line});
    }

    if (fragments.empty() && !content.empty()) {
        return {{FragmentType::CSS, content, line_start}};
    }

    return fragments;
}

enum class ScriptState {
    NORMAL,
    IN_SINGLE_QUOTE_STRING,
    IN_DOUBLE_QUOTE_STRING,
    IN_TEMPLATE_LITERAL
};

static std::vector<CodeFragment> processScriptBlock(const std::string& content, int line_start) {
    std::vector<CodeFragment> fragments;
    std::string buffer;
    int current_pos = 0;
    int current_line = line_start;
    ScriptState state = ScriptState::NORMAL;
    char prev_char = '\0';

    const std::unordered_set<std::string> chtl_js_keywords = {
        "Listen", "Delegate", "Animate", "Vir", "Router", "ScriptLoader", "iNeverAway", "printMylove", "util"
    };

    while (current_pos < content.length()) {
        char current_char = content[current_pos];
        if (content[current_pos] == '\n') current_line++;

        // State transition logic
        if (state == ScriptState::NORMAL) {
            if (current_char == '\'' && prev_char != '\\') state = ScriptState::IN_SINGLE_QUOTE_STRING;
            else if (current_char == '"' && prev_char != '\\') state = ScriptState::IN_DOUBLE_QUOTE_STRING;
            else if (current_char == '`' && prev_char != '\\') state = ScriptState::IN_TEMPLATE_LITERAL;
        } else if (state == ScriptState::IN_SINGLE_QUOTE_STRING) {
            if (current_char == '\'' && prev_char != '\\') state = ScriptState::NORMAL;
        } else if (state == ScriptState::IN_DOUBLE_QUOTE_STRING) {
            if (current_char == '"' && prev_char != '\\') state = ScriptState::NORMAL;
        } else if (state == ScriptState::IN_TEMPLATE_LITERAL) {
            if (current_char == '`' && prev_char != '\\') state = ScriptState::NORMAL;
        }

        // CHTL JS detection logic (only runs in NORMAL state)
        if (state == ScriptState::NORMAL) {
            if (content.substr(current_pos, 2) == "{{") {
                if (!buffer.empty()) fragments.push_back({FragmentType::JS, buffer, current_line});
                buffer.clear();

                int chtl_start = current_pos;
                current_pos += 2;
                while (current_pos + 1 < content.length() && content.substr(current_pos, 2) != "}}") {
                    if (content[current_pos] == '\n') current_line++;
                    current_pos++;
                }
                current_pos += 2;
                fragments.push_back({FragmentType::CHTL_JS, content.substr(chtl_start, current_pos - chtl_start), current_line});
                prev_char = '\0';
                continue;
            }

            bool keyword_found = false;
            for (const auto& keyword : chtl_js_keywords) {
                if (content.substr(current_pos, keyword.length()) == keyword) {
                    if (current_pos > 0 && isalnum(content[current_pos-1])) continue;
                    if (current_pos + keyword.length() < content.length() && isalnum(content[current_pos + keyword.length()])) continue;

                    if (!buffer.empty()) fragments.push_back({FragmentType::JS, buffer, current_line});
                    buffer.clear();

                    int chtl_start = current_pos;
                    int lookahead = current_pos + keyword.length();
                    while (lookahead < content.length() && isspace(content[lookahead])) {
                        if (content[lookahead] == '\n') current_line++;
                        lookahead++;
                    }

                    if (lookahead < content.length() && content[lookahead] == '{') {
                        int brace_level = 1;
                        lookahead++;
                        while (lookahead < content.length() && brace_level > 0) {
                            if (content[lookahead] == '\n') current_line++;
                            else if (content[lookahead] == '{') brace_level++;
                            else if (content[lookahead] == '}') brace_level--;
                            lookahead++;
                        }
                        current_pos = lookahead;
                    } else {
                        while (current_pos < content.length() && content[current_pos] != ';' && content[current_pos] != '\n') {
                            current_pos++;
                        }
                    }

                    fragments.push_back({FragmentType::CHTL_JS, content.substr(chtl_start, current_pos - chtl_start), current_line});
                    keyword_found = true;
                    prev_char = '\0';
                    break;
                }
            }
            if (keyword_found) continue;
        }

        buffer += current_char;
        prev_char = current_char;
        current_pos++;
    }

    if (!buffer.empty()) {
        fragments.push_back({FragmentType::JS, buffer, current_line});
    }

    if (fragments.empty() && !content.empty()) {
        return {{FragmentType::JS, content, line_start}};
    }

    if (fragments.empty() && !content.empty()) {
        return {{FragmentType::JS, content, line_start}};
    }

    return fragments;
}

} // namespace CHTL
