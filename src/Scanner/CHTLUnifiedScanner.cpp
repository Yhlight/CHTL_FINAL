#include "CHTLUnifiedScanner.h"
#include <cctype>
#include <stdexcept>

namespace CHTL {

// Helper to advance past whitespace
static void skipWhitespace(const std::string& source, size_t& pos) {
    while (pos < source.length() && isspace(source[pos])) {
        pos++;
    }
}

// Helper to check for a keyword at a specific position.
// This is a simplified version for finding 'script'.
static bool isKeyword(const std::string& source, size_t pos, const std::string& keyword) {
    if (pos + keyword.length() > source.length()) {
        return false;
    }
    // Check that it's not part of a larger word
    if (pos > 0 && (isalnum(source[pos - 1]) || source[pos-1] == '_')) {
        return false;
    }
    if (source.substr(pos, keyword.length()) != keyword) {
        return false;
    }
    size_t after_keyword = pos + keyword.length();
    if (after_keyword < source.length() && (isalnum(source[after_keyword]) || source[after_keyword] == '_')) {
        return false;
    }
    return true;
}

CHTLUnifiedScanner::CHTLUnifiedScanner(const std::string& source)
    : source_(source) {}

size_t CHTLUnifiedScanner::findEndOfBlock(size_t block_start_pos) {
    if (block_start_pos >= source_.length() || source_[block_start_pos] != '{') {
        return std::string::npos; // Should start with an opening brace
    }

    int brace_level = 1;
    size_t scan_pos = block_start_pos + 1;
    while (scan_pos < source_.length() && brace_level > 0) {
        char current_char = source_[scan_pos];
        if (current_char == '"' || current_char == '\'') { // Handle strings
            char quote_char = current_char;
            scan_pos++;
            while(scan_pos < source_.length() && source_[scan_pos] != quote_char) {
                if(source_[scan_pos] == '\\') { // skip escaped chars
                    scan_pos++;
                }
                scan_pos++;
            }
        } else if (current_char == '{') {
            brace_level++;
        } else if (current_char == '}') {
            brace_level--;
        }
        scan_pos++;
    }
    return (brace_level == 0) ? scan_pos : std::string::npos;
}


std::vector<CodeFragment> CHTLUnifiedScanner::scan() {
    std::vector<CodeFragment> fragments;
    size_t last_pos = 0;

    while (cursor_ < source_.length()) {
        // Find the next potential 'script' keyword
        size_t script_pos = source_.find("script", cursor_);
        if (script_pos == std::string::npos) {
            break; // No more script blocks
        }

        // Check if it's a valid keyword and not part of another identifier
        if (!isKeyword(source_, script_pos, "script")) {
            cursor_ = script_pos + 1;
            continue;
        }

        size_t block_start_finder = script_pos + 6; // start looking for '{' after "script"
        skipWhitespace(source_, block_start_finder);

        if (block_start_finder < source_.length() && source_[block_start_finder] == '{') {
            size_t block_end = findEndOfBlock(block_start_finder);
            if (block_end != std::string::npos) {
                // We found a script block.
                // 1. Add the preceding CHTL part as a fragment.
                if (script_pos > last_pos) {
                    fragments.push_back({FragmentType::CHTL, source_.substr(last_pos, script_pos - last_pos)});
                }

                // 2. Add the script content as a JS fragment.
                size_t content_start = block_start_finder + 1;
                size_t content_len = block_end - content_start - 1;
                fragments.push_back({FragmentType::JS, source_.substr(content_start, content_len)});

                // 3. Update cursors
                cursor_ = block_end;
                last_pos = cursor_;
            } else {
                // Malformed block, treat as CHTL and continue scanning
                cursor_ = block_start_finder;
            }
        } else {
            // "script" not followed by '{', treat as CHTL and continue scanning
            cursor_ = script_pos + 1;
        }
    }

    // Add any remaining CHTL content at the end of the file
    if (last_pos < source_.length()) {
        fragments.push_back({FragmentType::CHTL, source_.substr(last_pos)});
    }

    return fragments;
}

} // namespace CHTL
