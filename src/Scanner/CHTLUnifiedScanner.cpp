#include "CHTLUnifiedScanner.h"
#include <cctype>
#include <stdexcept>
#include <algorithm>

namespace CHTL {

CHTLUnifiedScanner::CHTLUnifiedScanner(const std::string& source)
    : source_(source),
      keywords_({"{{", "listen", "animate", "delegate", "vir", "router"}) {}

ScanningResult CHTLUnifiedScanner::scan() {
    process();
    // Flush any remaining JS at the end
    if (last_flush_pos_ < source_.length()) {
        flushJsChunk(source_.length());
    }
    return {processed_content_.str(), placeholder_map_};
}

// --- Main Processing Logic ---
void CHTLUnifiedScanner::process() {
    while (cursor_ < source_.length()) {
        size_t keyword_pos = findNextKeyword();

        if (keyword_pos == std::string::npos) {
            // No more keywords, the rest is JS.
            break;
        }

        flushJsChunk(keyword_pos);
        cursor_ = keyword_pos;

        std::string keyword = identifyKeywordAt(cursor_);
        if (keyword.empty()) {
            advance();
            continue;
        }

        size_t end_of_construct = findEndOfConstruct(keyword);
        if (end_of_construct == std::string::npos) {
             throw std::runtime_error("Malformed CHTL JS construct: " + keyword);
        }

        // Append the CHTL JS part to the output and update cursors.
        processed_content_ << source_.substr(cursor_, end_of_construct - cursor_);
        cursor_ = end_of_construct;
        last_flush_pos_ = cursor_;
    }
}


// --- Helper Methods ---

char CHTLUnifiedScanner::peek(size_t offset) const {
    if (cursor_ + offset >= source_.length()) return '\0';
    return source_[cursor_ + offset];
}

bool CHTLUnifiedScanner::isAtEnd() const {
    return cursor_ >= source_.length();
}

void CHTLUnifiedScanner::advance(size_t n) {
    cursor_ += n;
}

void CHTLUnifiedScanner::skipIrrelevant() {
    // This is a placeholder
}

std::string CHTLUnifiedScanner::identifyKeywordAt(size_t pos) {
    for (const auto& kw : keywords_) {
        if (source_.rfind(kw, pos) == pos) {
            if (isalnum(kw[0])) {
                if (pos > 0 && isalnum(source_[pos - 1])) continue;
                if (pos + kw.length() < source_.length() && isalnum(source_[pos + kw.length()])) continue;
            }
            return kw;
        }
    }
    return "";
}

size_t CHTLUnifiedScanner::findNextKeyword() {
    size_t temp_cursor = cursor_;
    while(temp_cursor < source_.length()) {
        char c = source_[temp_cursor];
        if (c == '"' || c == '\'') { // Skip strings
            char quote = c;
            temp_cursor++;
            while (temp_cursor < source_.length() && (source_[temp_cursor] != quote || source_[temp_cursor-1] == '\\')) {
                temp_cursor++;
            }
            if(temp_cursor < source_.length()) temp_cursor++;
            continue;
        }
        if (c == '/' && temp_cursor + 1 < source_.length()) {
            if (source_[temp_cursor + 1] == '/') { // single line comment
                temp_cursor += 2;
                while(temp_cursor < source_.length() && source_[temp_cursor] != '\n') {
                    temp_cursor++;
                }
                continue;
            }
            if (source_[temp_cursor + 1] == '*') { // multi-line comment
                temp_cursor += 2;
                while(temp_cursor + 1 < source_.length() && (source_[temp_cursor] != '*' || source_[temp_cursor+1] != '/')) {
                    temp_cursor++;
                }
                if (temp_cursor + 1 < source_.length()) temp_cursor += 2;
                continue;
            }
        }

        if (identifyKeywordAt(temp_cursor) != "") {
            return temp_cursor;
        }
        temp_cursor++;
    }
    return std::string::npos;
}

size_t CHTLUnifiedScanner::findEndOfConstruct(const std::string& keyword) {
    if (keyword == "{{") {
        size_t end_pos = source_.find("}}", cursor_);
        return (end_pos != std::string::npos) ? end_pos + 2 : std::string::npos;
    }
    if (keyword == "vir") {
        size_t end_pos = source_.find(';', cursor_);
        return (end_pos != std::string::npos) ? end_pos + 1 : std::string::npos;
    }

    // For block constructs like listen, animate, etc.
    size_t start_brace = source_.find('{', cursor_);
    if (start_brace == std::string::npos) return std::string::npos;

    int brace_level = 1;
    size_t temp_cursor = start_brace + 1;
    while (temp_cursor < source_.length()) {
        char c = source_[temp_cursor];
        if (c == '"' || c == '\'') { // Skip strings
            char quote = c;
            temp_cursor++;
            while (temp_cursor < source_.length() && (source_[temp_cursor] != quote || source_[temp_cursor-1] == '\\')) {
                temp_cursor++;
            }
        } else if (c == '{') {
            brace_level++;
        } else if (c == '}') {
            brace_level--;
            if (brace_level == 0) return temp_cursor + 1;
        }
        temp_cursor++;
    }
    return std::string::npos;
}

void CHTLUnifiedScanner::flushJsChunk(size_t end) {
    if (last_flush_pos_ >= end) return;
    std::string chunk = source_.substr(last_flush_pos_, end - last_flush_pos_);
    if (chunk.find_first_not_of(" \t\n\r\f\v") != std::string::npos) {
        std::string placeholder = "__JS_PLACEHOLDER_" + std::to_string(placeholder_id_++) + "__";
        placeholder_map_[placeholder] = chunk;
        processed_content_ << placeholder;
    } else {
        processed_content_ << chunk;
    }
}

} // namespace CHTL
