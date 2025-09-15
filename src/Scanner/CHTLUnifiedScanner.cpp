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
          {ScannerContext::IN_STYLE, {"@Style", "&:", "#", "."}}
      }) {}

std::vector<CodeFragment> CHTLUnifiedScanner::scan() {
    while (!isAtEnd()) {
        process();
    }
    // After the main loop, there might be a final chunk of CHTL content.
    flushChunk(source_.length(), FragmentType::CHTL);
    return fragments_;
}

// --- Main Processing Logic ---

void CHTLUnifiedScanner::process() {
    switch (context_) {
        case ScannerContext::GENERAL:
            processGeneral();
            break;
        case ScannerContext::IN_SCRIPT:
            // This case should not be hit directly from the main loop,
            // as processGeneral now calls processScript with a boundary.
            // This is a safeguard.
            throw std::logic_error("processScript should be called directly by processGeneral.");
            break;
        case ScannerContext::IN_STYLE:
            processStyle();
            break;
    }
}

void CHTLUnifiedScanner::processGeneral() {
    size_t keyword_pos = findNextKeyword();

    if (keyword_pos == std::string::npos) {
        // No more global keywords, the rest of the file is considered one CHTL chunk.
        // This will be parsed by the CHTL parser which understands nested structures.
        flushChunk(source_.length(), FragmentType::CHTL);
        cursor_ = source_.length();
        return;
    }

    // We found a keyword. Flush everything before it as CHTL.
    flushChunk(keyword_pos, FragmentType::CHTL);
    cursor_ = keyword_pos;

    std::string keyword = identifyKeywordAt(cursor_);

    // Find the opening brace that belongs to this keyword.
    size_t block_start = source_.find('{', cursor_);

    // Handle line-based constructs like [Import]
    if (block_start == std::string::npos ||
        (keyword.rfind('[', 0) == 0 && source_.substr(cursor_ + keyword.length(), block_start - (cursor_ + keyword.length())).find('\n') != std::string::npos)) {
        size_t end_of_statement = source_.find('\n', cursor_);
        if (end_of_statement == std::string::npos) {
            end_of_statement = source_.length();
        }
        flushChunk(end_of_statement, FragmentType::CHTL);
        cursor_ = end_of_statement;
        last_flush_pos_ = cursor_;
        return;
    }

    // We found a block. Move the cursor inside it.
    cursor_ = block_start + 1;

    // Find the end of this entire block.
    size_t block_end = findEndOfBlock('{', '}');
    if (block_end == std::string::npos) {
        throw std::runtime_error("Malformed block for keyword: " + keyword);
    }

    // Now, decide what to do with this block.
    if (keyword == "script") {
        // The content of the script block needs to be processed by processScript.
        last_flush_pos_ = cursor_; // Mark the start of the script content.
        context_ = ScannerContext::IN_SCRIPT;
        // We call process() again, which will dispatch to processScript.
        // But processScript needs to know where to stop. We pass the boundary.
        processScript(block_end);
    } else if (keyword == "style") {
        // For now, we assume any top-level style block is global CSS.
        // The CHTL parser will be responsible for local styles.
        flushChunk(block_end, FragmentType::CSS);
        cursor_ = block_end;
        last_flush_pos_ = cursor_;
    } else {
        // For any other global CHTL construct ([Template], etc.), flush the whole thing as CHTL.
        flushChunk(block_end, FragmentType::CHTL);
        cursor_ = block_end;
        last_flush_pos_ = cursor_;
    }
}

// processScript now takes a boundary
void CHTLUnifiedScanner::processScript(size_t script_end_pos) {
    // This is the implementation from the previous step.
    // It scans within the script block for CHTL JS keywords.
    while (cursor_ < script_end_pos - 1) {
        size_t keyword_pos = findNextKeyword();

        if (keyword_pos == std::string::npos || keyword_pos >= script_end_pos - 1) {
            flushChunk(script_end_pos - 1, FragmentType::JS);
            last_flush_pos_ = cursor_ = script_end_pos - 1;
            break;
        }

        flushChunk(keyword_pos, FragmentType::JS);
        cursor_ = keyword_pos;

        std::string keyword = identifyKeywordAt(cursor_);
        size_t construct_end_pos = findEndOfConstruct(keyword);
        if (construct_end_pos == std::string::npos) {
            throw std::runtime_error("Malformed CHTL JS construct starting with: " + keyword);
        }

        flushChunk(construct_end_pos, FragmentType::CHTL_JS);
        cursor_ = construct_end_pos;
        last_flush_pos_ = cursor_;
    }

    // Move cursor past the script block's closing brace and switch context back.
    cursor_ = script_end_pos;
    last_flush_pos_ = cursor_;
    context_ = ScannerContext::GENERAL;
}

