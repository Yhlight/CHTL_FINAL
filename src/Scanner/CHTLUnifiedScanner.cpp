#include "CHTLUnifiedScanner.h"
#include <cctype>
#include <stdexcept>
#include <algorithm>

namespace CHTL {

CHTLUnifiedScanner::CHTLUnifiedScanner(const std::string& source)
    : source_(source),
      context_keywords_({
          {ScannerContext::GENERAL, {"script", "style", "[Template]", "[Custom]", "[Origin]", "[Import]", "[Namespace]", "[Configuration]"}},
          {ScannerContext::IN_SCRIPT, {"{{", "listen", "animate", "delegate", "vir", "router", "->"}},
          // Style keywords are less relevant now as style blocks are passed to the CHTL parser
      }) {}

ScanningResult CHTLUnifiedScanner::scan() {
    ScanningResult result;
    std::stringstream chtl_stream;

    while (!isAtEnd()) {
        // The core logic is now inlined here from the old `process` methods
        // for better control over the output stream.
        size_t next_pos = findNextKeyword();

        if (next_pos == std::string::npos) {
            // No more keywords, append the rest of the source as CHTL
            chtl_stream << source_.substr(cursor_);
            cursor_ = source_.length();
            break;
        }

        // Append the CHTL part before the keyword
        chtl_stream << source_.substr(cursor_, next_pos - cursor_);
        cursor_ = next_pos;

        std::string keyword = identifyKeywordAt(cursor_);

        // Find the block associated with the keyword
        size_t block_start = source_.find('{', cursor_);
        if (block_start == std::string::npos) {
             // Not a block-based keyword, treat as simple CHTL
             size_t end_of_line = source_.find('\n', cursor_);
             chtl_stream << source_.substr(cursor_, end_of_line - cursor_);
             cursor_ = end_of_line;
             continue;
        }

        size_t block_end = findEndOfBlock('{', '}');
        if (block_end == std::string::npos) {
            throw std::runtime_error("Malformed block for keyword: " + keyword);
        }

        // The content of the block is from after the '{' to before the '}'
        std::string block_content = source_.substr(block_start + 1, block_end - block_start - 2);

        if (keyword == "script") {
            // For script blocks, we create a placeholder for the entire raw content.
            // The CHTL parser will receive this placeholder. The JS/CHTL-JS content
            // will be processed later from the placeholder map.
            std::string placeholder = createPlaceholder(block_content, FragmentType::JS);
            result.placeholder_map[placeholder] = {block_content, FragmentType::JS, placeholder};
            chtl_stream << "{" << placeholder << "}";
        } else {
            // For any other CHTL construct (style, [Template], etc.), append the whole block
            // as-is, because the CHTL parser needs to see it.
            chtl_stream << source_.substr(cursor_, block_end - cursor_);
        }
        cursor_ = block_end;
    }

    result.chtl_with_placeholders = chtl_stream.str();
    return result;
}

// --- Main Processing Logic ---

// The old `process*` and `flushChunk` methods are removed as their logic
// is now incorporated into the new `scan` method.

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

void CHTLUnifiedScanner::skipWhitespace() {
    while (!isAtEnd() && isspace(peek())) {
        advance();
    }
}

std::string CHTLUnifiedScanner::identifyKeywordAt(size_t pos) {
    // We only need to identify "script" at the top level now.
    const auto& keywords = context_keywords_.at(ScannerContext::GENERAL);
    for (const auto& kw : keywords) {
        if (source_.rfind(kw, pos) == pos) {
            // Basic word boundary check
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
        // This simplified search only looks for top-level keywords.
        // It skips blocks and strings to avoid matching keywords inside them.
        char c = source_[temp_cursor];
        if (c == '{') {
            int brace_level = 1;
            temp_cursor++;
            while(temp_cursor < source_.length() && brace_level > 0) {
                if (source_[temp_cursor] == '{') brace_level++;
                if (source_[temp_cursor] == '}') brace_level--;
                temp_cursor++;
            }
            continue;
        }
        if (c == '"' || c == '\'' || c == '`') { // Skip strings
            char quote = c;
            temp_cursor++;
            while (temp_cursor < source_.length() && (source_[temp_cursor] != quote || source_[temp_cursor - 1] == '\\')) {
                temp_cursor++;
            }
            if(temp_cursor < source_.length()) temp_cursor++;
            continue;
        }
        if (c == '/' && temp_cursor + 1 < source_.length()) { // Skip comments
            if (source_[temp_cursor + 1] == '/') {
                temp_cursor += 2;
                while(temp_cursor < source_.length() && source_[temp_cursor] != '\n') temp_cursor++;
                continue;
            }
            if (source_[temp_cursor + 1] == '*') {
                temp_cursor += 2;
                while(temp_cursor + 1 < source_.length() && (source_[temp_cursor] != '*' || source_[temp_cursor+1] != '/')) temp_cursor++;
                if (temp_cursor + 1 < source_.length()) temp_cursor += 2;
                continue;
            }
        }

        // Check for top-level keywords at the current position.
        if (identifyKeywordAt(temp_cursor) != "") {
            return temp_cursor;
        }
        temp_cursor++;
    }
    return std::string::npos;
}

size_t CHTLUnifiedScanner::findEndOfBlock(char open_brace, char close_brace) {
    // This assumes the cursor is at the opening brace.
    // It correctly handles nested braces, strings, and comments.
    int brace_level = 0;
    size_t temp_cursor = cursor_;
    while (temp_cursor < source_.length()) {
        char c = source_[temp_cursor];
        if (c == '"' || c == '\'' || c == '`') {
            char quote = c;
            temp_cursor++;
            while (temp_cursor < source_.length() && (source_[temp_cursor] != quote || source_[temp_cursor - 1] == '\\')) temp_cursor++;
        } else if (c == '/' && temp_cursor + 1 < source_.length()) {
            if (source_[temp_cursor + 1] == '/') {
                temp_cursor += 2;
                while(temp_cursor < source_.length() && source_[temp_cursor] != '\n') temp_cursor++;
                continue;
            }
            if (source_[temp_cursor + 1] == '*') {
                temp_cursor += 2;
                while(temp_cursor + 1 < source_.length() && (source_[temp_cursor] != '*' || source_[temp_cursor+1] != '/')) temp_cursor++;
                if (temp_cursor + 1 < source_.length()) temp_cursor += 2;
                continue;
            }
        } else if (c == open_brace) {
            brace_level++;
        } else if (c == close_brace) {
            brace_level--;
            if (brace_level == 0) {
                return temp_cursor + 1; // Return position *after* the closing brace
            }
        }
        temp_cursor++;
    }
    return std::string::npos; // Unbalanced braces
}


// This helper is no longer needed as the logic is in scan()
// void CHTLUnifiedScanner::flushChunk(size_t end, FragmentType type) { ... }


// This helper is no longer needed as the logic is in scan()
// size_t CHTLUnifiedScanner::findEndOfConstruct(const std::string& keyword) { ... }

std::string CHTLUnifiedScanner::createPlaceholder(const std::string& content, FragmentType type) {
    // For now, we only create placeholders for script blocks.
    // The type could be used in the future to generate different kinds of placeholders.
    placeholder_id_++;
    return "__CHTL_SCRIPT_PLACEHOLDER_" + std::to_string(placeholder_id_) + "__";
}
} // namespace CHTL
