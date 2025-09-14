#include "Scanner/CHTLUnifiedScanner.h"
#include <cctype>

namespace CHTL {

// Helper to check if the position is at a word boundary
bool isWordBoundary(const std::string& source, size_t pos, size_t word_len) {
    if (pos > 0 && (isalnum(source[pos - 1]) || source[pos - 1] == '_')) {
        return false; // Not a start of a word
    }
    size_t after_pos = pos + word_len;
    if (after_pos < source.length() && (isalnum(source[after_pos]) || source[after_pos] == '_')) {
        return false; // Not an end of a word
    }
    return true;
}

// Helper to find a matching closing brace
size_t findMatchingBrace(const std::string& source, size_t start_pos) {
    if (start_pos >= source.length() || source[start_pos] != '{') {
        return std::string::npos;
    }
    int brace_level = 1;
    size_t current_pos = start_pos + 1;
    while(current_pos < source.length() && brace_level > 0) {
        if(source[current_pos] == '{') brace_level++;
        if(source[current_pos] == '}') brace_level--;
        current_pos++;
    }
    if (brace_level == 0) {
        return current_pos;
    }
    return std::string::npos; // Not found
}


CHTLUnifiedScanner::CHTLUnifiedScanner(const std::string& source) : source_(source) {}

std::vector<CodeChunk> CHTLUnifiedScanner::scan() {
    size_t last_pos = 0;
    current_ = 0;

    while (current_ < source_.length()) {
        // Find the next interesting keyword
        size_t script_pos = source_.find("script", current_);
        size_t style_pos = source_.find("style", current_);
        size_t origin_pos = source_.find("[Origin]", current_);

        size_t next_pos = std::string::npos;
        if (script_pos != std::string::npos) next_pos = std::min(next_pos, script_pos);
        if (style_pos != std::string::npos) next_pos = std::min(next_pos, style_pos);
        if (origin_pos != std::string::npos) next_pos = std::min(next_pos, origin_pos);

        if (next_pos == std::string::npos) {
            // No more special blocks, the rest is CHTL
            break;
        }

        // Check if the found keyword is a valid block start
        bool handled = false;
        if (next_pos == script_pos && isWordBoundary(source_, script_pos, 6)) {
            handled = handleScriptBlock(script_pos, last_pos);
        } else if (next_pos == style_pos && isWordBoundary(source_, style_pos, 5)) {
            handled = handleStyleBlock(style_pos, last_pos);
        } else if (next_pos == origin_pos) { // [Origin] doesn't need word boundary check
            handled = handleOriginBlock(origin_pos, last_pos);
        }

        if (!handled) {
            // Keyword was not a valid block start (e.g., 'myscript'), advance past it
            current_ = next_pos + 1;
        }
    }

    // Add the final remaining CHTL chunk
    if (last_pos < source_.length()) {
        chunks_.push_back({ChunkType::CHTL, source_.substr(last_pos)});
    }

    return chunks_;
}

std::string CHTLUnifiedScanner::scanScriptContent(const std::string& content) {
    std::stringstream result_stream;
    size_t last_pos = 0;
    size_t current_pos = 0;

    while ((current_pos = content.find("{{", last_pos)) != std::string::npos) {
        // Part 1: Handle the JS code before the {{
        std::string js_part = content.substr(last_pos, current_pos - last_pos);
        if (!js_part.empty()) {
            std::string placeholder = "_JS_CODE_PLACEHOLDER_" + std::to_string(placeholder_id_++) + "_";
            placeholder_map_[placeholder] = js_part;
            result_stream << placeholder;
        }

        // Part 2: Find the matching }} and handle the CHTL JS part
        size_t chtl_js_end = content.find("}}", current_pos + 2);
        if (chtl_js_end == std::string::npos) {
             // Malformed CHTL JS, treat rest as JS
            break;
        }
        chtl_js_end += 2; // Include the }}
        result_stream << content.substr(current_pos, chtl_js_end - current_pos);
        last_pos = chtl_js_end;
    }

    // Part 3: Handle any remaining JS code after the last }}
    std::string remaining_js = content.substr(last_pos);
    if (!remaining_js.empty()) {
        std::string placeholder = "_JS_CODE_PLACEHOLDER_" + std::to_string(placeholder_id_++) + "_";
        placeholder_map_[placeholder] = remaining_js;
        result_stream << placeholder;
    }

    return result_stream.str();
}

bool CHTLUnifiedScanner::handleScriptBlock(size_t keyword_pos, size_t& last_pos) {
    size_t brace_open_pos = source_.find('{', keyword_pos + 6);
    if (brace_open_pos == std::string::npos) return false;

    // Ensure only whitespace is between keyword and brace
    for (size_t i = keyword_pos + 6; i < brace_open_pos; ++i) {
        if (!isspace(source_[i])) return false;
    }

    size_t brace_close_pos = findMatchingBrace(source_, brace_open_pos);
    if (brace_close_pos == std::string::npos) return false;

    // We found a script block. Finalize the CHTL chunk before it.
    if (keyword_pos > last_pos) {
        chunks_.push_back({ChunkType::CHTL, source_.substr(last_pos, keyword_pos - last_pos)});
    }

    // Scan the content for JS/CHTL JS separation
    std::string raw_content = source_.substr(brace_open_pos + 1, brace_close_pos - brace_open_pos - 2);
    std::string processed_content = scanScriptContent(raw_content);
    chunks_.push_back({ChunkType::ChtlJs, processed_content});

    current_ = brace_close_pos;
    last_pos = current_;
    return true;
}

bool CHTLUnifiedScanner::handleStyleBlock(size_t keyword_pos, size_t& last_pos) {
    size_t brace_open_pos = source_.find('{', keyword_pos + 5);
    if (brace_open_pos == std::string::npos) return false;

    for (size_t i = keyword_pos + 5; i < brace_open_pos; ++i) {
        if (!isspace(source_[i])) return false;
    }

    size_t brace_close_pos = findMatchingBrace(source_, brace_open_pos);
    if (brace_close_pos == std::string::npos) return false;

    if (keyword_pos > last_pos) {
        chunks_.push_back({ChunkType::CHTL, source_.substr(last_pos, keyword_pos - last_pos)});
    }

    // TODO: This logic is simplified. A full parser would need to track CHTL element
    // nesting to differentiate between a global <style> block (pure CSS) and a local
    // one inside an element (which allows CHTL features). For now, all style
    // blocks are treated as pure CSS.
    std::string content = source_.substr(brace_open_pos + 1, brace_close_pos - brace_open_pos - 2);
    chunks_.push_back({ChunkType::Css, content});

    current_ = brace_close_pos;
    last_pos = current_;
    return true;
}

bool CHTLUnifiedScanner::handleOriginBlock(size_t keyword_pos, size_t& last_pos) {
    size_t type_start_pos = source_.find('@', keyword_pos + 8);
    if (type_start_pos == std::string::npos) return false;

    size_t brace_open_pos = source_.find('{', type_start_pos);
    if (brace_open_pos == std::string::npos) return false;

    // Extract type, skipping the '@'
    std::string type_str;
    size_t type_end_pos = type_start_pos + 1;
    while(type_end_pos < brace_open_pos && !isspace(source_[type_end_pos])) {
        type_end_pos++;
    }
    type_str = source_.substr(type_start_pos + 1, type_end_pos - (type_start_pos + 1));


    size_t brace_close_pos = findMatchingBrace(source_, brace_open_pos);
    if (brace_close_pos == std::string::npos) return false;

    // Finalize the CHTL chunk before this block
    if (keyword_pos > last_pos) {
        chunks_.push_back({ChunkType::CHTL, source_.substr(last_pos, keyword_pos - last_pos)});
    }

    std::string content = source_.substr(brace_open_pos + 1, brace_close_pos - brace_open_pos - 2);
    ChunkType chunk_type = ChunkType::CHTL; // Default for @Html and custom types
    if (type_str == "Style") {
        chunk_type = ChunkType::Css;
    } else if (type_str == "JavaScript") {
        chunk_type = ChunkType::JavaScript;
    }

    chunks_.push_back({chunk_type, content});

    current_ = brace_close_pos;
    last_pos = current_;
    return true;
}

// Private methods will be implemented in future steps.
void CHTLUnifiedScanner::process() {}
void CHTLUnifiedScanner::handleChtlBlock() {}

const std::map<std::string, std::string>& CHTLUnifiedScanner::getPlaceholderMap() const {
    return placeholder_map_;
}

} // namespace CHTL
