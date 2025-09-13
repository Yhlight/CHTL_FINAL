#include "Scanner/CHTLUnifiedScanner.h"
#include <cctype>
#include <algorithm>

namespace CHTL {

// Helper to check if a substring at a given position is a whole word keyword
bool isKeyword(const std::string& source, size_t pos, const std::string& keyword) {
    if (pos + keyword.length() > source.length()) return false;
    if (source.substr(pos, keyword.length()) == keyword) {
        if (pos + keyword.length() == source.length() || !isalnum(source[pos + keyword.length()])) {
            return true;
        }
    }
    return false;
}

CHTLUnifiedScanner::CHTLUnifiedScanner(const std::string& source) : source_(source) {}

std::vector<CodeChunk> CHTLUnifiedScanner::scan() {
    process();
    return chunks_;
}

const std::map<std::string, std::string>& CHTLUnifiedScanner::getPlaceholderMap() const {
    return placeholder_map_;
}

void CHTLUnifiedScanner::skipString(const std::string& s, size_t& pos) {
    char quote_char = s[pos];
    pos++;
    while (pos < s.length() && s[pos] != quote_char) {
        if (s[pos] == '\\') {
            pos++;
        }
        pos++;
    }
}

size_t CHTLUnifiedScanner::findBlockEnd(size_t start_pos) {
    if (start_pos >= source_.length() || source_[start_pos] != '{') {
        return std::string::npos;
    }
    int brace_level = 1;
    size_t pos = start_pos + 1;
    while (pos < source_.length()) {
        char c = source_[pos];
        if (c == '\'' || c == '"') {
            skipString(source_, pos);
            if (pos < source_.length()) pos++;
            continue;
        }
        if (c == '/' && pos + 1 < source_.length()) {
            if (source_[pos+1] == '/') {
                pos = source_.find('\n', pos);
                if (pos == std::string::npos) pos = source_.length();
                continue;
            }
            if (source_[pos+1] == '*') {
                pos = source_.find("*/", pos + 2);
                if (pos == std::string::npos) pos = source_.length();
                else pos += 2;
                continue;
            }
        }
        if (c == '{') brace_level++;
        else if (c == '}') {
            brace_level--;
            if (brace_level == 0) return pos;
        }
        pos++;
    }
    return std::string::npos;
}

std::string CHTLUnifiedScanner::processScriptContent(size_t content_start, size_t content_end) {
    std::string processed_content;
    processed_content.reserve(content_end - content_start);
    size_t pos = content_start;
    while(pos < content_end) {
        char c = source_[pos];
        if (c == '{' && pos + 1 < content_end && source_[pos+1] == '{') {
             size_t end_marker = source_.find("}}", pos);
             if (end_marker != std::string::npos && end_marker < content_end) {
                 processed_content += source_.substr(pos, end_marker - pos + 2);
                 pos = end_marker + 2;
                 continue;
             }
        }
        if (c == '{') {
            size_t block_end = findBlockEnd(pos);
            if (block_end != std::string::npos && block_end < content_end) {
                std::string js_block = source_.substr(pos, block_end - pos + 1);
                std::string placeholder = "_JS_CODE_PLACEHOLDER_" + std::to_string(placeholder_id_++) + "_";
                placeholder_map_[placeholder] = js_block;
                processed_content += placeholder;
                pos = block_end + 1;
                continue;
            }
        }
        processed_content += c;
        pos++;
    }
    return processed_content;
}

void CHTLUnifiedScanner::handleBlock(size_t& last_pos, size_t keyword_pos, ChunkType type) {
    size_t temp_pos = keyword_pos + (type == ChunkType::Css ? 5 : 6);
    while (temp_pos < source_.length() && isspace(source_[temp_pos])) {
        temp_pos++;
    }

    if (temp_pos >= source_.length() || source_[temp_pos] != '{') {
        current_ = keyword_pos + 1; // Not a block, skip keyword
        return;
    }
    size_t brace_open_pos = temp_pos;

    size_t brace_close_pos = findBlockEnd(brace_open_pos);
    if (brace_close_pos == std::string::npos) {
        current_ = source_.length(); // Malformed, end processing
        return;
    }

    // Create CHTL chunk for content before this special block
    if (keyword_pos > last_pos) {
        chunks_.push_back({ChunkType::CHTL, source_.substr(last_pos, keyword_pos - last_pos)});
    }

    if (type == ChunkType::JavaScript) {
        std::string content = source_.substr(brace_open_pos + 1, brace_close_pos - (brace_open_pos + 1));
        std::string processed_script = processScriptContent(brace_open_pos + 1, brace_close_pos);
        chunks_.push_back({ChunkType::ChtlJs, processed_script});
        chunks_.push_back({ChunkType::JavaScript, content});
    } else { // CSS
        std::string content = source_.substr(brace_open_pos + 1, brace_close_pos - (brace_open_pos + 1));
        chunks_.push_back({ChunkType::Css, content});
    }

    current_ = brace_close_pos + 1;
    last_pos = current_;
}


void CHTLUnifiedScanner::process() {
    size_t last_pos = 0;
    current_ = 0;

    while (current_ < source_.length()) {
        size_t block_start = source_.find('{', current_);
        if (block_start == std::string::npos) {
            break;
        }

        size_t keyword_check_pos = block_start;
        while (keyword_check_pos > 0 && isspace(source_[keyword_check_pos - 1])) {
            keyword_check_pos--;
        }

        bool keyword_found = false;
        if (keyword_check_pos >= 6 && source_.substr(keyword_check_pos - 6, 6) == "script") {
            handleBlock(last_pos, keyword_check_pos - 6, ChunkType::JavaScript);
            keyword_found = true;
        } else if (keyword_check_pos >= 5 && source_.substr(keyword_check_pos - 5, 5) == "style") {
            handleBlock(last_pos, keyword_check_pos - 5, ChunkType::Css);
            keyword_found = true;
        }

        if (!keyword_found) {
            current_ = block_start + 1;
        }
    }

    if (last_pos < source_.length()) {
        chunks_.push_back({ChunkType::CHTL, source_.substr(last_pos)});
    }
}

} // namespace CHTL
