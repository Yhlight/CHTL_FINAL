#include "Scanner/CHTLUnifiedScanner.h"
#include <cctype>
#include <stdexcept>
#include <algorithm> // For std::min

namespace CHTL {

// Helper function to check for a keyword
bool isKeyword(const std::string& source, size_t pos, const std::string& keyword) {
    if (pos + keyword.length() > source.length()) {
        return false;
    }
    if (keyword == "{{" || keyword == "}}") {
         return source.substr(pos, keyword.length()) == keyword;
    }
    if (pos > 0 && isalnum(source[pos - 1])) {
        return false;
    }
    if (source.substr(pos, keyword.length()) != keyword) {
        return false;
    }
    size_t after_keyword = pos + keyword.length();
    if (after_keyword < source.length() && isalnum(source[after_keyword])) {
        return false;
    }
    return true;
}

bool isBlockStart(const std::string& source, size_t pos, const std::string& keyword, size_t& brace_pos) {
    if (!isKeyword(source, pos, keyword)) {
        return false;
    }
    size_t keyword_end = pos + keyword.length();
    brace_pos = source.find('{', keyword_end);
    if (brace_pos == std::string::npos) {
        return false;
    }
    for (size_t i = keyword_end; i < brace_pos; ++i) {
        if (!isspace(source[i])) {
            return false;
        }
    }
    return true;
}

CHTLUnifiedScanner::CHTLUnifiedScanner(const std::string& source)
    : source_(source),
      chtljs_keywords_({"{{", "listen", "animate", "delegate", "vir", "router"}) {}

std::vector<CodeChunk> CHTLUnifiedScanner::scan() {
    process();
    return chunks_;
}

void CHTLUnifiedScanner::process() {
    size_t last_pos = 0;
    while (current_ < source_.length()) {
        size_t brace_pos = 0;
        if (isBlockStart(source_, current_, "script", brace_pos)) {
            if (current_ > last_pos) {
                chunks_.push_back({ChunkType::CHTL, source_.substr(last_pos, current_ - last_pos)});
            }
            handleScriptTag(brace_pos);
            last_pos = current_;
        } else if (isBlockStart(source_, current_, "style", brace_pos)) {
            if (current_ > last_pos) {
                chunks_.push_back({ChunkType::CHTL, source_.substr(last_pos, current_ - last_pos)});
            }
            handleStyleTag(brace_pos);
            last_pos = current_;
        } else {
            current_++;
        }
    }
    if (last_pos < source_.length()) {
        chunks_.push_back({ChunkType::CHTL, source_.substr(last_pos)});
    }
}

std::string CHTLUnifiedScanner::generatePlaceholder() {
    return "__JS_PLACEHOLDER_" + std::to_string(placeholder_id_++) + "__";
}

size_t CHTLUnifiedScanner::findEndOfChtlJsBlock(const std::string& content, size_t block_start_pos) {
    size_t brace_open_pos = content.find('{', block_start_pos);
    if (brace_open_pos == std::string::npos) return std::string::npos;
    int brace_level = 1;
    size_t scan_pos = brace_open_pos + 1;
    while (scan_pos < content.length() && brace_level > 0) {
        if (content[scan_pos] == '{') brace_level++;
        if (content[scan_pos] == '}') brace_level--;
        scan_pos++;
    }
    return (brace_level == 0) ? scan_pos : std::string::npos;
}

std::string CHTLUnifiedScanner::processScriptContent(const std::string& script_content) {
    std::string processed_content;
    size_t cursor = 0;

    while (cursor < script_content.length()) {
        size_t next_kw_pos = std::string::npos;
        std::string found_kw;

        for (const auto& kw : chtljs_keywords_) {
            size_t pos = script_content.find(kw, cursor);
            if (pos != std::string::npos && (next_kw_pos == std::string::npos || pos < next_kw_pos)) {
                if (isKeyword(script_content, pos, kw)) {
                    next_kw_pos = pos;
                    found_kw = kw;
                }
            }
        }

        if (next_kw_pos == std::string::npos) {
            std::string js_part = script_content.substr(cursor);
            if (!js_part.empty()) {
                std::string placeholder = generatePlaceholder();
                placeholder_map_[placeholder] = js_part;
                processed_content += placeholder;
            }
            break;
        }

        if (next_kw_pos > cursor) {
            std::string js_part = script_content.substr(cursor, next_kw_pos - cursor);
            std::string placeholder = generatePlaceholder();
            placeholder_map_[placeholder] = js_part;
            processed_content += placeholder;
        }

        if (found_kw == "{{") {
            size_t construct_end_pos = script_content.find("}}", next_kw_pos);
            if (construct_end_pos != std::string::npos) {
                construct_end_pos += 2;
                processed_content += script_content.substr(next_kw_pos, construct_end_pos - next_kw_pos);
                cursor = construct_end_pos;
            } else {
                 throw std::runtime_error("Unmatched {{ in script.");
            }
        } else { // Block constructs like listen, animate, etc.
            size_t brace_open_pos = script_content.find('{', next_kw_pos);
            size_t construct_end_pos = findEndOfChtlJsBlock(script_content, next_kw_pos);

            if (brace_open_pos == std::string::npos || construct_end_pos == std::string::npos) {
                throw std::runtime_error("Malformed CHTL JS block in script: " + found_kw);
            }

            // Append keyword and opening brace
            processed_content += script_content.substr(next_kw_pos, brace_open_pos - next_kw_pos + 1);

            // Recursively process content within the braces
            std::string inner_content = script_content.substr(brace_open_pos + 1, construct_end_pos - brace_open_pos - 2);
            processed_content += processScriptContent(inner_content);

            // Append closing brace
            processed_content += "}";
            cursor = construct_end_pos;
        }
    }
    return processed_content;
}


void CHTLUnifiedScanner::handleScriptTag(size_t brace_open_pos) {
    int brace_level = 1;
    size_t script_end_pos = brace_open_pos + 1;
    while (script_end_pos < source_.length() && brace_level > 0) {
        if (source_[script_end_pos] == '{') brace_level++;
        if (source_[script_end_pos] == '}') brace_level--;
        script_end_pos++;
    }
    if (brace_level != 0) {
        throw std::runtime_error("Unmatched brace in script block.");
    }

    std::string script_content = source_.substr(brace_open_pos + 1, script_end_pos - (brace_open_pos + 1) - 1);
    std::string processed_content = processScriptContent(script_content);

    chunks_.push_back({ChunkType::ChtlJs, processed_content});
    current_ = script_end_pos;
}

void CHTLUnifiedScanner::handleStyleTag(size_t brace_open_pos) {
    int brace_level = 1;
    current_ = brace_open_pos + 1;
    while (current_ < source_.length() && brace_level > 0) {
        if (source_[current_] == '{') brace_level++;
        if (source_[current_] == '}') brace_level--;
        current_++;
    }
    if (brace_level == 0) {
        std::string content = source_.substr(brace_open_pos + 1, current_ - (brace_open_pos + 1) - 1);
        chunks_.push_back({ChunkType::Css, content});
    } else {
        throw std::runtime_error("Unmatched brace in style block.");
    }
}

} // namespace CHTL
