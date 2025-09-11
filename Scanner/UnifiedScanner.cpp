#include "UnifiedScanner.h"
#include <sstream>
#include <cctype>

// Helper to find the matching closing brace for a block, handling nested braces.
size_t find_matching_brace(const std::string& s, size_t start_pos) {
    if (start_pos >= s.length() || s[start_pos] != '{') {
        return std::string::npos;
    }
    int depth = 1;
    for (size_t i = start_pos + 1; i < s.length(); ++i) {
        if (s[i] == '{') {
            depth++;
        } else if (s[i] == '}') {
            depth--;
            if (depth == 0) {
                return i;
            }
        }
    }
    return std::string::npos;
}

// Helper to skip whitespace
void skip_whitespace(const std::string& s, size_t& pos) {
    while (pos < s.length() && isspace(s[pos])) {
        pos++;
    }
}

UnifiedScanner::UnifiedScanner(const std::string& source) : source(source) {}

std::string UnifiedScanner::generatePlaceholder(const std::string& type) {
    std::stringstream placeholder;
    if (type == "STYLE") {
        placeholder << "__CHTL_STYLE_PLACEHOLDER_" << css_block_count++ << "__";
    } else if (type == "SCRIPT") {
        placeholder << "__CHTL_SCRIPT_PLACEHOLDER_" << js_block_count++ << "__";
    } else if (type == "ORIGIN_CSS") {
        placeholder << "__CHTL_ORIGIN_CSS_PLACEHOLDER_" << css_block_count++ << "__";
    } else if (type == "ORIGIN_JS") {
        placeholder << "__CHTL_ORIGIN_JS_PLACEHOLDER_" << js_block_count++ << "__";
    } else if (type == "ORIGIN_HTML") {
        placeholder << "__CHTL_ORIGIN_HTML_PLACEHOLDER_" << js_block_count++ << "__";
    }
    return placeholder.str();
}

ScannedContent UnifiedScanner::scan() {
    std::stringstream processed_ss;

    while (current < source.length()) {
        // Check for [Origin] blocks first, as they are fully replaced
        if (source.substr(current, 8) == "[Origin]") {
            size_t block_type_start = current + 8;
            skip_whitespace(source, block_type_start);

            std::string type;
            if (source[block_type_start] == '@') {
                 size_t type_end = source.find_first_of(" \t\n{", block_type_start);
                 type = source.substr(block_type_start, type_end - block_type_start);
            }

            size_t block_start = source.find('{', current);
            if (block_start != std::string::npos) {
                size_t block_end = find_matching_brace(source, block_start);
                if (block_end != std::string::npos) {
                    std::string block_content = source.substr(block_start + 1, block_end - block_start - 1);
                    std::string placeholder;
                    if (type == "@Style" || type == "@CSS") {
                        placeholder = generatePlaceholder("ORIGIN_CSS");
                        content.css_placeholders[placeholder] = block_content;
                    } else if (type == "@JavaScript" || type == "@JS") {
                        placeholder = generatePlaceholder("ORIGIN_JS");
                        content.js_placeholders[placeholder] = block_content;
                    } else { // @Html or other custom types
                        placeholder = generatePlaceholder("ORIGIN_HTML");
                        content.js_placeholders[placeholder] = block_content; // JS map can hold HTML for now
                    }
                    processed_ss << placeholder;
                    current = block_end + 1;
                    continue;
                }
            }
        }

        // Check for regular `style` blocks
        if (source.substr(current, 5) == "style" && (isspace(source[current+5]) || source[current+5] == '{')) {
            size_t block_start = source.find('{', current);
            if (block_start != std::string::npos) {
                size_t block_end = find_matching_brace(source, block_start);
                if (block_end != std::string::npos) {
                    std::string block_content = source.substr(block_start + 1, block_end - block_start - 1);
                    std::string placeholder = generatePlaceholder("STYLE");
                    content.css_placeholders[placeholder] = block_content;

                    processed_ss << source.substr(current, block_start - current + 1) << placeholder << "}";
                    current = block_end + 1;
                    continue;
                }
            }
        }

        // Check for regular `script` blocks
        if (source.substr(current, 6) == "script" && (isspace(source[current+6]) || source[current+6] == '{')) {
            size_t block_start = source.find('{', current);
            if (block_start != std::string::npos) {
                size_t block_end = find_matching_brace(source, block_start);
                if (block_end != std::string::npos) {
                    std::string block_content = source.substr(block_start + 1, block_end - block_start - 1);
                    std::string placeholder = generatePlaceholder("SCRIPT");
                    content.js_placeholders[placeholder] = block_content;

                    processed_ss << source.substr(current, block_start - current + 1) << placeholder << "}";
                    current = block_end + 1;
                    continue;
                }
            }
        }

        processed_ss << source[current];
        current++;
    }

    content.chtl_content = processed_ss.str();
    return content;
}
