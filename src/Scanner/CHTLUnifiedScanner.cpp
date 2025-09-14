#include "CHTLUnifiedScanner.h"
#include <stdexcept>
#include <cctype>
#include <vector>
#include <algorithm>

namespace CHTL {

CHTLUnifiedScanner::CHTLUnifiedScanner(const std::string& source) : source_(source) {}

std::vector<CodeChunk> CHTLUnifiedScanner::scan() {
    if (source_.empty()) {
        return {};
    }
    scanSource();
    return chunks_;
}

const std::map<std::string, std::string>& CHTLUnifiedScanner::getPlaceholderMap() const {
    return placeholder_map_;
}

void CHTLUnifiedScanner::scanSource() {
    while (!isAtEnd()) {
        start_ = current_;
        scanChtl();
    }
}

void CHTLUnifiedScanner::scanChtl() {
    size_t next_script = source_.find("script", current_);
    size_t next_style = source_.find("style", current_);

    size_t next_block = std::min(next_script, next_style);

    if (next_block == std::string::npos) {
        // No more blocks, the rest is CHTL
        addChunk(ChunkType::CHTL, source_.substr(current_));
        current_ = source_.length();
        return;
    }

    // Add the CHTL part before the block
    if (next_block > current_) {
        addChunk(ChunkType::CHTL, source_.substr(current_, next_block - current_));
    }

    current_ = next_block;

    if (current_ == next_script) {
        scanScriptBlock();
    } else if (current_ == next_style) {
        scanStyleBlock();
    }
}

void CHTLUnifiedScanner::scanScriptBlock() {
    size_t keyword_end = current_ + 6; // "script"

    size_t opening_brace = source_.find('{', keyword_end);
    if (opening_brace == std::string::npos) {
        addChunk(ChunkType::CHTL, "script"); // Not a valid block
        current_ = keyword_end;
        return;
    }

    // Check for only whitespace between keyword and brace
    for (size_t i = keyword_end; i < opening_brace; ++i) {
        if (!isspace(source_[i])) {
            // Not a script block, treat as CHTL and continue scanning
            addChunk(ChunkType::CHTL, source_.substr(current_, opening_brace - current_));
            current_ = opening_brace;
            return;
        }
    }

    int brace_level = 1;
    size_t closing_brace = opening_brace + 1;
    while (closing_brace < source_.length() && brace_level > 0) {
        if (source_[closing_brace] == '{') brace_level++;
        else if (source_[closing_brace] == '}') brace_level--;
        closing_brace++;
    }

    if (brace_level != 0) {
        throw std::runtime_error("Mismatched braces in script block.");
    }

    addChunk(ChunkType::CHTL, source_.substr(current_, opening_brace - current_ + 1));
    scanJavaScriptContent(opening_brace + 1, closing_brace - 1);
    addChunk(ChunkType::CHTL, "}");

    current_ = closing_brace;
}

void CHTLUnifiedScanner::scanStyleBlock() {
    // For now, treat style blocks as pure CHTL as per spec for global style
    // and the CHTL compiler handles local style blocks.
    size_t opening_brace = source_.find('{', current_);
    if (opening_brace == std::string::npos) {
        addChunk(ChunkType::CHTL, "style");
        current_ += 5;
        return;
    }

    int brace_level = 1;
    size_t closing_brace = opening_brace + 1;
    while (closing_brace < source_.length() && brace_level > 0) {
        if (source_[closing_brace] == '{') brace_level++;
        else if (source_[closing_brace] == '}') brace_level--;
        closing_brace++;
    }

    if (brace_level != 0) {
        throw std::runtime_error("Mismatched braces in style block.");
    }

    addChunk(ChunkType::CHTL, source_.substr(current_, closing_brace - current_));
    current_ = closing_brace;
}

// Scans the content of a script block.
// This method implements the core "placeholder" logic. It scans for CHTL JS
// constructs and wraps any surrounding plain JavaScript code in placeholders.
//
// NOTE: This implementation performs a top-level scan of the script content.
// It does not currently recurse into CHTL JS blocks (e.g., the arrow function
// within a `listen` block). A more advanced implementation would be required
// to parse the CHTL JS structures themselves to isolate nested plain JS.
// However, this implementation correctly handles the primary separation task.
void CHTLUnifiedScanner::scanJavaScriptContent(size_t block_start, size_t block_end) {
    size_t local_pos = block_start;
    size_t last_pos = block_start;

    while (local_pos < block_end) {
        size_t selector_pos = findEnhancedSelector(local_pos);
        size_t keyword_pos = std::string::npos;

        // Find the earliest CHTL JS keyword
        for (const auto& keyword : {"listen", "delegate", "animate", "vir", "router"}) {
            size_t pos = source_.find(keyword, local_pos);
            if (pos != std::string::npos && isChtlJsKeyword(pos)) {
                if (keyword_pos == std::string::npos || pos < keyword_pos) {
                    keyword_pos = pos;
                }
            }
        }

        size_t next_chtljs_pos = std::min(selector_pos, keyword_pos);

        if (next_chtljs_pos < block_end) {
            if (next_chtljs_pos > last_pos) {
                std::string js_part = source_.substr(last_pos, next_chtljs_pos - last_pos);
                addChunk(ChunkType::Placeholder, createPlaceholder(js_part));
            }

            size_t chtljs_end;
            if (next_chtljs_pos == selector_pos) {
                // It's an enhanced selector {{...}}
                size_t end_marker = source_.find("}}", next_chtljs_pos);
                if (end_marker == std::string::npos || end_marker >= block_end) {
                    // Malformed, consume the '{{' as CHTL JS and continue
                    chtljs_end = next_chtljs_pos + 2;
                } else {
                    chtljs_end = end_marker + 2;
                }
            } else {
                // It's a CHTL JS keyword block like listen {...}
                chtljs_end = findEndOfChtlJsBlock(next_chtljs_pos);
            }

            addChunk(ChunkType::ChtlJs, source_.substr(next_chtljs_pos, chtljs_end - next_chtljs_pos));
            local_pos = chtljs_end;
            last_pos = local_pos;
        } else {
            break; // No more CHTL JS found
        }
    }

    if (last_pos < block_end) {
        std::string js_part = source_.substr(last_pos, block_end - last_pos);
        addChunk(ChunkType::Placeholder, createPlaceholder(js_part));
    }
}

bool CHTLUnifiedScanner::isChtlJsKeyword(size_t position) {
    const std::vector<std::string> keywords = {"listen", "delegate", "animate", "vir", "router"};
    for (const auto& keyword : keywords) {
        if (source_.substr(position, keyword.length()) == keyword) {
            size_t end_pos = position + keyword.length();
            if (end_pos >= source_.length() || !isalnum(source_[end_pos])) {
                return true;
            }
        }
    }
    return false;
}

size_t CHTLUnifiedScanner::findEndOfChtlJsBlock(size_t position) {
    size_t brace_pos = source_.find('{', position);
    if (brace_pos == std::string::npos) return position + 1;

    int brace_level = 1;
    size_t end_pos = brace_pos + 1;
    while (end_pos < source_.length() && brace_level > 0) {
        if (source_[end_pos] == '{') brace_level++;
        else if (source_[end_pos] == '}') brace_level--;
        end_pos++;
    }
    return end_pos;
}

size_t CHTLUnifiedScanner::findEnhancedSelector(size_t position) {
    return source_.find("{{", position);
}

std::string CHTLUnifiedScanner::createPlaceholder(const std::string& content) {
    if (content.empty()) return "";
    std::string placeholder = "_JS_CODE_PLACEHOLDER_" + std::to_string(placeholder_id_++) + "_";
    placeholder_map_[placeholder] = content;
    return placeholder;
}

void CHTLUnifiedScanner::addChunk(ChunkType type, const std::string& content) {
    if (!content.empty()) {
        chunks_.push_back({type, content});
    }
}

char CHTLUnifiedScanner::peek() {
    if (isAtEnd()) return '\0';
    return source_[current_];
}

char CHTLUnifiedScanner::advance() {
    if (!isAtEnd()) current_++;
    return source_[current_ - 1];
}

bool CHTLUnifiedScanner::isAtEnd() {
    return current_ >= source_.length();
}

bool CHTLUnifiedScanner::match(const std::string& expected) {
    // This helper is not used in the current logic, but good to keep.
    return false;
}

void CHTLUnifiedScanner::skipWhitespace() {
    while (!isAtEnd() && isspace(peek())) {
        advance();
    }
}

} // namespace CHTL
