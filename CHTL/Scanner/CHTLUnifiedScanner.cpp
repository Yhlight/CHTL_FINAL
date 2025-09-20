#include "CHTLUnifiedScanner.h"
#include <stdexcept>
#include <iostream>

namespace CHTL {

CHTLUnifiedScanner::CHTLUnifiedScanner(const std::string& source)
    : source(source) {}

const std::map<std::string, std::string>& CHTLUnifiedScanner::getPlaceholders() const {
    return placeholder_map;
}

std::vector<CodeFragment> CHTLUnifiedScanner::scan() {
    std::vector<CodeFragment> fragments;
    current = 0;
    line = 1;
    placeholder_counter = 0;
    placeholder_map.clear();

    while (current < source.length()) {
        skip_whitespace_and_comments(current);
        if (current >= source.length()) break;

        if (source.substr(current, 5) == "style") {
            process_style_block(fragments, current);
        } else if (source.substr(current, 6) == "script") {
            process_script_block(fragments, current);
        } else {
            process_chtl_block(fragments, current);
        }
    }

    return fragments;
}

void CHTLUnifiedScanner::process_style_block(std::vector<CodeFragment>& fragments, size_t& pos) {
    size_t block_start = pos;

    // Move past the "style" keyword
    pos += 5;
    skip_whitespace_and_comments(pos);

    if (pos >= source.length() || source[pos] != '{') {
        process_chtl_block(fragments, block_start);
        return;
    }

    size_t content_start = pos + 1;
    size_t content_end = find_matching_brace(pos);

    if (content_end == std::string::npos) {
        throw std::runtime_error("Mismatched braces in style block at line " + std::to_string(line));
    }

    std::string style_content = source.substr(content_start, content_end - content_start);
    pos = content_end + 1;

    size_t current_style_pos = 0;
    while(current_style_pos < style_content.length()) {
        // Skip leading whitespace for the current statement
        while(current_style_pos < style_content.length() && isspace(style_content[current_style_pos])) {
            current_style_pos++;
        }
        if(current_style_pos >= style_content.length()) break;

        size_t stmt_start = current_style_pos;
        size_t stmt_end = style_content.find(';', stmt_start);

        if (stmt_end == std::string::npos) {
            // Handle case with no trailing semicolon
            stmt_end = style_content.length();
        } else {
            stmt_end++; // include the semicolon
        }

        std::string statement = style_content.substr(stmt_start, stmt_end - stmt_start);

        if (!statement.empty()) {
             bool is_chtl_feature = statement.find('@') != std::string::npos ||
                                   statement.find('[') != std::string::npos ||
                                   statement.find('+') != std::string::npos ||
                                   statement.find('?') != std::string::npos ||
                                   statement.find("ThemeColor") != std::string::npos;

            if (is_chtl_feature) {
                fragments.push_back({FragmentType::CHTL, statement, line});
            } else {
                fragments.push_back({FragmentType::CSS, statement, line});
            }
        }
        current_style_pos = stmt_end;
    }
}

// This is the core of the "strict" analysis for script blocks.
void CHTLUnifiedScanner::process_script_block(std::vector<CodeFragment>& fragments, size_t& pos) {
    size_t block_start = pos;

    // Move past the "script" keyword
    pos += 6;
    skip_whitespace_and_comments(pos);

    if (pos >= source.length() || source[pos] != '{') {
        // Not a block, treat the "script" keyword as CHTL content
        process_chtl_block(fragments, block_start);
        return;
    }

    size_t content_start = pos + 1;
    size_t content_end = find_matching_brace(pos);

    if (content_end == std::string::npos) {
        throw std::runtime_error("Mismatched braces in script block at line " + std::to_string(line));
    }

    // The actual content of the script block
    std::string script_content = source.substr(content_start, content_end - content_start);
    pos = content_end + 1; // Move scanner past this block

    std::string js_buffer;
    size_t script_pos = 0;

    while(script_pos < script_content.length()) {
        // Check for CHTL JS constructs
        if (script_content.substr(script_pos, 2) == "{{") {
            // Flush the JS buffer before processing the CHTL JS part
            if (!js_buffer.empty()) {
                fragments.push_back({FragmentType::JS, create_placeholder(js_buffer), line});
                js_buffer.clear();
            }

            // Find the matching '}}'
            size_t chtl_js_end = script_content.find("}}", script_pos);
            if (chtl_js_end == std::string::npos) {
                 throw std::runtime_error("Mismatched '{{' in script block at line " + std::to_string(line));
            }
            chtl_js_end += 2; // include the '}}'
            fragments.push_back({FragmentType::CHTL_JS, script_content.substr(script_pos, chtl_js_end - script_pos), line});
            script_pos = chtl_js_end;
        } else if (script_content.substr(script_pos, 2) == "->") {
            // Flush the JS buffer
             if (!js_buffer.empty()) {
                fragments.push_back({FragmentType::JS, create_placeholder(js_buffer), line});
                js_buffer.clear();
            }
            fragments.push_back({FragmentType::CHTL_JS, "->", line});
            script_pos += 2;
        } else if (isalpha(script_content[script_pos])) {
            // Check for CHTL JS block keywords
            size_t end_of_word = script_pos;
            while(end_of_word < script_content.length() && isalnum(script_content[end_of_word])) {
                end_of_word++;
            }
            std::string keyword = script_content.substr(script_pos, end_of_word - script_pos);

            bool is_chtl_js_keyword = (keyword == "Listen" || keyword == "Animate" || keyword == "Router" || keyword == "Delegate");

            size_t temp_pos = end_of_word;
            while(temp_pos < script_content.length() && isspace(script_content[temp_pos])) {
                temp_pos++;
            }

            if (is_chtl_js_keyword && temp_pos < script_content.length() && script_content[temp_pos] == '{') {
                if (!js_buffer.empty()) {
                    fragments.push_back({FragmentType::JS, create_placeholder(js_buffer), line});
                    js_buffer.clear();
                }

                // Find the end of the CHTL JS block
                // We need to find the matching brace in the substring, so we need to adjust positions.
                size_t block_end_in_substring = find_matching_brace_in_string(script_content, temp_pos);
                if (block_end_in_substring == std::string::npos) {
                    throw std::runtime_error("Mismatched braces for CHTL JS block '" + keyword + "' at line " + std::to_string(line));
                }

                size_t chtl_js_end = block_end_in_substring + 1;
                fragments.push_back({FragmentType::CHTL_JS, script_content.substr(script_pos, chtl_js_end - script_pos), line});
                script_pos = chtl_js_end;

            } else {
                js_buffer += script_content[script_pos];
                script_pos++;
            }
        }
        else {
            js_buffer += script_content[script_pos];
            script_pos++;
        }
    }

    // Flush any remaining JS in the buffer
    if (!js_buffer.empty()) {
        // Trim trailing space from the buffer before creating the fragment
        size_t end = js_buffer.find_last_not_of(" \t\n\r");
        if(end != std::string::npos) {
            js_buffer = js_buffer.substr(0, end + 1);
        }
        if (!js_buffer.empty()){
            fragments.push_back({FragmentType::JS, create_placeholder(js_buffer), line});
        }
    }
}

void CHTLUnifiedScanner::process_chtl_block(std::vector<CodeFragment>& fragments, size_t& pos) {
    size_t start_pos = pos;
    size_t next_style = source.find("style", pos);
    size_t next_script = source.find("script", pos);

    size_t end_pos = std::min(next_style, next_script);
    if (end_pos == std::string::npos) {
        end_pos = source.length();
    }

    std::string content = source.substr(start_pos, end_pos - start_pos);
    if (!content.empty()) {
        fragments.push_back({FragmentType::CHTL, content, line});
    }
    pos = end_pos;
}


std::string CHTLUnifiedScanner::create_placeholder(const std::string& content) {
    std::string placeholder = "_PLACEHOLDER_" + std::to_string(placeholder_counter++) + "_";
    placeholder_map[placeholder] = content;
    return placeholder;
}

size_t CHTLUnifiedScanner::find_matching_brace(size_t start_pos) {
    return find_matching_brace_in_string(source, start_pos);
}

size_t CHTLUnifiedScanner::find_matching_brace_in_string(const std::string& str, size_t start_pos) {
    if (start_pos >= str.length() || str[start_pos] != '{') {
        return std::string::npos;
    }
    int brace_level = 1;
    for (size_t i = start_pos + 1; i < str.length(); ++i) {
        if (str[i] == '{') {
            brace_level++;
        } else if (str[i] == '}') {
            brace_level--;
            if (brace_level == 0) {
                return i;
            }
        }
    }
    return std::string::npos; // Not found
}

void CHTLUnifiedScanner::skip_whitespace_and_comments(size_t& pos) {
    while (pos < source.length()) {
        if (isspace(source[pos])) {
            if (source[pos] == '\n') line++;
            pos++;
            continue;
        }
        if (pos + 1 < source.length() && source[pos] == '/') {
            if (source[pos + 1] == '/') {
                pos += 2;
                while (pos < source.length() && source[pos] != '\n') {
                    pos++;
                }
                if (pos < source.length() && source[pos] == '\n') {
                    line++;
                    pos++;
                }
                continue;
            }
            if (source[pos + 1] == '*') {
                pos += 2;
                while (pos + 1 < source.length() && (source[pos] != '*' || source[pos + 1] != '/')) {
                    if (source[pos] == '\n') line++;
                    pos++;
                }
                if (pos + 1 < source.length()) {
                    pos += 2;
                } else {
                    pos = source.length();
                }
                continue;
            }
        }
        break;
    }
}

} // namespace CHTL
