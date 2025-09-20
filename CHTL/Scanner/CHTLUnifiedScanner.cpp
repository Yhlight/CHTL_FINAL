#include "CHTLUnifiedScanner.h"
#include <stdexcept>
#include <iostream>

namespace CHTL {

CHTLUnifiedScanner::CHTLUnifiedScanner(const std::string& source)
    : source(source) {}


bool CHTLUnifiedScanner::isAtEnd() {
    return current >= source.length();
}

char CHTLUnifiedScanner::advance() {
    if (!isAtEnd()) {
        if (source[current] == '\n') {
            line++;
        }
        current++;
    }
    return source[current - 1];
}

char CHTLUnifiedScanner::peek() {
    if (isAtEnd()) return '\0';
    return source[current];
}

char CHTLUnifiedScanner::peekNext() {
    if (current + 1 >= source.length()) return '\0';
    return source[current + 1];
}

void CHTLUnifiedScanner::skipWhitespace() {
    while (!isAtEnd()) {
        char c = peek();
        switch (c) {
            case ' ':
            case '\r':
            case '\t':
            case '\n':
                advance();
                break;
            default:
                return;
        }
    }
}

std::vector<CodeFragment> CHTLUnifiedScanner::scan() {
    std::vector<CodeFragment> fragments;
    int fragment_start = 0;
    int line_start = 1;

    while (!isAtEnd()) {
        // Skip comments first
        if (peek() == '/' && peekNext() == '/') {
            while (peek() != '\n' && !isAtEnd()) advance();
            continue;
        }
        if (peek() == '/' && peekNext() == '*') {
            advance(); // Consume '/'
            advance(); // Consume '*'
            while (!(peek() == '*' && peekNext() == '/') && !isAtEnd()) {
                advance();
            }
            if (!isAtEnd()) {
                advance(); // Consume '*'
                advance(); // Consume '/'
            }
            continue;
        }

        bool is_style = source.substr(current, 5) == "style";
        bool is_script = source.substr(current, 6) == "script";

        int lookahead_pos = current + (is_style ? 5 : (is_script ? 6 : 0));

        // Temporarily advance to check for '{'
        int temp_current = current;
        int temp_line = line;

        this->current = lookahead_pos;
        skipWhitespace();
        bool is_block = peek() == '{';

        // Restore cursor
        this->current = temp_current;
        this->line = temp_line;


        if ((is_style || is_script) && is_block) {
            // 1. Finalize the preceding CHTL fragment
            if (current > fragment_start) {
                fragments.push_back({FragmentType::CHTL, source.substr(fragment_start, current - fragment_start), line_start});
            }

            // 2. Find the end of the block
            lookahead_pos = current + (is_style ? 5 : 6);
            while(lookahead_pos < source.length() && isspace(source[lookahead_pos])) {
                lookahead_pos++;
            }
            int content_start = lookahead_pos + 1;
            int content_line_start = line;

            int brace_level = 1;
            current = content_start;
            while (!isAtEnd() && brace_level > 0) {
                if (peek() == '{') brace_level++;
                if (peek() == '}') brace_level--;
                advance();
            }
            int content_end = current - 1;

            // 3. Call the appropriate processor
            if (is_style) {
                process_style_block(fragments, content_start, content_end);
            } else { // is_script
                process_script_block(fragments, content_start, content_end);
            }

            // 4. Reset for the next fragment
            fragment_start = current;
            line_start = line;
        } else {
            advance();
        }
    }

    // Add the final remaining part as a CHTL fragment
    if (current > fragment_start) {
        fragments.push_back({FragmentType::CHTL, source.substr(fragment_start, current - fragment_start), line_start});
    }

    return fragments;
}


void CHTLUnifiedScanner::process_style_block(std::vector<CodeFragment>& fragments, int content_start, int content_end) {
    int local_current = content_start;
    int css_fragment_start = content_start;
    int current_line = this->line; // Use the line number at the start of the block

    while (local_current < content_end) {
        int statement_start = local_current;

        // Naively find the end of a statement (semicolon or a full css rule block)
        int statement_end = local_current;
        int inner_brace_level = 0;
        while(statement_end < content_end) {
            if (source[statement_end] == '{') {
                inner_brace_level++;
            } else if (source[statement_end] == '}') {
                if (inner_brace_level > 0) {
                    inner_brace_level--;
                } else {
                    // End of a rule block that started at level 0
                    statement_end++;
                    break;
                }
            } else if (source[statement_end] == ';' && inner_brace_level == 0) {
                statement_end++; // include semicolon
                break;
            }
            statement_end++;
        }


        std::string statement = source.substr(statement_start, statement_end - statement_start);

        // Heuristic check for CHTL features
        bool is_chtl = (
            statement.find("@Style") != std::string::npos ||
            statement.find("@Var") != std::string::npos ||
            statement.find("?") != std::string::npos ||
            statement.find("[Template]") != std::string::npos ||
            statement.find("[Custom]") != std::string::npos ||
            statement.find("[Origin]") != std::string::npos ||
            statement.find("delete") != std::string::npos ||
            statement.find("inherit") != std::string::npos ||
            (statement.find(":") != std::string::npos && (statement.find("+") != std::string::npos || statement.find("-") != std::string::npos || statement.find("*") != std::string::npos || statement.find("/") != std::string::npos) && statement.find("calc") == std::string::npos)
        );

        if (is_chtl) {
            // Finalize preceding CSS fragment
            if (statement_start > css_fragment_start) {
                fragments.push_back({FragmentType::CSS, source.substr(css_fragment_start, statement_start - css_fragment_start), current_line});
            }

            // Add the CHTL fragment
            fragments.push_back({FragmentType::CHTL, statement, current_line});

            // Reset for next CSS fragment
            css_fragment_start = statement_end;
        }

        local_current = statement_end;
    }

    // Add any remaining CSS content
    if (content_end > css_fragment_start) {
        fragments.push_back({FragmentType::CSS, source.substr(css_fragment_start, content_end - css_fragment_start), current_line});
    }
}

void CHTLUnifiedScanner::process_script_block(std::vector<CodeFragment>& fragments, int content_start, int content_end) {
    int last_pos = content_start;
    int current_pos = content_start;

    while (current_pos < content_end) {
        if (current_pos + 1 < content_end && source.substr(current_pos, 2) == "{{") {
            if (current_pos > last_pos) {
                fragments.push_back({FragmentType::JS, source.substr(last_pos, current_pos - last_pos), line});
            }
            int chtml_start = current_pos + 2;
            current_pos += 2;
            while (current_pos < content_end - 1 && source.substr(current_pos, 2) != "}}") {
                current_pos++;
            }
            fragments.push_back({FragmentType::CHTL, source.substr(chtml_start, current_pos - chtml_start), line});
            if (current_pos < content_end - 1) {
                current_pos += 2;
            } else {
                current_pos = content_end;
            }
            last_pos = current_pos;
        } else {
            current_pos++;
        }
    }

    if (current_pos > last_pos) {
        fragments.push_back({FragmentType::JS, source.substr(last_pos, current_pos - last_pos), line});
    }
}

} // namespace CHTL
