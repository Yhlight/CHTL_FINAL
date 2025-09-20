#include "CHTLUnifiedScanner.h"
#include <stdexcept>
#include <iostream>
#include <vector>
#include <string>
#include <regex>
#include <algorithm>
#include <cctype> // For isspace, isalpha, isalnum

namespace CHTL {

const std::vector<std::string> CHTLJS_KEYWORDS = {
    "Listen", "Delegate", "Animate", "Router", "Vir", "util"
};

bool is_chtl_in_css_statement(const std::string& statement);

CHTLUnifiedScanner::CHTLUnifiedScanner(const std::string& source)
    : source(source) {}

std::vector<CodeFragment> CHTLUnifiedScanner::scan() {
    std::vector<CodeFragment> fragments;
    while (!is_at_end()) {
        start = current;
        scan_token(fragments);
    }
    for (const auto& pair : js_placeholders) {
        fragments.push_back({FragmentType::JS_PLACEHOLDER, pair.second, -1, pair.first});
    }
    return fragments;
}

void CHTLUnifiedScanner::scan_token(std::vector<CodeFragment>& fragments) {
    switch (state) {
        case ScannerState::TOP_LEVEL:
            scan_top_level(fragments);
            break;
        case ScannerState::IN_STYLE_BLOCK:
            scan_style_block(fragments);
            break;
        case ScannerState::IN_SCRIPT_BLOCK:
            scan_script_block(fragments);
            break;
    }
}

void CHTLUnifiedScanner::scan_top_level(std::vector<CodeFragment>& fragments) {
    while (!is_at_end()) {
        skip_whitespace();
        if (is_at_end()) break;
        size_t checkpoint = current;

        if (source.substr(current, 5) == "style") {
            size_t after_keyword = current + 5;
            size_t temp_cursor = after_keyword;
            while(temp_cursor < source.length() && isspace(source[temp_cursor])) temp_cursor++;
            if (temp_cursor < source.length() && source[temp_cursor] == '{') {
                add_fragment(FragmentType::CHTL, fragments, source.substr(start, current - start));
                add_fragment(FragmentType::CHTL, fragments, "style");
                current = temp_cursor;
                start = current;
                state = ScannerState::IN_STYLE_BLOCK;
                return;
            }
        }

        if (source.substr(current, 6) == "script") {
            size_t after_keyword = current + 6;
            size_t temp_cursor = after_keyword;
            while(temp_cursor < source.length() && isspace(source[temp_cursor])) temp_cursor++;
            if (temp_cursor < source.length() && source[temp_cursor] == '{') {
                add_fragment(FragmentType::CHTL, fragments, source.substr(start, current - start));
                add_fragment(FragmentType::CHTL, fragments, "script");
                current = temp_cursor;
                start = current;
                state = ScannerState::IN_SCRIPT_BLOCK;
                return;
            }
        }

        if (current == checkpoint) advance();
    }
    add_fragment(FragmentType::CHTL, fragments, source.substr(start, current - start));
}

void CHTLUnifiedScanner::scan_style_block(std::vector<CodeFragment>& fragments) {
    if (!match('{')) return;

    int brace_level = 1;
    size_t block_start_pos = current;

    while (!is_at_end() && brace_level > 0) {
        if (peek() == '{') brace_level++;
        else if (peek() == '}') brace_level--;
        if (brace_level > 0) advance();
    }
    std::string content = source.substr(block_start_pos, current - block_start_pos);

    std::string css_buffer;
    size_t local_cursor = 0;
    while (local_cursor < content.length()) {
        size_t stmt_start = content.find_first_not_of(" \t\n\r", local_cursor);
        if (stmt_start == std::string::npos) break;

        size_t stmt_end = stmt_start;
        int inner_brace_level = 0;
        bool in_string = false;
        char string_char = '\0';

        while(stmt_end < content.length()) {
            char ch = content[stmt_end];
            if (in_string) {
                if (ch == string_char) in_string = false;
            } else {
                if (ch == '\'' || ch == '"') {
                    in_string = true;
                    string_char = ch;
                } else if (ch == '{') {
                    inner_brace_level++;
                } else if (ch == '}') {
                    inner_brace_level--;
                } else if (ch == ';' && inner_brace_level == 0) {
                    stmt_end++;
                    break;
                }
            }
            stmt_end++;
            if (inner_brace_level < 0) break;
        }


        std::string statement = content.substr(stmt_start, stmt_end - stmt_start);

        if (is_chtl_in_css_statement(statement)) {
            if (!css_buffer.empty()) {
                add_fragment(FragmentType::CSS, fragments, css_buffer);
                css_buffer.clear();
            }
            add_fragment(FragmentType::CHTL_in_CSS, fragments, statement);
        } else {
            css_buffer += statement;
        }
        local_cursor = stmt_end;
    }

    if (!css_buffer.empty()) {
        add_fragment(FragmentType::CSS, fragments, css_buffer);
    }

    if (!match('}')) { /* error */ }
    start = current;
    state = ScannerState::TOP_LEVEL;
}

void CHTLUnifiedScanner::scan_script_block(std::vector<CodeFragment>& fragments) {
    if (!match('{')) return;
    int brace_level = 1;
    size_t content_start = current;

    while (!is_at_end() && brace_level > 0) {
        if (peek() == '{') brace_level++;
        else if (peek() == '}') brace_level--;
        if (brace_level > 0) advance();
    }

    std::string block_content = source.substr(content_start, current - content_start);
    std::string chtl_js_with_placeholders = process_script_content(block_content);
    add_fragment(FragmentType::CHTL_JS, fragments, chtl_js_with_placeholders);

    if (!match('}')) { /* error */ }
    start = current;
    state = ScannerState::TOP_LEVEL;
}

std::string CHTLUnifiedScanner::process_script_content(const std::string& content) {
    std::string result_string;
    std::string js_buffer;
    size_t local_cursor = 0;

    auto commit_js_buffer = [&]() {
        if (!js_buffer.empty()) {
            std::string placeholder = get_next_placeholder_id();
            js_placeholders[placeholder] = js_buffer;
            result_string += placeholder;
            js_buffer.clear();
        }
    };

    while (local_cursor < content.length()) {
        size_t checkpoint = local_cursor;

        if (content.substr(local_cursor, 2) == "{{") {
            size_t end_pos = content.find("}}", local_cursor);
            if (end_pos != std::string::npos) {
                commit_js_buffer();
                result_string += content.substr(local_cursor, end_pos - local_cursor + 2);
                local_cursor = end_pos + 2;
                continue;
            }
        }
        if (content[local_cursor] == '$' && local_cursor + 1 < content.length() && isalpha(content[local_cursor+1])) {
             size_t end_pos = local_cursor + 1;
             while(end_pos < content.length() && isalnum(content[end_pos])) {
                 end_pos++;
             }
             if (end_pos < content.length() && content[end_pos] == '$') {
                commit_js_buffer();
                result_string += content.substr(local_cursor, end_pos - local_cursor + 1);
                local_cursor = end_pos + 1;
                continue;
             }
        }
        if (content.substr(local_cursor, 3) == "&->") {
            commit_js_buffer();
            result_string += "&->";
            local_cursor += 3;
            continue;
        }
        bool keyword_found = false;
        for (const auto& keyword : CHTLJS_KEYWORDS) {
            if (content.substr(local_cursor, keyword.length()) == keyword) {
                if (local_cursor + keyword.length() < content.length()) {
                    char next_char = content[local_cursor + keyword.length()];
                    if (isspace(next_char) || next_char == '{' || next_char == '=') {
                        commit_js_buffer();
                        result_string += keyword;
                        local_cursor += keyword.length();
                        keyword_found = true;
                        break;
                    }
                }
            }
        }
        if (keyword_found) continue;

        js_buffer += content[local_cursor];
        local_cursor++;
    }
    commit_js_buffer();
    return result_string;
}

bool is_chtl_in_css_statement(const std::string& statement) {
    std::string trimmed_stmt = std::regex_replace(statement, std::regex("^\\s+"), "");
    if (trimmed_stmt.rfind("//", 0) == 0) return true;
    if (std::regex_search(statement, std::regex("\\b(delete|inherit)\\b"))) {
        return true;
    }
    size_t at_pos = statement.find('@');
    if (at_pos != std::string::npos) {
        if (statement.rfind("@media", at_pos) != at_pos &&
            statement.rfind("@keyframes", at_pos) != at_pos &&
            statement.rfind("@font-face", at_pos) != at_pos &&
            statement.rfind("@import", at_pos) != at_pos &&
            statement.rfind("@charset", at_pos) != at_pos &&
            statement.rfind("@namespace", at_pos) != at_pos) {
            return true;
        }
    }
    size_t colon_pos = statement.find(':');
    if (colon_pos != std::string::npos) {
        std::string value = statement.substr(colon_pos + 1);
        if (value.find('?') != std::string::npos) {
            return true;
        }
        std::string cleaned_value = std::regex_replace(value, std::regex("calc\\([^)]*\\)"), "");
        cleaned_value = std::regex_replace(cleaned_value, std::regex("\"[^\"]*\""), "");
        cleaned_value = std::regex_replace(cleaned_value, std::regex("'[^']*'"), "");
        if (std::regex_search(cleaned_value, std::regex("[\\+\\*\\/]"))) {
            return true;
        }
    }
    return false;
}

bool CHTLUnifiedScanner::is_at_end() { return current >= source.length(); }
char CHTLUnifiedScanner::advance() { if (is_at_end()) return '\0'; if (source[current] == '\n') line++; current++; return source[current - 1]; }
char CHTLUnifiedScanner::peek(size_t offset) { if (current + offset >= source.length()) return '\0'; return source[current + offset]; }
char CHTLUnifiedScanner::peek_next() { return peek(1); }
bool CHTLUnifiedScanner::match(char expected) { if (is_at_end() || source[current] != expected) return false; advance(); return true; }
void CHTLUnifiedScanner::skip_whitespace() { while (!is_at_end()) { char c = peek(); switch (c) { case ' ': case '\r': case '\t': case '\n': advance(); break; case '/': if (peek_next() == '/') { skip_line_comment(); } else if (peek_next() == '*') { skip_block_comment(); } else { return; } break; default: return; } } }
void CHTLUnifiedScanner::skip_line_comment() { while (peek() != '\n' && !is_at_end()) advance(); }
void CHTLUnifiedScanner::skip_block_comment() { advance(); advance(); while (!(peek() == '*' && peek_next() == '/') && !is_at_end()) advance(); if (!is_at_end()) advance(); if (!is_at_end()) advance(); }
void CHTLUnifiedScanner::add_fragment(FragmentType type, std::vector<CodeFragment>& fragments, const std::string& content) { if (!content.empty()) { fragments.push_back({type, content, line}); } }
bool CHTLUnifiedScanner::is_alpha(char c) { return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_'; }
bool CHTLUnifiedScanner::is_alpha_numeric(char c) { return isalpha(c) || isdigit(c); }
std::string CHTLUnifiedScanner::consume_identifier() { size_t ident_start = current; while (is_alpha_numeric(peek())) advance(); return source.substr(ident_start, current - ident_start); }
std::string CHTLUnifiedScanner::get_next_placeholder_id() { return "_JS_PLACEHOLDER_" + std::to_string(placeholder_id_counter++) + "_"; }

} // namespace CHTL
