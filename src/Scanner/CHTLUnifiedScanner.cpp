#include "CHTLUnifiedScanner.h"
#include <cctype>
#include <stdexcept>
#include <algorithm>

namespace CHTL {

CHTLUnifiedScanner::CHTLUnifiedScanner(const std::string& source)
    : source_(source),
      context_keywords_({
          // We only need to find top-level block keywords now.
          {ScannerContext::GENERAL, {"script", "style", "[Template]", "[Custom]", "[Origin]", "[Import]", "[Namespace]", "[Configuration]"}}
      }) {}

ScanResult CHTLUnifiedScanner::scan() {
    while (!isAtEnd()) {
        processGeneral();
    }
    // After the main loop, flush any remaining CHTL content.
    flushChunk(source_.length());
    return result_;
}

// --- Main Processing Logic ---

void CHTLUnifiedScanner::processGeneral() {
    size_t keyword_pos = findNextKeyword();

    if (keyword_pos == std::string::npos) {
        // No more global keywords, the rest of the file is one CHTL chunk.
        flushChunk(source_.length());
        cursor_ = source_.length();
        return;
    }

    // We found a keyword. Flush everything before it as CHTL.
    flushChunk(keyword_pos);
    cursor_ = keyword_pos;

    std::string keyword = identifyKeywordAt(cursor_);

    // Find the opening brace '{' that belongs to this keyword.
    size_t block_start = source_.find('{', cursor_);

    // Handle line-based constructs like [Import] that don't use braces.
    if (block_start == std::string::npos ||
        (keyword.rfind('[', 0) == 0 && source_.substr(cursor_ + keyword.length(), block_start - (cursor_ + keyword.length())).find('\n') != std::string::npos)) {
        size_t end_of_statement = source_.find('\n', cursor_);
        if (end_of_statement == std::string::npos) {
            end_of_statement = source_.length();
        }
        // These are just CHTL, so we let flushChunk handle them.
        // We just need to advance the cursor past them.
        cursor_ = end_of_statement;
        return;
    }

    // We found a block construct.

    // Find the end of this entire block.
    size_t block_end = findEndOfBlock('{', '}');
    if (block_end == std::string::npos) {
        throw std::runtime_error("Malformed block for keyword: " + keyword);
    }

    // Now, decide what to do with this block.
    FragmentType block_type = FragmentType::CHTL; // Default to CHTL
    if (keyword == "script") {
        // The entire script block is treated as one fragment.
        // The CHTL JS compiler will be responsible for separating JS and CHTL JS.
        block_type = FragmentType::JS;
    } else if (keyword == "style") {
        // The entire style block is a CSS fragment.
        block_type = FragmentType::CSS;
    }

    if (block_type == FragmentType::CHTL) {
        // For CHTL constructs like [Template], we don't create placeholders.
        // They are part of the main CHTL source. We just need to ensure the cursor
        // moves past them so we don't scan inside them.
        cursor_ = block_end;
    } else {
        // For non-CHTL blocks, create a placeholder.
        std::string placeholder = createPlaceholder(block_type);
        result_.chtl_with_placeholders += placeholder;

        // Advance the main cursor past the block and update the flush position.
        cursor_ = block_end;
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

std::string CHTLUnifiedScanner::identifyKeywordAt(size_t pos) {
    // This is simplified as we are only ever in the GENERAL context now.
    const auto& keywords = context_keywords_.at(ScannerContext::GENERAL);
    for (const auto& kw : keywords) {
        if (source_.rfind(kw, pos) == pos) {
            if (isalnum(kw[0])) { // Basic word boundary check
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
    while (temp_cursor < source_.length()) {
        char c = source_[temp_cursor];

        if (c == '"' || c == '\'' || c == '`') { // Skip strings
            char quote = c;
            temp_cursor++;
            while (temp_cursor < source_.length() && (source_[temp_cursor] != quote || source_[temp_cursor - 1] == '\\')) {
                temp_cursor++;
            }
        } else if (c == '/' && temp_cursor + 1 < source_.length()) { // Skip comments
            if (source_[temp_cursor + 1] == '/') { // single line comment
                temp_cursor += 2;
                while (temp_cursor < source_.length() && source_[temp_cursor] != '\n') {
                    temp_cursor++;
                }
            } else if (source_[temp_cursor + 1] == '*') { // multi-line comment
                temp_cursor += 2;
                while (temp_cursor + 1 < source_.length() && (source_[temp_cursor] != '*' || source_[temp_cursor+1] != '/')) {
                    temp_cursor++;
                }
                if (temp_cursor + 1 < source_.length()) temp_cursor++; // move past the second '/'
            }
        } else if (identifyKeywordAt(temp_cursor) != "") {
            return temp_cursor;
        }

        temp_cursor++;
    }
    return std::string::npos;
}

size_t CHTLUnifiedScanner::findEndOfBlock(char open_brace, char close_brace) {
    // This function now assumes the cursor is at the START of the block keyword.
    size_t start_pos = source_.find(open_brace, cursor_);
    if (start_pos == std::string::npos) return std::string::npos;

    int brace_level = 1;
    size_t temp_cursor = start_pos + 1;
    while (temp_cursor < source_.length()) {
        char c = source_[temp_cursor];

        if (c == '"' || c == '\'' || c == '`') { // Skip strings
            char quote = c;
            temp_cursor++;
            while (temp_cursor < source_.length() && (source_[temp_cursor] != quote || source_[temp_cursor - 1] == '\\')) {
                temp_cursor++;
            }
        } else if (c == '/' && temp_cursor + 1 < source_.length()) { // Skip comments
            if (source_[temp_cursor + 1] == '/') { // single line comment
                temp_cursor += 2;
                while (temp_cursor < source_.length() && source_[temp_cursor] != '\n') {
                    temp_cursor++;
                }
            } else if (source_[temp_cursor + 1] == '*') { // multi-line comment
                temp_cursor += 2;
                while (temp_cursor + 1 < source_.length() && (source_[temp_cursor] != '*' || source_[temp_cursor+1] != '/')) {
                    temp_cursor++;
                }
                if (temp_cursor + 1 < source_.length()) temp_cursor++; // move past the second '/'
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


void CHTLUnifiedScanner::flushChunk(size_t end) {
    if (last_flush_pos_ >= end) return;
    std::string content = source_.substr(last_flush_pos_, end - last_flush_pos_);
    if (!content.empty()) {
        result_.chtl_with_placeholders += content;
    }
    last_flush_pos_ = end;
}

std::string CHTLUnifiedScanner::createPlaceholder(FragmentType type) {
    std::string placeholder_key = "__CHTL_PLACEHOLDER_" + std::to_string(placeholder_id_++) + "__";

    // The content of the placeholder is the full block, including the keyword and braces.
    size_t block_end = findEndOfBlock('{', '}');
    std::string content = source_.substr(cursor_, block_end - cursor_);

    result_.placeholder_map[placeholder_key] = {content, type, placeholder_key};

    return placeholder_key;
}

// All old private methods that are no longer needed have been removed:
// - process()
// - processScript()
// - processStyle()
// - skipWhitespace()
// - findEndOfConstruct()

} // namespace CHTL
