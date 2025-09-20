#include "CHTLUnifiedScanner.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include <sstream>

namespace CHTL {

// Helper function to find the position of a matching brace
size_t find_matching_brace(const std::string& content, size_t start_pos) {
    if (start_pos >= content.length() || content[start_pos] != '{') {
        return std::string::npos;
    }
    int brace_level = 1;
    for (size_t i = start_pos + 1; i < content.length(); ++i) {
        if (content[i] == '{') {
            brace_level++;
        } else if (content[i] == '}') {
            brace_level--;
            if (brace_level == 0) {
                return i;
            }
        }
    }
    return std::string::npos; // Not found
}


CHTLUnifiedScanner::CHTLUnifiedScanner(const std::string& source)
    : source(source) {}

const std::map<std::string, std::string>& CHTLUnifiedScanner::getPlaceholderMap() const {
    return placeholder_map;
}

std::string CHTLUnifiedScanner::addPlaceholder(const std::string& content, bool is_style) {
    if (content.empty()) {
        return "";
    }
    std::string key;
    std::string map_key = "_PLACEHOLDER_" + std::to_string(placeholder_counter) + "_";

    if (is_style) {
        key = "/* " + map_key + " */";
    } else {
        key = map_key;
    }
    placeholder_map["_PLACEHOLDER_" + std::to_string(placeholder_counter++) + "_"] = content;
    return key;
}

std::string CHTLUnifiedScanner::processScript(const std::string& content) {
    std::string result;
    size_t last_pos = 0;

    const std::vector<std::string> keywords = {"Listen", "Delegate", "Animate", "Vir", "Router", "printMylove", "iNeverAway"};

    while (last_pos < content.length()) {
        size_t next_construct_pos = std::string::npos;

        // 1. Check for '{{'
        size_t brace_pos = content.find("{{", last_pos);
        if (brace_pos != std::string::npos) {
            next_construct_pos = brace_pos;
        }

        // 2. Check for '$'
        size_t dollar_pos = content.find('$', last_pos);
        if (dollar_pos != std::string::npos && (next_construct_pos == std::string::npos || dollar_pos < next_construct_pos)) {
            next_construct_pos = dollar_pos;
        }

        // 3. Check for keywords
        for (const auto& keyword : keywords) {
            size_t keyword_pos = content.find(keyword, last_pos);
            if (keyword_pos != std::string::npos && (next_construct_pos == std::string::npos || keyword_pos < next_construct_pos)) {
                next_construct_pos = keyword_pos;
            }
        }

        if (next_construct_pos == std::string::npos) {
            result += addPlaceholder(content.substr(last_pos), false);
            break;
        }

        result += addPlaceholder(content.substr(last_pos, next_construct_pos - last_pos), false);

        size_t construct_len = 0;
        if (content.substr(next_construct_pos, 2) == "{{") {
            size_t end_brace = content.find("}}", next_construct_pos);
            construct_len = (end_brace == std::string::npos) ? content.length() - next_construct_pos : end_brace - next_construct_pos + 2;
        } else if (content[next_construct_pos] == '$') {
            size_t end_dollar = content.find('$', next_construct_pos + 1);
            construct_len = (end_dollar == std::string::npos) ? content.length() - next_construct_pos : end_dollar - next_construct_pos + 1;
        } else {
            size_t open_brace = content.find('{', next_construct_pos);
            size_t end_brace = (open_brace == std::string::npos) ? std::string::npos : find_matching_brace(content, open_brace);
            construct_len = (end_brace == std::string::npos) ? content.length() - next_construct_pos : end_brace - next_construct_pos + 1;
        }

        result += content.substr(next_construct_pos, construct_len);
        last_pos = next_construct_pos + construct_len;
    }

    return result;
}

std::string CHTLUnifiedScanner::processStyle(const std::string& content) {
    std::string result;
    std::string current_line;
    std::istringstream stream(content);
    const std::vector<std::string> triggers = {" + ", " - ", " * ", " / ", " ? ", "@Style", "@Var", "ThemeColor("};

    while (std::getline(stream, current_line)) {
        bool has_trigger = false;
        for (const auto& trigger : triggers) {
            if (current_line.find(trigger) != std::string::npos) {
                has_trigger = true;
                break;
            }
        }

        if (has_trigger) {
            result += addPlaceholder(current_line, true) + "\n";
        } else {
            result += current_line + "\n";
        }
    }
    return result;
}

std::vector<CodeFragment> CHTLUnifiedScanner::scan() {
    std::vector<CodeFragment> fragments;
    size_t last_pos = 0;

    while (last_pos < source.length()) {
        size_t script_pos = source.find("script", last_pos);
        size_t style_pos = source.find("style", last_pos);

        size_t next_block_pos = std::min(script_pos, style_pos);

        if (next_block_pos == std::string::npos) {
            if (last_pos < source.length()) {
                std::string final_chunk = source.substr(last_pos);
                // Trim whitespace from the beginning and end of the final chunk
                size_t first = final_chunk.find_first_not_of(" \t\n\r");
                if (std::string::npos != first) {
                    size_t last = final_chunk.find_last_not_of(" \t\n\r");
                    std::string trimmed_chunk = final_chunk.substr(first, (last - first + 1));
                    if (!trimmed_chunk.empty()) {
                        fragments.push_back({FragmentType::CHTL, trimmed_chunk, line});
                    }
                }
            }
            break;
        }

        if (next_block_pos > last_pos) {
            fragments.push_back({FragmentType::CHTL, source.substr(last_pos, next_block_pos - last_pos), line});
        }

        size_t brace_open_pos = source.find('{', next_block_pos);
        if (brace_open_pos == std::string::npos) {
            fragments.push_back({FragmentType::CHTL, source.substr(next_block_pos), line});
            break;
        }

        size_t brace_close_pos = find_matching_brace(source, brace_open_pos);
        if (brace_close_pos == std::string::npos) {
            fragments.push_back({FragmentType::CHTL, source.substr(next_block_pos), line});
            break;
        }

        std::string content = source.substr(brace_open_pos + 1, brace_close_pos - brace_open_pos - 1);

        if (next_block_pos == script_pos) {
            fragments.push_back({FragmentType::JS, processScript(content), line});
        } else {
            fragments.push_back({FragmentType::CSS, processStyle(content), line});
        }

        last_pos = brace_close_pos + 1;
    }

    return fragments;
}

// Helper method implementations (if any are kept)
bool CHTLUnifiedScanner::isAtEnd() {
    return current >= source.length();
}

char CHTLUnifiedScanner::advance() {
    current++;
    if (source[current - 1] == '\n') line++;
    return source[current - 1];
}

char CHTLUnifiedScanner::peek() {
    if (isAtEnd()) return '\0';
    return source[current];
}

} // namespace CHTL
