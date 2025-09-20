#include "CHTLUnifiedScanner.h"
#include <stdexcept>
#include <iostream>

namespace CHTL {

CHTLUnifiedScanner::CHTLUnifiedScanner(const std::string& source)
    : source(source) {}

std::vector<CodeFragment> CHTLUnifiedScanner::scan() {
    std::vector<CodeFragment> fragments;
    scan_toplevel(fragments);
    return fragments;
}

void CHTLUnifiedScanner::scan_toplevel(std::vector<CodeFragment>& fragments) {
    size_t last_pos = 0;
    current = 0;

    while (current < source.length()) {
        // Find the next potential block start
        size_t style_pos = source.find("style", current);
        size_t script_pos = source.find("script", current);

        size_t next_block_pos = std::min(style_pos, script_pos);

        if (next_block_pos == std::string::npos) {
            // No more style or script blocks, the rest is CHTL
            break;
        }

        bool is_style = (next_block_pos == style_pos);

        // Check if it's a real block (keyword followed by optional whitespace and '{')
        size_t keyword_end = next_block_pos + (is_style ? 5 : 6);
        size_t brace_pos = source.find('{', keyword_end);

        bool is_valid_block = false;
        if (brace_pos != std::string::npos) {
            is_valid_block = true;
            for (size_t i = keyword_end; i < brace_pos; ++i) {
                if (!isspace(source[i])) {
                    is_valid_block = false;
                    break;
                }
            }
        }

        if (is_valid_block) {
            // The content before this block is CHTL
            if (next_block_pos > last_pos) {
                fragments.push_back({FragmentType::CHTL, source.substr(last_pos, next_block_pos - last_pos), line});
            }

            current = brace_pos;
            int block_start_line = line; // apx

            if (is_style) {
                process_style_block(fragments, block_start_line);
            } else { // is_script
                process_script_block(fragments, block_start_line);
            }

            last_pos = current;
        } else {
            // This wasn't a valid block, so we skip the keyword and continue searching
            current = next_block_pos + 1;
        }
    }

    // Add any remaining content as a CHTL fragment
    if (last_pos < source.length()) {
        fragments.push_back({FragmentType::CHTL, source.substr(last_pos), line});
    }
}

void CHTLUnifiedScanner::process_style_block(std::vector<CodeFragment>& fragments, int block_start_line) {
    std::string content = consume_block_content();

    std::string css_buffer;
    std::string chtl_buffer;

    size_t content_cursor = 0;
    while(content_cursor < content.length()) {
        // Find the end of the next statement (';' or '{')
        size_t statement_end = content.find_first_of("};", content_cursor);
        if (statement_end == std::string::npos) {
            // End of content
            statement_end = content.length();
        } else {
            // include the terminator
            statement_end++;
        }

        std::string statement = content.substr(content_cursor, statement_end - content_cursor);

        // Simple heuristic to detect CHTL syntax
        bool is_chtl_syntax =
            statement.find('@') != std::string::npos ||
            statement.find('?') != std::string::npos ||
            statement.find('[') != std::string::npos ||
            statement.find("delete") != std::string::npos ||
            statement.find("inherit") != std::string::npos;
            // A more complex check could parse property values for arithmetic operators
            // For now, this is a reasonable approximation.

        if (is_chtl_syntax) {
            // If there's pending CSS, commit it first
            if (!css_buffer.empty()) {
                fragments.push_back({FragmentType::CSS, css_buffer, block_start_line});
                css_buffer.clear();
            }
            // Commit the CHTL statement
            fragments.push_back({FragmentType::CHTL_in_CSS, statement, block_start_line});
        } else {
            // It's likely standard CSS, add to buffer
            css_buffer += statement;
        }

        content_cursor = statement_end;
    }

    // Commit any remaining CSS in the buffer
    if (!css_buffer.empty()) {
        fragments.push_back({FragmentType::CSS, css_buffer, block_start_line});
    }
}

void CHTLUnifiedScanner::process_script_block(std::vector<CodeFragment>& fragments, int block_start_line) {
    std::string content = consume_block_content();
    if (content.empty()) {
        return;
    }

    std::map<std::string, std::string> placeholders;
    std::string chtl_js_content = scan_script_recursive(content, placeholders);

    // Add the main CHTL_JS fragment
    if (!chtl_js_content.empty()) {
        fragments.push_back({FragmentType::CHTL_JS, chtl_js_content, block_start_line});
    }

    // Add all the pure JS fragments that were extracted
    for (const auto& pair : placeholders) {
        // The key (placeholder_id) is implicitly linked to the CHTL_JS fragment.
        // The CodeMerger will need to handle this relationship.
        // We add the JS content as a fragment.
        fragments.push_back({FragmentType::JS, pair.second, block_start_line}); // Line number is an approximation
    }
}

std::string CHTLUnifiedScanner::scan_script_recursive(const std::string& input, std::map<std::string, std::string>& placeholders) {
    std::string output;
    size_t cursor = 0;

    const std::vector<std::string> keywords = {"Listen", "Delegate", "Animate", "Router", "Vir", "iNeverAway", "util"};
    const std::vector<std::string> patterns = {"{{", "&->", "$"};

    while (cursor < input.length()) {
        size_t next_pos = std::string::npos;
        std::string found_token;

        // Find the earliest CHTL JS token (keyword or pattern)
        // 1. Keywords
        for (const auto& keyword : keywords) {
            size_t pos = input.find(keyword, cursor);
            if (pos != std::string::npos && (pos < next_pos)) {
                 if ((pos == 0 || !isalnum(input[pos-1])) && (pos + keyword.length() == input.length() || !isalnum(input[pos + keyword.length()]))) {
                    next_pos = pos;
                    found_token = keyword;
                 }
            }
        }
        // 2. Patterns
        for (const auto& pattern : patterns) {
            size_t pos = input.find(pattern, cursor);
            if (pos != std::string::npos && (pos < next_pos)) {
                next_pos = pos;
                found_token = pattern;
            }
        }

        if (next_pos == std::string::npos) {
            // No more CHTL JS, the rest is pure JS
            if (cursor < input.length()) {
                std::string js_part = input.substr(cursor);
                std::string placeholder_id = get_next_placeholder_id();
                placeholders[placeholder_id] = js_part;
                output += placeholder_id;
            }
            break;
        }

        // The content before the token is pure JS
        if (next_pos > cursor) {
            std::string js_part = input.substr(cursor, next_pos - cursor);
            std::string placeholder_id = get_next_placeholder_id();
            placeholders[placeholder_id] = js_part;
            output += placeholder_id;
        }

        // Process the CHTL JS token
        size_t token_start = next_pos;
        size_t token_end = token_start + found_token.length();

        if (found_token == "{{") {
            token_end = input.find("}}", token_start);
            if (token_end != std::string::npos) token_end += 2;
        } else if (found_token == "$") {
            token_end = input.find("$", token_start + 1);
            if (token_end != std::string::npos) token_end += 1;
        } else { // Keyword expecting a block
            size_t block_start = input.find('{', token_start);
            if (block_start != std::string::npos) {
                int brace_level = 1;
                size_t content_start = block_start + 1;
                token_end = content_start;
                 while(token_end < input.length() && brace_level > 0) {
                    if (input[token_end] == '{') brace_level++;
                    if (input[token_end] == '}') brace_level--;
                    token_end++;
                }
                size_t content_end = token_end - 1;
                std::string inner_content = input.substr(content_start, content_end - content_start);
                std::string processed_inner = scan_script_recursive(inner_content, placeholders);
                output += input.substr(token_start, content_start - token_start) + processed_inner + "}";
                cursor = token_end;
                continue;
            }
        }

        if (token_end == std::string::npos) token_end = input.length();
        output += input.substr(token_start, token_end - token_start);
        cursor = token_end;
    }

    return output;
}


// --- Utility Methods ---

std::string CHTLUnifiedScanner::consume_block_content() {
    if (peek() != '{') {
        // Should not happen if called correctly
        return "";
    }
    advance(); // Consume '{'

    std::string content;
    int brace_level = 1;
    while (!is_at_end() && brace_level > 0) {
        if (peek() == '{') brace_level++;
        if (peek() == '}') brace_level--;

        if (brace_level > 0) {
            content += advance();
        }
    }

    if (!is_at_end()) {
        advance(); // Consume '}'
    }
    return content;
}

bool CHTLUnifiedScanner::is_at_end() {
    return current >= source.length();
}

char CHTLUnifiedScanner::advance() {
    if (is_at_end()) return '\0';
    if (source[current] == '\n') line++;
    return source[current++];
}

char CHTLUnifiedScanner::peek(size_t offset) {
    if (current + offset >= source.length()) return '\0';
    return source[current + offset];
}

bool CHTLUnifiedScanner::match(const std::string& expected) {
    if (source.rfind(expected, current) == current) {
        current += expected.length();
        return true;
    }
    return false;
}

void CHTLUnifiedScanner::skip_whitespace_and_comments() {
    while (!is_at_end()) {
        char c = peek();
        if (isspace(c)) {
            advance();
        } else if (peek() == '/' && peek(1) == '/') {
            while (peek() != '\n' && !is_at_end()) advance();
        } else if (peek() == '/' && peek(1) == '*') {
            advance(); advance(); // Consume /*
            while (!(peek() == '*' && peek(1) == '/') && !is_at_end()) {
                advance();
            }
            if (!is_at_end()) advance(); // *
            if (!is_at_end()) advance(); // /
        } else {
            break;
        }
    }
}

std::string CHTLUnifiedScanner::get_next_placeholder_id() {
    return "_JS_PLACEHOLDER_" + std::to_string(placeholder_id_counter++) + "_";
}

} // namespace CHTL
