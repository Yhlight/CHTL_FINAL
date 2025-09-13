#include "Scanner/CHTLUnifiedScanner.h"
#include <iostream>

namespace CHTL {

CHTLUnifiedScanner::CHTLUnifiedScanner(const std::string& source) : source_(source) {}

// Utility function to check if a substring at a given position is a standalone word
bool isStandaloneWord(const std::string& source, size_t pos, const std::string& word) {
    if (pos > 0 && (isalnum(source[pos - 1]) || source[pos - 1] == '_')) {
        return false; // Not at the beginning of a word
    }
    size_t end_pos = pos + word.length();
    if (end_pos < source.length() && (isalnum(source[end_pos]) || source[end_pos] == '_')) {
        return false; // Not at the end of a word
    }
    return true;
}

std::vector<CodeChunk> CHTLUnifiedScanner::scan() {
    process();
    return chunks_;
}

const std::map<std::string, std::string>& CHTLUnifiedScanner::getPlaceholderMap() const {
    return placeholder_map_;
}

void CHTLUnifiedScanner::process() {
    size_t last_pos = 0;

    while (current_ < source_.length()) {
        if (source_.substr(current_, 6) == "script" && isStandaloneWord(source_, current_, "script")) {
            if (current_ > last_pos) {
                chunks_.push_back({ChunkType::CHTL, source_.substr(last_pos, current_ - last_pos)});
            }
            handleScriptTag();
            last_pos = current_;
            continue;
        }

        if (source_.substr(current_, 5) == "style" && isStandaloneWord(source_, current_, "style")) {
            int brace_level = 0;
            for(size_t i = 0; i < current_; ++i) {
                 if (source_[i] == '{') brace_level++;
                 if (source_[i] == '}') { if (brace_level > 0) brace_level--; }
            }

            if (current_ > last_pos) {
                chunks_.push_back({ChunkType::CHTL, source_.substr(last_pos, current_ - last_pos)});
            }
            handleStyleTag(brace_level);
            last_pos = current_;
            continue;
        }

        current_++;
    }

    if (last_pos < source_.length()) {
        std::string final_chunk = source_.substr(last_pos);
        if (!final_chunk.empty()) {
            chunks_.push_back({ChunkType::CHTL, final_chunk});
        }
    }
}

// Forward declaration for the new method
std::string processScriptContent(
    const std::string& content,
    std::map<std::string, std::string>& placeholder_map,
    int& placeholder_id
);

void CHTLUnifiedScanner::handleScriptTag() {
    // Find the opening brace of the script block
    size_t brace_open_pos = source_.find('{', current_);
    if (brace_open_pos == std::string::npos) {
        current_++; // Malformed, skip
        return;
    }

    // Find the matching closing brace
    int brace_level = 1;
    size_t brace_close_pos = brace_open_pos + 1;
    while (brace_close_pos < source_.length() && brace_level > 0) {
        if (source_[brace_close_pos] == '{') brace_level++;
        else if (source_[brace_close_pos] == '}') brace_level--;
        brace_close_pos++;
    }

    if (brace_level != 0) {
        current_++; // Malformed, skip
        return;
    }

    // Extract and process the content
    std::string script_content = source_.substr(brace_open_pos + 1, brace_close_pos - brace_open_pos - 2);
    std::string processed_content = processScriptContent(script_content, placeholder_map_, placeholder_id_);
    chunks_.push_back({ChunkType::ChtlJs, processed_content});

    current_ = brace_close_pos;
}

std::string processScriptContent(
    const std::string& content,
    std::map<std::string, std::string>& placeholder_map,
    int& placeholder_id) {

    std::string result;
    size_t last_pos = 0;

    const std::vector<std::string> chtl_js_keywords = {
        "listen", "delegate", "animate", "router", "vir", "scriptloader", "{{", "iNeverAway", "util"
    };

    while (last_pos < content.length()) {
        size_t next_keyword_pos = std::string::npos;
        std::string found_keyword;

        // Find the earliest occurrence of any CHTL JS keyword
        for (const auto& keyword : chtl_js_keywords) {
            size_t pos = content.find(keyword, last_pos);
            if (pos != std::string::npos && (pos < next_keyword_pos || next_keyword_pos == std::string::npos)) {
                if (isStandaloneWord(content, pos, keyword) || keyword == "{{") {
                    next_keyword_pos = pos;
                    found_keyword = keyword;
                }
            }
        }

        if (next_keyword_pos == std::string::npos) {
            // No more CHTL JS keywords found, the rest is JS
            break;
        }

        // The content before the keyword is JS
        if (next_keyword_pos > last_pos) {
            std::string js_part = content.substr(last_pos, next_keyword_pos - last_pos);
            if (!js_part.empty()) {
                std::string placeholder = "_JS_PLACEHOLDER_" + std::to_string(placeholder_id++) + "_";
                placeholder_map[placeholder] = js_part;
                result += placeholder;
            }
        }

        // Find the end of the CHTL JS construct
        size_t construct_end_pos = 0;
        if (found_keyword == "{{") {
            construct_end_pos = content.find("}}", next_keyword_pos);
            if (construct_end_pos != std::string::npos) {
                construct_end_pos += 2; // include "}}"
            }
        } else if (found_keyword == "util") {
            // Special case for single-line util expressions
            construct_end_pos = content.find('\n', next_keyword_pos);
        }
        else {
            // For keywords like 'listen', 'animate', etc., find matching brace
            size_t brace_open_pos = content.find('{', next_keyword_pos);
            if (brace_open_pos != std::string::npos) {
                int brace_level = 1;
                size_t pos = brace_open_pos + 1;
                while (pos < content.length() && brace_level > 0) {
                    if (content[pos] == '{') brace_level++;
                    if (content[pos] == '}') brace_level--;
                    pos++;
                }
                construct_end_pos = pos;
            }
        }

        if (construct_end_pos == std::string::npos || construct_end_pos == 0) {
            // Malformed construct, treat the keyword as JS and continue scanning
            result += found_keyword;
            last_pos = next_keyword_pos + found_keyword.length();
            continue;
        }

        // Append the CHTL JS construct itself to the result
        result += content.substr(next_keyword_pos, construct_end_pos - next_keyword_pos);
        last_pos = construct_end_pos;
    }

    // Any remaining content is JS
    if (last_pos < content.length()) {
        std::string js_part = content.substr(last_pos);
        if (!js_part.empty()) {
            std::string placeholder = "_JS_PLACEHOLDER_" + std::to_string(placeholder_id++) + "_";
            placeholder_map[placeholder] = js_part;
            result += placeholder;
        }
    }

    return result;
}

void CHTLUnifiedScanner::handleStyleTag(int brace_level) {
    // Find the opening brace of the style block
    size_t brace_open_pos = source_.find('{', current_);
    if (brace_open_pos == std::string::npos) {
        current_++; // Malformed, skip
        return;
    }

    // Find the matching closing brace
    int style_brace_level = 1;
    size_t brace_close_pos = brace_open_pos + 1;
    while (brace_close_pos < source_.length() && style_brace_level > 0) {
        if (source_[brace_close_pos] == '{') style_brace_level++;
        else if (source_[brace_close_pos] == '}') style_brace_level--;
        brace_close_pos++;
    }

    if (style_brace_level != 0) {
        current_++; // Malformed, skip
        return;
    }

    // Extract the content of the style block
    std::string style_content = source_.substr(brace_open_pos + 1, brace_close_pos - brace_open_pos - 2);

    // Determine chunk type based on nesting level
    ChunkType type = (brace_level > 0) ? ChunkType::CHTL : ChunkType::Css;

    // For local style blocks, we need to wrap the content in "style { ... }"
    // because the CHTL parser expects the keyword.
    if (type == ChunkType::CHTL) {
        chunks_.push_back({type, "style { " + style_content + " }"});
    } else {
        chunks_.push_back({type, style_content});
    }

    current_ = brace_close_pos;
}

void CHTLUnifiedScanner::handleChtlBlock() {
    // This will be used for general CHTL block parsing if needed.
}

} // namespace CHTL
