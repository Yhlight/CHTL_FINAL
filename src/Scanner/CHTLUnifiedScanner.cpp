#include "CHTLUnifiedScanner.h"
#include <stdexcept>
#include <cctype>
#include <algorithm>
#include <iostream>

namespace CHTL {

// Helper to check if a substring matches at a certain position
static bool startsWith(const std::string& haystack, const std::string& needle, size_t pos) {
    if (pos + needle.length() > haystack.length()) {
        return false;
    }
    return haystack.substr(pos, needle.length()) == needle;
}

// A robust static helper function to find a matching brace, aware of JS strings and comments.
static size_t find_matching_brace_static(const std::string& content, size_t start_pos) {
    if (start_pos >= content.length() || content[start_pos] != '{') {
        return std::string::npos;
    }

    int brace_level = 1;
    bool in_string = false;
    char string_char = '\0';

    for (size_t i = start_pos + 1; i < content.length(); ++i) {
        char current_char = content[i];
        char next_char = (i + 1 < content.length()) ? content[i + 1] : '\0';

        if (in_string) {
            if (current_char == '\\') { i++; continue; }
            if (current_char == string_char) { in_string = false; continue; }
            continue;
        }

        if (current_char == '"' || current_char == '\'' || current_char == '`') {
            in_string = true;
            string_char = current_char;
            continue;
        }

        if (current_char == '/' && next_char == '/') {
            size_t end_of_line = content.find('\n', i);
            i = (end_of_line == std::string::npos) ? content.length() : end_of_line;
            continue;
        }

        if (current_char == '/' && next_char == '*') {
            size_t end_of_comment = content.find("*/", i + 2);
            if (end_of_comment == std::string::npos) return std::string::npos;
            i = end_of_comment + 1;
            continue;
        }

        if (current_char == '{') {
            brace_level++;
        } else if (current_char == '}') {
            brace_level--;
            if (brace_level == 0) return i;
        }
    }
    return std::string::npos; // Unbalanced braces
}


// CHTL JS Keywords that signify the start of a CHTL JS construct
const std::vector<std::string> CHTLJS_KEYWORDS = {
    "{{", "Listen", "Delegate", "Animate", "Vir", "Router", "ScriptLoader"
};

CHTLUnifiedScanner::CHTLUnifiedScanner(const std::string& source)
    : source_(source) {}

ScannedData CHTLUnifiedScanner::scan() {
    process();
    return scanned_data_;
}

std::string CHTLUnifiedScanner::createJsPlaceholder(const std::string& content) {
    if (content.empty() || content.find_first_not_of(" \t\n\r") == std::string::npos) {
        return "";
    }
    std::string placeholder = "__JS_PLACEHOLDER_" + std::to_string(placeholder_id_++) + "__";
    scanned_data_.js_placeholder_map[placeholder] = content;
    return placeholder;
}

size_t CHTLUnifiedScanner::findEndOfBlock(size_t start_pos, char open_brace, char close_brace) {
    // This now uses the robust static helper, operating on the main source.
    return find_matching_brace_static(this->source_, start_pos);
}

size_t CHTLUnifiedScanner::findEndOfTag(size_t start_pos, const std::string& tag) {
    return source_.find(tag, start_pos);
}

bool CHTLUnifiedScanner::isChtlJsKeyword(const std::string& script_content, size_t pos) {
    for (const auto& keyword : CHTLJS_KEYWORDS) {
        if (startsWith(script_content, keyword, pos)) {
            if (keyword == "{{") return true;
            if (isalpha(keyword[0])) {
                if (pos > 0 && (isalnum(script_content[pos - 1]) || script_content[pos-1] == '.' || script_content[pos-1] == '>')) continue;
                if (pos + keyword.length() < script_content.length() && isalnum(script_content[pos + keyword.length()])) continue;
                return true;
            }
        }
    }
    return false;
}

size_t find_end_of_chtljs_construct(const std::string& content, size_t start_pos) {
    if (startsWith(content, "{{", start_pos)) {
        size_t end_pos = content.find("}}", start_pos);
        if (end_pos == std::string::npos) throw std::runtime_error("Unmatched '{{'");
        end_pos += 2;

        while (end_pos < content.length()) {
            size_t chain_start = end_pos;
            while (chain_start < content.length() && isspace(content[chain_start])) chain_start++;

            if (startsWith(content, "->", chain_start) || startsWith(content, "&->", chain_start)) {
                size_t op_end = chain_start + (startsWith(content, "->", chain_start) ? 2 : 3);
                size_t method_start = op_end;
                while (method_start < content.length() && isspace(content[method_start])) method_start++;
                size_t method_name_end = method_start;
                while(method_name_end < content.length() && isalnum(content[method_name_end])) method_name_end++;
                size_t arg_start = method_name_end;
                while (arg_start < content.length() && isspace(content[arg_start])) arg_start++;

                if (arg_start < content.length() && content[arg_start] == '{') {
                    size_t arg_end = find_matching_brace_static(content, arg_start);
                    if (arg_end == std::string::npos) throw std::runtime_error("Unmatched '{' in CHTL JS method call argument.");
                    end_pos = arg_end + 1;
                } else {
                    end_pos = method_name_end;
                }
            } else {
                break;
            }
        }
        return end_pos;
    }

    size_t block_start = content.find('{', start_pos);
    if (block_start == std::string::npos) throw std::runtime_error("Expected '{' after CHTL JS keyword.");

    size_t block_end = find_matching_brace_static(content, block_start);
    if (block_end == std::string::npos) throw std::runtime_error("Unmatched '{' in CHTL JS block.");
    return block_end + 1;
}

std::string CHTLUnifiedScanner::processScriptBlock(size_t block_start_pos) {
    size_t end_of_block = findEndOfBlock(block_start_pos, '{', '}');
    if (end_of_block == std::string::npos) {
        throw std::runtime_error("Unmatched '{' in script block.");
    }

    std::string script_content = source_.substr(block_start_pos + 1, end_of_block - block_start_pos - 1);
    std::stringstream result_stream;
    size_t cursor = 0;
    size_t last_flush_pos = 0;

    while (cursor < script_content.length()) {
        if (isChtlJsKeyword(script_content, cursor)) {
            result_stream << createJsPlaceholder(script_content.substr(last_flush_pos, cursor - last_flush_pos));
            size_t construct_start = cursor;
            size_t construct_end = find_end_of_chtljs_construct(script_content, construct_start);
            result_stream << script_content.substr(construct_start, construct_end - construct_start);
            cursor = construct_end;
            last_flush_pos = cursor;
        } else {
            cursor++;
        }
    }

    result_stream << createJsPlaceholder(script_content.substr(last_flush_pos));
    return result_stream.str();
}

void CHTLUnifiedScanner::processGlobalStyleBlock(size_t block_start_pos) {
    size_t end_tag_pos = findEndOfTag(block_start_pos, "</style>");
    if (end_tag_pos == std::string::npos) {
        throw std::runtime_error("Unmatched '<style>' tag.");
    }
    size_t content_start = source_.find('>', block_start_pos) + 1;
    std::string content = source_.substr(content_start, end_tag_pos - content_start);
    scanned_data_.css_fragments.push_back({content});
}

void CHTLUnifiedScanner::process() {
    std::stringstream chtl_source_stream;
    size_t cursor = 0;
    size_t last_flush_pos = 0;

    while (cursor < source_.length()) {
        if (startsWith(source_, "script", cursor) && source_.find('{', cursor) < source_.find('\n', cursor)) {
            size_t block_start = source_.find('{', cursor);
            if (block_start != std::string::npos) {
                chtl_source_stream << source_.substr(last_flush_pos, cursor - last_flush_pos);
                chtl_source_stream << "script ";
                std::string processed_script = processScriptBlock(block_start);
                chtl_source_stream << "{" << processed_script << "}";
                cursor = findEndOfBlock(block_start, '{', '}') + 1;
                last_flush_pos = cursor;
                continue;
            }
        }
        else if (startsWith(source_, "<style", cursor)) {
            chtl_source_stream << source_.substr(last_flush_pos, cursor - last_flush_pos);
            processGlobalStyleBlock(cursor);
            cursor = findEndOfTag(cursor, "</style>") + 8;
            last_flush_pos = cursor;
            continue;
        }
        cursor++;
    }

    if (last_flush_pos < source_.length()) {
        chtl_source_stream << source_.substr(last_flush_pos);
    }
    scanned_data_.chtl_source = chtl_source_stream.str();
}

} // namespace CHTL
