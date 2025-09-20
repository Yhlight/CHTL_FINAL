#include "CHTLUnifiedScanner.h"
#include <stdexcept>
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <unordered_set>
#include <cctype>

namespace CHTL {

// Helper function to check if a string is only whitespace
static bool is_whitespace(const std::string& s) {
    return std::all_of(s.begin(), s.end(), isspace);
}

// Helper function to trim from start (in place)
static inline void ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    }));
}

// Helper function to trim from end (in place)
static inline void rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
    }).base(), s.end());
}

// Helper function to trim from both ends (in place)
static inline void trim(std::string &s) {
    ltrim(s);
    rtrim(s);
}


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
    int current_pos = 0;
    int current_line = line_start;

    // Removed '-' from operators to prevent mis-classifying CSS properties like 'text-align'
    const std::string chtl_operators = "?+*/";

    while(current_pos < content.length()) {
        int statement_start = current_pos;
        int statement_end = content.find(';', statement_start);

        if (statement_end == std::string::npos) {
            statement_end = content.length();
        } else {
            statement_end++; // include the semicolon
        }

        std::string statement = content.substr(statement_start, statement_end - statement_start);
        trim(statement);

        if (!statement.empty()) {
            bool is_chtl = statement.find('@') != std::string::npos ||
                           statement.find('#') != std::string::npos ||
                           statement.find_first_of(chtl_operators) != std::string::npos;

            if (is_chtl) {
                fragments.push_back({FragmentType::CHTL, statement, current_line});
            } else {
                fragments.push_back({FragmentType::CSS, statement, current_line});
            }
        }
        current_pos = statement_end;
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

    auto push_js_buffer = [&](std::string& buf) {
        if (!buf.empty() && !is_whitespace(buf)) {
            fragments.push_back({FragmentType::JS, buf, current_line});
        }
        buf.clear();
    };

    while (current_pos < content.length()) {
        char current_char = content[current_pos];
        bool chtl_found = false;

        if (content[current_pos] == '\n') current_line++;

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

        if (state == ScriptState::NORMAL) {
            int chtl_start = current_pos;
            int chtl_end = -1;

            if (content.substr(current_pos, 2) == "{{") {
                chtl_end = current_pos + 2;
                while (chtl_end + 1 < content.length() && content.substr(chtl_end, 2) != "}}") chtl_end++;
                chtl_end += 2;
            } else if (content.substr(current_pos, 3) == "&->") {
                chtl_end = current_pos + 3;
            } else if (current_char == '$' && current_pos + 1 < content.length() && isalpha(content[current_pos + 1])) {
                chtl_end = current_pos + 1;
                while (chtl_end < content.length() && content[chtl_end] != '$') chtl_end++;
                chtl_end++;
            } else {
                 for (const auto& keyword : chtl_js_keywords) {
                    if (content.substr(current_pos, keyword.length()) == keyword) {
                        if (current_pos > 0 && isalnum(content[current_pos-1])) continue;
                        if (current_pos + keyword.length() < content.length() && isalnum(content[current_pos + keyword.length()])) continue;

                        chtl_end = current_pos + keyword.length();
                        while (chtl_end < content.length() && isspace(content[chtl_end])) chtl_end++;
                        if (chtl_end < content.length() && content[chtl_end] == '{') {
                            int brace_level = 1;
                            chtl_end++;
                            while (chtl_end < content.length() && brace_level > 0) {
                                if (content[chtl_end] == '{') brace_level++;
                                else if (content[chtl_end] == '}') brace_level--;
                                chtl_end++;
                            }
                        } else {
                            while (chtl_end < content.length() && chtl_end != '\n' && content[chtl_end] != ';') chtl_end++;
                        }
                        break;
                    }
                }
            }

            if (chtl_end != -1) {
                push_js_buffer(buffer);
                fragments.push_back({FragmentType::CHTL_JS, content.substr(chtl_start, chtl_end - chtl_start), current_line});
                current_pos = chtl_end - 1;
                chtl_found = true;
            }
        }

        if (!chtl_found) {
            buffer += current_char;
        }

        prev_char = current_char;
        current_pos++;
    }

    push_js_buffer(buffer);

    if (fragments.empty() && !content.empty()) {
        return {{FragmentType::JS, content, line_start}};
    }

    return fragments;
}

} // namespace CHTL
