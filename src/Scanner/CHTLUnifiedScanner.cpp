#include "Scanner/CHTLUnifiedScanner.h"
#include <cctype>

namespace CHTL {

namespace {
// Helper function to find the end of a balanced brace block
size_t find_matching_brace(const std::string& s, size_t start_pos) {
    if (start_pos >= s.length() || s[start_pos] != '{') {
        return std::string::npos;
    }
    int balance = 1;
    size_t pos = start_pos + 1;
    while (pos < s.length()) {
        if (s[pos] == '{') {
            balance++;
        } else if (s[pos] == '}') {
            balance--;
        }
        if (balance == 0) {
            return pos;
        }
        pos++;
    }
    return std::string::npos; // Not found
}

// Helper function to check if a keyword is a standalone word
bool is_standalone_word(const std::string& source, size_t pos, size_t len) {
    // Check before
    if (pos > 0 && (isalnum(source[pos - 1]) || source[pos - 1] == '_')) {
        return false;
    }
    // Check after
    if (pos + len < source.length() && (isalnum(source[pos + len]) || source[pos + len] == '_')) {
        return false;
    }
    return true;
}

// Helper function to check for only whitespace between keyword and brace
bool is_whitespace_between(const std::string& source, size_t start, size_t end) {
    for(size_t i = start; i < end; ++i) {
        if (!isspace(source[i])) {
            return false;
        }
    }
    return true;
}
} // anonymous namespace

CHTLUnifiedScanner::CHTLUnifiedScanner(const std::string& source) : source_(source) {}

std::vector<CodeChunk> CHTLUnifiedScanner::scan() {
    process();
    return chunks_;
}

void CHTLUnifiedScanner::process() {
    size_t last_pos = 0;
    current_ = 0;

    while (current_ < source_.length()) {
        // Find the next `{` to see if it's preceded by a keyword
        size_t brace_pos = source_.find('{', current_);
        if (brace_pos == std::string::npos) {
            // No more blocks, we're done
            break;
        }

        // Backtrack from the brace to find the keyword
        size_t keyword_start = source_.rfind("style", brace_pos);
        if (keyword_start != std::string::npos && is_standalone_word(source_, keyword_start, 5) && is_whitespace_between(source_, keyword_start + 5, brace_pos)) {
            // Found a style block
            if (keyword_start > last_pos) {
                chunks_.push_back({ChunkType::CHTL, source_.substr(last_pos, keyword_start - last_pos)});
            }
            current_ = keyword_start;
            handleStyleTag();
            last_pos = current_;
            continue;
        }

        keyword_start = source_.rfind("script", brace_pos);
        if (keyword_start != std::string::npos && is_standalone_word(source_, keyword_start, 6) && is_whitespace_between(source_, keyword_start + 6, brace_pos)) {
             // Found a script block
            if (keyword_start > last_pos) {
                chunks_.push_back({ChunkType::CHTL, source_.substr(last_pos, keyword_start - last_pos)});
            }
            current_ = keyword_start;
            handleScriptTag();
            last_pos = current_;
            continue;
        }

        // If we got here, the `{` was not for a script or style block, so just continue scanning
        current_ = brace_pos + 1;
    }

    // Add the final remaining CHTL chunk
    if (last_pos < source_.length()) {
        chunks_.push_back({ChunkType::CHTL, source_.substr(last_pos)});
    }
}


void CHTLUnifiedScanner::handleScriptTag() {
    size_t brace_start = source_.find('{', current_);
    size_t block_end = find_matching_brace(source_, brace_start);

    if (block_end == std::string::npos) {
        current_ += 6;
        return;
    }

    std::string script_content = source_.substr(brace_start + 1, block_end - brace_start - 1);
    std::string processed_content;
    size_t last_pos = 0;

    while (last_pos < script_content.length()) {
        size_t chtl_js_start = script_content.find("{{", last_pos);

        if (chtl_js_start == std::string::npos) {
            break;
        }

        if (chtl_js_start > last_pos) {
            std::string js_part = script_content.substr(last_pos, chtl_js_start - last_pos);
            std::string placeholder = "_JS_PLACEHOLDER_" + std::to_string(placeholder_id_++) + "_";
            placeholder_map_[placeholder] = js_part;
            processed_content += placeholder;
        }

        size_t chtl_js_end = script_content.find("}}", chtl_js_start);
        if (chtl_js_end == std::string::npos) {
            last_pos = chtl_js_start;
            break;
        }
        chtl_js_end += 2;

        // Greedily consume the rest of the CHTL JS expression
        while (chtl_js_end < script_content.length()) {
            char c = script_content[chtl_js_end];
            if (isspace(c)) { // Stop at whitespace
                break;
            }
            // Consuming a whole parenthesis block
            if (c == '(') {
                // This is a bit of a hack, we should have a proper parenthesis matcher
                size_t paren_end = script_content.find(')', chtl_js_end);
                if(paren_end != std::string::npos) {
                    chtl_js_end = paren_end + 1;
                    continue;
                }
            }

            if (isalnum(c) || c == '_' || c == '.' || c == ';' || c == '[' || c == ']') {
                 chtl_js_end++;
                 continue;
            }
            if (c == '-' && chtl_js_end + 1 < script_content.length() && script_content[chtl_js_end + 1] == '>') {
                chtl_js_end += 2;
                continue;
            }
            break;
        }

        processed_content += script_content.substr(chtl_js_start, chtl_js_end - chtl_js_start);
        last_pos = chtl_js_end;
    }

    if (last_pos < script_content.length()) {
        std::string js_part = script_content.substr(last_pos);
        if (!js_part.empty()) {
            std::string placeholder = "_JS_PLACEHOLDER_" + std::to_string(placeholder_id_++) + "_";
            placeholder_map_[placeholder] = js_part;
            processed_content += placeholder;
        }
    }

    if (!processed_content.empty()) {
        chunks_.push_back({ChunkType::ChtlJs, processed_content});
    }

    current_ = block_end + 1;
}

void CHTLUnifiedScanner::handleStyleTag() {
    size_t brace_start = source_.find('{', current_);
    size_t block_end = find_matching_brace(source_, brace_start);

    if (block_end != std::string::npos) {
        std::string content = source_.substr(brace_start + 1, block_end - brace_start - 1);
        chunks_.push_back({ChunkType::Css, content});
        current_ = block_end + 1;
    } else {
        // Unterminated block, just advance past the keyword to avoid infinite loop
        current_ += 5;
    }
}

const std::map<std::string, std::string>& CHTLUnifiedScanner::getPlaceholderMap() const {
    return placeholder_map_;
}

} // namespace CHTL
