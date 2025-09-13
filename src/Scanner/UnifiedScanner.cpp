#include "UnifiedScanner.h"
#include <stdexcept>
#include <vector>
#include <algorithm>
#include <sstream> // Changed from iostream to sstream for builder

namespace CHTL {

UnifiedScanner::UnifiedScanner(const std::string& source) : source_(source) {}

std::string UnifiedScanner::createPlaceholder() {
    return "_JS_P" + std::to_string(placeholder_counter_++) + "_";
}

void UnifiedScanner::skipWhitespace() {
    while (current_ < source_.length() && std::isspace(source_[current_])) {
        if (source_[current_] == '\n') line_++;
        current_++;
    }
}

// This helper is now used to find the end of a block from a specific starting point in the source
size_t UnifiedScanner::findBlockEnd(size_t start_offset) {
    size_t pos = start_offset;

    while(pos < source_.length() && std::isspace(source_[pos])) pos++;
    if (pos >= source_.length() || source_[pos] != '{') {
        return std::string::npos;
    }

    pos++;
    int brace_level = 1;

    while (pos < source_.length() && brace_level > 0) {
        if (source_[pos] == '/' && pos + 1 < source_.length()) {
            if (source_[pos + 1] == '/') { while(pos < source_.length() && source_[pos] != '\n') pos++; }
            else if (source_[pos + 1] == '*') {
                pos += 2;
                while(pos < source_.length() - 1 && !(source_[pos] == '*' && source_[pos + 1] == '/')) pos++;
                if (pos < source_.length()) pos++;
            }
        } else if (source_[pos] == '"' || source_[pos] == '\'') {
            char quote = source_[pos];
            pos++;
            while(pos < source_.length() && source_[pos] != quote) {
                if (pos == '\\' && pos + 1 < source_.length()) pos++;
                pos++;
            }
        } else if (source_[pos] == '{') {
            brace_level++;
        } else if (source_[pos] == '}') {
            brace_level--;
        }
        pos++;
    }

    if (brace_level > 0) throw std::runtime_error("Unmatched braces detected in source file.");
    return pos;
}

void UnifiedScanner::processScriptBlock(const std::string& content, CodeFragments& fragments) {
    std::stringstream chtl_js_builder;
    size_t last_pos = 0;
    const std::vector<std::string> patterns = {"{{", "}}", "->", "listen", "animate", "router", "delegate", "fileloader", "vir", "util"};

    while(last_pos < content.length()) {
        size_t nearest_pos = std::string::npos;
        std::string found_pattern;

        for(const auto& p : patterns) {
            size_t pos = content.find(p, last_pos);
            if (pos != std::string::npos && (pos < nearest_pos)) {
                nearest_pos = pos;
                found_pattern = p;
            }
        }

        if (nearest_pos == std::string::npos) break;

        if (nearest_pos > last_pos) {
            std::string js_chunk = content.substr(last_pos, nearest_pos - last_pos);
            std::string placeholder = createPlaceholder();
            fragments.js_placeholders[placeholder] = js_chunk;
            fragments.js.append(js_chunk);
            chtl_js_builder << placeholder;
        }

        size_t island_end = nearest_pos + found_pattern.length();
        if (found_pattern == "{{") {
            island_end = content.find("}}", nearest_pos);
            island_end = (island_end == std::string::npos) ? content.length() : island_end + 2;
        } else if (found_pattern != "->" && found_pattern != "}}") {
            size_t temp_pos = nearest_pos + found_pattern.length();
            while(temp_pos < content.length() && std::isspace(content[temp_pos])) temp_pos++;
            if (temp_pos < content.length() && content[temp_pos] == '{') {
                 int brace_level = 1;
                 island_end = temp_pos + 1;
                 while(island_end < content.length() && brace_level > 0) {
                     if(content[island_end] == '{') brace_level++;
                     if(content[island_end] == '}') brace_level--;
                     island_end++;
                 }
            }
        }

        chtl_js_builder << content.substr(nearest_pos, island_end - nearest_pos);
        last_pos = island_end;
    }

    if (last_pos < content.length()) {
        std::string js_chunk = content.substr(last_pos);
        std::string placeholder = createPlaceholder();
        fragments.js_placeholders[placeholder] = js_chunk;
        fragments.js.append(js_chunk);
        chtl_js_builder << placeholder;
    }

    fragments.chtl_js = chtl_js_builder.str();
}

CodeFragments UnifiedScanner::scan() {
    CodeFragments fragments;
    size_t last_append_pos = 0;
    int chtl_brace_level = 0;
    current_ = 0;

    while (current_ < source_.length()) {
        char c = source_[current_];

        // Skip comments and strings to not miscount braces
        if (c == '/' && current_ + 1 < source_.length()) {
            if (source_[current_ + 1] == '/') {
                current_ = source_.find('\n', current_);
                if (current_ == std::string::npos) current_ = source_.length();
                continue;
            } else if (source_[current_ + 1] == '*') {
                current_ = source_.find("*/", current_ + 2);
                if (current_ == std::string::npos) current_ = source_.length();
                else current_ += 2;
                continue;
            }
        } else if (c == '"' || c == '\'') {
            current_++;
            while (current_ < source_.length() && source_[current_] != c) {
                if (source_[current_] == '\\') current_++;
                current_++;
            }
            current_++;
            continue;
        }

        // Check for global style/script blocks
        bool is_style = source_.substr(current_, 5) == "style" && (current_ + 5 >= source_.length() || !std::isalnum(source_[current_ + 5]));
        bool is_script = source_.substr(current_, 6) == "script" && (current_ + 6 >= source_.length() || !std::isalnum(source_[current_ + 6]));

        if (chtl_brace_level == 0 && (is_style || is_script)) {
            fragments.chtl.append(source_.substr(last_append_pos, current_ - last_append_pos));

            size_t block_keyword_end = is_style ? current_ + 5 : current_ + 6;
            size_t block_end = findBlockEnd(block_keyword_end);

            if (block_end != std::string::npos) {
                if (is_style) {
                    fragments.css.append(source_.substr(current_, block_end - current_));
                    fragments.css.append("\n\n");
                } else {
                    size_t content_start = source_.find('{', block_keyword_end) + 1;
                    size_t content_end = block_end - 1;
                    std::string script_content = source_.substr(content_start, content_end - content_start);
                    processScriptBlock(script_content, fragments);
                }
                last_append_pos = block_end;
                current_ = block_end;
                continue;
            }
        }

        if (c == '{') {
            chtl_brace_level++;
        } else if (c == '}') {
            if (chtl_brace_level > 0) chtl_brace_level--;
        }

        current_++;
    }

    if (last_append_pos < source_.length()) {
        fragments.chtl.append(source_.substr(last_append_pos));
    }

    return fragments;
}

} // namespace CHTL
