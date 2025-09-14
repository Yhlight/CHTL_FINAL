#include "Scanner/CHTLUnifiedScanner.h"
#include <cctype>
#include <sstream>
#include <algorithm>

namespace CHTL {

CHTLUnifiedScanner::CHTLUnifiedScanner(const std::string& source) : source_(source) {}

std::vector<CodeChunk> CHTLUnifiedScanner::scan() {
    process();
    return chunks_;
}

// Helper to check if a keyword is a whole word
bool isWordBoundary(const std::string& source, size_t pos, size_t len) {
    bool start_boundary = (pos == 0) || isspace(source[pos - 1]) || source[pos - 1] == '{' || source[pos - 1] == '}';
    bool end_boundary = (pos + len == source.length()) || isspace(source[pos + len]) || source[pos + len] == '{';
    return start_boundary && end_boundary;
}

// Helper to find the matching closing brace
size_t findClosingBrace(const std::string& source, size_t start_pos) {
    int brace_level = 0;
    for (size_t i = start_pos; i < source.length(); ++i) {
        if (source[i] == '{') {
            brace_level++;
        } else if (source[i] == '}') {
            brace_level--;
            if (brace_level == 0) {
                return i;
            }
        }
    }
    return std::string::npos; // Not found
}

void CHTLUnifiedScanner::process() {
    size_t last_pos = 0;
    while (current_ < source_.length()) {
        size_t script_pos = source_.find("script", current_);
        size_t style_pos = source_.find("style", current_);

        size_t next_pos = std::min(script_pos, style_pos);

        if (next_pos == std::string::npos) {
            // No more script or style tags, the rest is CHTL
            break;
        }

        const std::string& keyword = (next_pos == script_pos) ? "script" : "style";

        if (isWordBoundary(source_, next_pos, keyword.length())) {
            size_t brace_pos = source_.find('{', next_pos + keyword.length());
            if (brace_pos != std::string::npos) {
                // Check for only whitespace between keyword and brace
                bool only_whitespace = true;
                for (size_t i = next_pos + keyword.length(); i < brace_pos; ++i) {
                    if (!isspace(source_[i])) {
                        only_whitespace = false;
                        break;
                    }
                }

                if (only_whitespace) {
                    // We found a block. Chunk the preceding text as CHTL.
                    if (next_pos > last_pos) {
                        chunks_.push_back({ChunkType::CHTL, source_.substr(last_pos, next_pos - last_pos)});
                    }

                    current_ = brace_pos; // Position at the opening brace
                    if (keyword == "script") {
                        handleScriptTag();
                    } else {
                        handleStyleTag();
                    }
                    last_pos = current_;
                    continue; // Continue scanning from the new position
                }
            }
        }
        // If it wasn't a valid block, just advance past the keyword and continue scanning
        current_ = next_pos + 1;
    }

    // Add the final remaining CHTL chunk
    if (last_pos < source_.length()) {
        chunks_.push_back({ChunkType::CHTL, source_.substr(last_pos)});
    }
}

void CHTLUnifiedScanner::handleScriptTag() {
    size_t end_brace = findClosingBrace(source_, current_);
    if (end_brace == std::string::npos) {
        // Unmatched brace, consume rest of file to avoid infinite loops.
        current_ = source_.length();
        return;
    }

    std::string script_content = source_.substr(current_ + 1, end_brace - current_ - 1);
    size_t local_pos = 0;

    while (local_pos < script_content.length()) {
        size_t chtl_js_start = script_content.find("{{", local_pos);

        if (chtl_js_start == std::string::npos) {
            // No more CHTL JS, the rest is pure JavaScript
            break;
        }

        // The part before the CHTL-JS is JavaScript
        if (chtl_js_start > local_pos) {
            std::string js_part = script_content.substr(local_pos, chtl_js_start - local_pos);
            std::stringstream placeholder;
            placeholder << "_JS_CODE_PLACEHOLDER_" << placeholder_id_++ << "_";
            placeholder_map_[placeholder.str()] = js_part;
            chunks_.push_back({ChunkType::Placeholder, placeholder.str()});
        }

        // Find the end of the CHTL-JS block
        size_t chtl_js_end = script_content.find("}}", chtl_js_start);
        if (chtl_js_end == std::string::npos) {
            // Malformed CHTL JS, treat the rest as JS and stop.
            local_pos = chtl_js_start;
            break;
        }
        chtl_js_end += 2; // Include the closing "}}"

        // The CHTL-JS part
        std::string chtl_js_part = script_content.substr(chtl_js_start, chtl_js_end - chtl_js_start);
        chunks_.push_back({ChunkType::ChtlJs, chtl_js_part});

        local_pos = chtl_js_end;
    }

    // Handle any remaining pure JavaScript at the end of the script block
    if (local_pos < script_content.length()) {
        std::string js_part = script_content.substr(local_pos);
        std::stringstream placeholder;
        placeholder << "_JS_CODE_PLACEHOLDER_" << placeholder_id_++ << "_";
        placeholder_map_[placeholder.str()] = js_part;
        chunks_.push_back({ChunkType::Placeholder, placeholder.str()});
    }

    current_ = end_brace + 1;
}

void CHTLUnifiedScanner::handleStyleTag() {
    size_t end_brace = findClosingBrace(source_, current_);
    if (end_brace != std::string::npos) {
        // Extract the entire content of the style block as a single CSS chunk.
        // The CompilerDispatcher will later send this to the appropriate compiler
        // that can handle CHTL features within CSS.
        std::string content = source_.substr(current_ + 1, end_brace - current_ - 1);
        chunks_.push_back({ChunkType::Css, content});
        current_ = end_brace + 1;
    } else {
        // Unmatched brace, treat as error or consume rest of file.
        current_ = source_.length();
    }
}

} // namespace CHTL