void CHTLUnifiedScanner::processStyle() {
    // This function is now only for top-level style blocks.
    // The logic from the previous step is sufficient.
    size_t end_pos = findEndOfBlock('{', '}');
    if (end_pos == std::string::npos) {
        throw std::runtime_error("Unmatched '{' in style block.");
    }

    flushChunk(end_pos - 1, FragmentType::CSS);
    cursor_ = end_pos;
    last_flush_pos_ = cursor_;
    context_ = ScannerContext::GENERAL;
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

void CHTLUnifiedScanner::skipWhitespace() {
    while (!isAtEnd() && isspace(peek())) {
        advance();
    }
}

std::string CHTLUnifiedScanner::identifyKeywordAt(size_t pos) {
    const auto& keywords = context_keywords_.at(context_);
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
        char c = source_[temp_cursor];
        if (c == '{') { // In general context, we don't scan for keywords inside blocks.
            if (context_ == ScannerContext::GENERAL) {
                int brace_level = 1;
                temp_cursor++;
                while(temp_cursor < source_.length() && brace_level > 0) {
                    if (source_[temp_cursor] == '{') brace_level++;
                    if (source_[temp_cursor] == '}') brace_level--;
                    temp_cursor++;
                }
                continue;
            }
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

size_t CHTLUnifiedScanner::findEndOfBlock(char open_brace, char close_brace) {
    // This function assumes the cursor is already *inside* the block.
    int brace_level = 1;
    size_t temp_cursor = cursor_;
    while (temp_cursor < source_.length()) {
        char c = source_[temp_cursor];

        // In the GENERAL context, we must not find keywords inside other blocks.
        // We simply skip the entire block.
        if (c == '{' && context_ == ScannerContext::GENERAL) {
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
        } else if (c == '/' && temp_cursor + 1 < source_.length()) { // Skip comments
            if (source_[temp_cursor + 1] == '/') { // single line comment
                temp_cursor += 2;
                while(temp_cursor < source_.length() && source_[temp_cursor] != '\n') {
                    temp_cursor++;
                }
                // continue without incrementing at the end
                continue;
            }
            if (source_[temp_cursor + 1] == '*') { // multi-line comment
                temp_cursor += 2;
                while(temp_cursor + 1 < source_.length() && (source_[temp_cursor] != '*' || source_[temp_cursor+1] != '/')) {
                    temp_cursor++;
                }
                if (temp_cursor + 1 < source_.length()) temp_cursor += 2;
                // continue without incrementing at the end
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


void CHTLUnifiedScanner::flushChunk(size_t end, FragmentType type) {
    if (last_flush_pos_ >= end) return;
    std::string content = source_.substr(last_flush_pos_, end - last_flush_pos_);
    if (!content.empty() && content.find_first_not_of(" \t\n\r\f\v") != std::string::npos) {
        fragments_.push_back({content, type, ""});
    }
    last_flush_pos_ = end;
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
    // For block constructs like listen, animate, delegate, router
    size_t block_start = source_.find('{', cursor_ + keyword.length());
    if (block_start == std::string::npos) return std::string::npos;

    // This is a duplication of the logic in findEndOfBlock. It is necessary
    // to give findEndOfConstruct its own brace-balancing logic that does not
    // interfere with the main scanner's cursor.
    int brace_level = 1;
    size_t temp_cursor = block_start + 1;
    while (temp_cursor < source_.length()) {
        char c = source_[temp_cursor];
        if (c == '"' || c == '\'' || c == '`') { // Skip strings
            char quote = c;
            temp_cursor++;
            while (temp_cursor < source_.length() && (source_[temp_cursor] != quote || source_[temp_cursor - 1] == '\\')) {
                temp_cursor++;
            }
        } else if (c == '{') {
            brace_level++;
        } else if (c == '}') {
            brace_level--;
            if (brace_level == 0) {
                return temp_cursor + 1; // Return position *after* the closing brace
            }
        }
        temp_cursor++;
    }
    return std::string::npos; // Unbalanced braces
}

std::string CHTLUnifiedScanner::createPlaceholder(const std::string& content, FragmentType type) { return ""; }
} // namespace CHTL
