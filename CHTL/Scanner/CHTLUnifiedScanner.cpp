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

bool CHTLUnifiedScanner::is_chtl_in_css_statement(const std::string& statement) {
    // 1. Check for CHTL keywords
    if (statement.find("delete") != std::string::npos || statement.find("inherit") != std::string::npos) {
        return true;
    }

    // 2. Check for CHTL comments (# comment) vs CSS hex colors (#id)
    size_t hash_pos = statement.find('#');
    if (hash_pos != std::string::npos) {
        if (hash_pos + 1 < statement.length() && isspace(statement[hash_pos + 1])) {
            return true; // It's a CHTL generator comment
        }
    }

    // 3. Check for CHTL's [Keyword] vs CSS's [attribute=value]
    size_t open_bracket_pos = statement.find('[');
    if (open_bracket_pos != std::string::npos) {
        size_t close_bracket_pos = statement.find(']', open_bracket_pos);
        if (close_bracket_pos != std::string::npos) {
            bool has_equals = false;
            for (size_t i = open_bracket_pos; i < close_bracket_pos; ++i) {
                if (statement[i] == '=') {
                    has_equals = true;
                    break;
                }
            }
            if (!has_equals) {
                return true; // It's a CHTL [Keyword]
            }
        }
    }

    // 4. Check for CHTL's @-rules vs standard CSS @-rules
    size_t at_pos = statement.find('@');
    if (at_pos != std::string::npos) {
        const std::vector<std::string> standard_at_rules = {
            "@charset", "@import", "@namespace", "@media", "@supports", "@font-face",
            "@keyframes", "@page", "@counter-style", "@font-feature-values"
        };
        std::string at_rule = statement.substr(at_pos);
        bool is_standard = false;
        for (const auto& rule : standard_at_rules) {
            if (at_rule.rfind(rule, 0) == 0) { // Check if the statement starts with a standard rule
                is_standard = true;
                break;
            }
        }
        if (!is_standard) {
            return true; // It's a CHTL @-rule like @Style or @Var
        }
        // If it IS a standard rule, we should not perform further checks on it.
        return false;
    }

    // 5. Check for property-based CHTL: arithmetic and conditionals
    size_t colon_pos = statement.find(':');
    if (colon_pos != std::string::npos) {
        std::string value = statement.substr(colon_pos + 1);
        // Check for ? for conditionals
        if (value.find('?') != std::string::npos) {
            return true;
        }
        // Check for arithmetic operators
        if (value.find('+') != std::string::npos || value.find('-') != std::string::npos ||
            value.find('*') != std::string::npos || value.find('/') != std::string::npos) {
            // This is still a heuristic, as operators can appear inside strings or url().
            // A full implementation would need a proper value tokenizer.
            // For this project's scope, this is a significant improvement.
            return true;
        }
    }

    return false;
}

void CHTLUnifiedScanner::process_style_block(std::vector<CodeFragment>& fragments, int block_start_line) {
    std::string content = consume_block_content();
    std::string css_buffer;
    size_t content_cursor = 0;

    while (content_cursor < content.length()) {
        size_t statement_start = content.find_first_not_of(" \t\n\r", content_cursor);
        if (statement_start == std::string::npos) {
            break; // No more non-whitespace content
        }

        size_t statement_end;
        // Special handling for CHTL comments
        if (content[statement_start] == '#' && statement_start + 1 < content.length() && isspace(content[statement_start + 1])) {
            statement_end = content.find('\n', statement_start);
            if (statement_end == std::string::npos) {
                statement_end = content.length();
            }
        } else {
            statement_end = content.find_first_of("};", statement_start);
            if (statement_end == std::string::npos) {
                statement_end = content.length();
            } else {
                statement_end++; // Include the terminator
            }
        }

        std::string statement = content.substr(statement_start, statement_end - statement_start);

        if (statement.empty()) {
            content_cursor = statement_end;
            continue;
        }

        if (is_chtl_in_css_statement(statement)) {
            if (!css_buffer.empty()) {
                fragments.push_back({FragmentType::CSS, css_buffer, block_start_line});
                css_buffer.clear();
            }
            fragments.push_back({FragmentType::CHTL_in_CSS, statement, block_start_line});
            // Also add the pending CSS buffer from the same line if any
            std::string remaining_line = content.substr(statement_end, content.find('\n', statement_end) - statement_end);
            if (!remaining_line.empty()) {
                 css_buffer += remaining_line;
            }

        } else {
            css_buffer += statement;
        }

        content_cursor = statement_end;
    }

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

    const std::vector<std::string> patterns = {"{{", "&->", "$"};

    while (cursor < input.length()) {
        size_t next_pos = std::string::npos;
        std::string found_pattern;

        // Find the earliest CHTL JS special pattern
        for (const auto& pattern : patterns) {
            size_t pos = input.find(pattern, cursor);
            if (pos != std::string::npos && (pos < next_pos)) {
                next_pos = pos;
                found_pattern = pattern;
            }
        }

        if (next_pos == std::string::npos) {
            // No more CHTL JS patterns, the rest is pure JS
            if (cursor < input.length()) {
                std::string js_part = input.substr(cursor);
                if (!js_part.empty()) {
                    std::string placeholder_id = get_next_placeholder_id();
                    placeholders[placeholder_id] = js_part;
                    output += placeholder_id;
                }
            }
            break; // Exit loop
        }

        // The content before the pattern is pure JS
        if (next_pos > cursor) {
            std::string js_part = input.substr(cursor, next_pos - cursor);
             if (!js_part.empty()) {
                std::string placeholder_id = get_next_placeholder_id();
                placeholders[placeholder_id] = js_part;
                output += placeholder_id;
            }
        }

        // Process and append the CHTL JS pattern itself
        size_t pattern_start = next_pos;
        size_t pattern_end = pattern_start;

        if (found_pattern == "{{") {
            pattern_end = input.find("}}", pattern_start);
            if (pattern_end != std::string::npos) {
                pattern_end += 2; // include the "}}"
            }
        } else if (found_pattern == "$") {
            pattern_end = input.find("$", pattern_start + 1);
            if (pattern_end != std::string::npos) {
                pattern_end += 1; // include the second "$"
            }
        } else { // &-> or other simple patterns
            pattern_end = pattern_start + found_pattern.length();
        }

        if (pattern_end == std::string::npos) {
            // Malformed pattern, treat rest of string as JS
            pattern_end = input.length();
        }

        output += input.substr(pattern_start, pattern_end - pattern_start);
        cursor = pattern_end;
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
