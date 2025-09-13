#include "Scanner/CHTLUnifiedScanner.h"

namespace CHTL {

CHTLUnifiedScanner::CHTLUnifiedScanner(const std::string& source) : source_(source) {}

std::vector<CodeChunk> CHTLUnifiedScanner::scan() {
    size_t last_pos = 0;
    size_t current_pos = 0;

    while ((current_pos = source_.find("script", last_pos)) != std::string::npos) {
        // Find the opening brace after "script"
        size_t brace_open_pos = source_.find('{', current_pos + 6);
        if (brace_open_pos == std::string::npos) {
            break; // No opening brace found, assume rest is CHTL
        }

        // Check if there is non-whitespace between "script" and "{"
        bool non_whitespace_found = false;
        for (size_t i = current_pos + 6; i < brace_open_pos; ++i) {
            if (!isspace(source_[i])) {
                non_whitespace_found = true;
                break;
            }
        }
        if (non_whitespace_found) {
            last_pos = current_pos + 1;
            continue;
        }

        // Add the CHTL part before the script block
        if (current_pos > last_pos) {
            chunks_.push_back({ChunkType::CHTL, source_.substr(last_pos, current_pos - last_pos)});
        }

        // Find the matching closing brace (this is a simplified implementation)
        int brace_level = 1;
        size_t brace_close_pos = brace_open_pos + 1;
        while (brace_level > 0 && brace_close_pos < source_.length()) {
            if (source_[brace_close_pos] == '{') brace_level++;
            else if (source_[brace_close_pos] == '}') brace_level--;
            brace_close_pos++;
        }

        if (brace_level == 0) {
            // Found a script block
            std::string script_content = source_.substr(brace_open_pos + 1, brace_close_pos - brace_open_pos - 2);
            chunks_.push_back({ChunkType::ChtlJs, script_content});
            last_pos = brace_close_pos;
        } else {
            // Unterminated script block, treat as CHTL
            break;
        }
    }

    // Add the remaining CHTL content
    if (last_pos < source_.length()) {
        chunks_.push_back({ChunkType::CHTL, source_.substr(last_pos)});
    }

    return chunks_;
}

// Private methods will be implemented in future steps.
void CHTLUnifiedScanner::process() {
    // Main state-machine loop will go here.
}

void CHTLUnifiedScanner::handleScriptTag() {
    // Logic to distinguish between <script> for JS and script {} for CHTL JS.
}

void CHTLUnifiedScanner::handleStyleTag() {
    // Logic to distinguish between <style> for CSS and style {} for CHTL.
}

void CHTLUnifiedScanner::handleChtlBlock() {
    // Logic for parsing a standard CHTL block.
}


} // namespace CHTL
