#include "CHTLUnifiedScanner.h"
#include <stdexcept>
#include <iostream>
#include <regex>

namespace CHTL {

CHTLUnifiedScanner::CHTLUnifiedScanner(const std::string& source)
    : source(source) {}

std::vector<CodeFragment> CHTLUnifiedScanner::scan() {
    std::vector<CodeFragment> fragments;
    scan_toplevel(fragments);
    return fragments;
}

void CHTLUnifiedScanner::scan_toplevel(std::vector<CodeFragment>& fragments) {
    std::string chtl_buffer;

    while (!is_at_end()) {
        size_t loop_start_pos = current;

        // Check for `style {`
        if (match_whole_word("style")) {
            size_t after_keyword = current;
            skip_whitespace_and_comments();
            if (peek() == '{') {
                if (!chtl_buffer.empty()) {
                    fragments.push_back({FragmentType::CHTL, chtl_buffer, line});
                    chtl_buffer.clear();
                }

                // Rewind to the start of the style block to process it
                current = loop_start_pos;
                match_whole_word("style");
                skip_whitespace_and_comments();

                int block_start_line = line;
                process_style_block(fragments, block_start_line);
                continue;
            } else {
                current = loop_start_pos; // Rewind
            }
        }

        // Check for `script {`
        if (match_whole_word("script")) {
            size_t after_keyword = current;
            skip_whitespace_and_comments();
            if (peek() == '{') {
                if (!chtl_buffer.empty()) {
                    fragments.push_back({FragmentType::CHTL, chtl_buffer, line});
                    chtl_buffer.clear();
                }

                // Rewind to the start of the script block to process it
                current = loop_start_pos;
                match_whole_word("script");
                skip_whitespace_and_comments();

                int block_start_line = line;
                process_script_block(fragments, block_start_line);
                continue;
            } else {
                current = loop_start_pos; // Rewind
            }
        }

        // If we get here, it's just regular CHTL content.
        // Add the character at the start of the loop to the buffer and advance.
        current = loop_start_pos;
        chtl_buffer += advance();
    }

    if (!chtl_buffer.empty()) {
        fragments.push_back({FragmentType::CHTL, chtl_buffer, line});
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
        // Check for CHTL-specific operators: **, %
        if (value.find("**") != std::string::npos || value.find('%') != std::string::npos) {
            return true;
        }

        // Clean value for safer regex matching
        std::string cleaned_value = std::regex_replace(value, std::regex("calc\\([^)]*\\)"), "");
        cleaned_value = std::regex_replace(cleaned_value, std::regex("\"[^\"]*\""), "");
        cleaned_value = std::regex_replace(cleaned_value, std::regex("'[^']*'"), "");

        // Check for less ambiguous operators +, *, /
        if (std::regex_search(cleaned_value, std::regex("[\\+\\*\\/]"))) {
            return true;
        }
        // A minus sign is likely arithmetic if it's surrounded by spaces.
        if (std::regex_search(cleaned_value, std::regex("\\s-\\s"))) {
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
        if (content[statement_start] == '#' && statement_start + 1 < content.length() && isspace(content[statement_start + 1])) {
            statement_end = content.find('\n', statement_start);
            if (statement_end == std::string::npos) statement_end = content.length();
        } else {
            statement_end = content.find_first_of("};", statement_start);
            if (statement_end == std::string::npos) {
                statement_end = content.length();
            } else {
                statement_end++;
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
    std::string chtl_js_with_placeholders = scan_script_recursive(content, placeholders);

    if (!chtl_js_with_placeholders.empty()) {
        // This fragment contains the CHTL JS code, with placeholders for the pure JS parts.
        // We will also store the map of placeholders within this fragment for the merger to use.
        CodeFragment fragment = {FragmentType::CHTL_JS, chtl_js_with_placeholders, block_start_line};
        fragment.js_placeholders = placeholders;
        fragments.push_back(fragment);
    }
}

std::string CHTLUnifiedScanner::scan_script_recursive(const std::string& input, std::map<std::string, std::string>& placeholders) {
    std::string output;
    size_t local_cursor = 0;

    const std::vector<std::string> simple_patterns = {"{{", "&->", "$"};
    const std::vector<std::string> block_keywords = {
        "ScriptLoader", "Listen", "Delegate", "Animate", "Router",
        "printMylove", "iNeverAway"
    };

    while (local_cursor < input.length()) {
        size_t next_pos = std::string::npos;
        std::string found_construct;
        bool is_block = false;

        // Find earliest simple pattern
        for (const auto& pattern : simple_patterns) {
            size_t pos = input.find(pattern, local_cursor);
            if (pos != std::string::npos && (pos < next_pos)) {
                next_pos = pos;
                found_construct = pattern;
                is_block = false;
            }
        }

        // Find earliest block keyword
        for (const auto& keyword : block_keywords) {
            size_t pos = local_cursor;
            while ((pos = input.find(keyword, pos)) != std::string::npos) {
                bool is_prefix_ok = (pos == 0) || !isalnum(input[pos - 1]);
                bool is_suffix_ok = (pos + keyword.length() >= input.length()) || !isalnum(input[pos + keyword.length()]);
                if (is_prefix_ok && is_suffix_ok) {
                    if (pos < next_pos) {
                        next_pos = pos;
                        found_construct = keyword;
                        is_block = true;
                    }
                    break;
                }
                pos += keyword.length();
            }
        }

        if (next_pos == std::string::npos) {
            if (local_cursor < input.length()) {
                std::string js_part = input.substr(local_cursor);
                if (!js_part.empty()) {
                    std::string placeholder_id = get_next_placeholder_id();
                    placeholders[placeholder_id] = js_part;
                    output += placeholder_id;
                }
            }
            break;
        }

        if (next_pos > local_cursor) {
            std::string js_part = input.substr(local_cursor, next_pos - local_cursor);
            if (!js_part.empty()) {
                std::string placeholder_id = get_next_placeholder_id();
                placeholders[placeholder_id] = js_part;
                output += placeholder_id;
            }
        }

        size_t construct_start = next_pos;
        size_t construct_end = construct_start;

        if (is_block) {
            size_t brace_pos = input.find('{', construct_start + found_construct.length());
            if (brace_pos != std::string::npos) {
                int brace_level = 1;
                size_t content_cursor = brace_pos + 1;
                while (content_cursor < input.length() && brace_level > 0) {
                    if (input[content_cursor] == '{') brace_level++;
                    if (input[content_cursor] == '}') brace_level--;
                    content_cursor++;
                }
                construct_end = content_cursor;
            } else {
                construct_end = construct_start + found_construct.length();
            }
        } else {
            if (found_construct == "{{") {
                construct_end = input.find("}}", construct_start);
                if (construct_end != std::string::npos) construct_end += 2;
            } else if (found_construct == "$") {
                construct_end = input.find("$", construct_start + 1);
                if (construct_end != std::string::npos) construct_end += 1;
            } else { // &->
                construct_end = construct_start + found_construct.length();
            }
        }

        if (construct_end == std::string::npos || construct_end > input.length()) {
            construct_end = input.length();
        }

        output += input.substr(construct_start, construct_end - construct_start);
        local_cursor = construct_end;
    }
    return output;
}


// --- Utility Methods ---

std::string CHTLUnifiedScanner::consume_block_content() {
    if (peek() != '{') {
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

bool CHTLUnifiedScanner::match_whole_word(const std::string& word) {
    if (source.rfind(word, current) == current) {
        if (current + word.length() >= source.length() || !isalnum(source[current + word.length()])) {
            // It's a whole word. Consume and return true.
            // We don't consume here, we let the caller do it.
            // The function is just a predicate. Let's change this.
            // The caller expects `current` to be advanced.
            current += word.length();
            return true;
        }
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
