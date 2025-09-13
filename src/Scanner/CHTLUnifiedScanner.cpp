#include "Scanner/CHTLUnifiedScanner.h"
#include <iostream> // For debugging
#include <cctype>   // For isspace

namespace CHTL {

// Constructor
CHTLUnifiedScanner::CHTLUnifiedScanner(const std::string& source) : source_(source) {}

// Main scanning function to be called by users
std::vector<CodeChunk> CHTLUnifiedScanner::scan() {
    process();
    return chunks_;
}

// The main processing loop
void CHTLUnifiedScanner::process() {
    size_t last_pos = 0;
    while (current_ < source_.length()) {
        if (source_.substr(current_, 6) == "script") {
            // Found a potential script block
            if (current_ > last_pos) {
                chunks_.push_back({ChunkType::CHTL, source_.substr(last_pos, current_ - last_pos)});
            }
            handleScriptBlock();
            last_pos = current_;
        } else if (source_.substr(current_, 5) == "style") {
            // Found a potential style block
            if (current_ > last_pos) {
                chunks_.push_back({ChunkType::CHTL, source_.substr(last_pos, current_ - last_pos)});
            }
            handleStyleBlock();
            last_pos = current_;
        } else if (source_.substr(current_, 8) == "[Origin]") {
            // Found a potential origin block
            if (current_ > last_pos) {
                chunks_.push_back({ChunkType::CHTL, source_.substr(last_pos, current_ - last_pos)});
            }
            handleOriginBlock();
            last_pos = current_;
        } else {
            current_++;
        }
    }

    if (last_pos < source_.length()) {
        chunks_.push_back({ChunkType::CHTL, source_.substr(last_pos)});
    }
}

// Helper to find the position of a matching closing brace
size_t findMatchingBrace(const std::string& s, size_t start_pos) {
    int brace_level = 0;
    bool in_string = false;
    char string_char = '\0';

    for (size_t i = start_pos; i < s.length(); ++i) {
        if (in_string) {
            if (s[i] == '\\' && i + 1 < s.length()) {
                i++; // Skip escaped character
                continue;
            }
            if (s[i] == string_char) {
                in_string = false;
            }
        } else {
            if (s[i] == '"' || s[i] == '\'' || s[i] == '`') {
                in_string = true;
                string_char = s[i];
            } else if (s[i] == '{') {
                brace_level++;
            } else if (s[i] == '}') {
                brace_level--;
                if (brace_level == 0) {
                    return i;
                }
            }
        }
    }
    return std::string::npos; // Not found
}

// Handles the separation of JS and CHTL-JS within a script block
void CHTLUnifiedScanner::handleScriptBlock() {
    size_t start_of_block = source_.find('{', current_);
    if (start_of_block == std::string::npos) {
        // Malformed, just consume keyword and move on
        current_ += 6;
        return;
    }

    size_t end_of_block = findMatchingBrace(source_, start_of_block + 1);
    if (end_of_block == std::string::npos) {
        // Malformed, just consume keyword and move on
        current_ += 6;
        return;
    }

    // Extract the content of the script block
    std::string content = source_.substr(start_of_block + 1, end_of_block - start_of_block - 1);

    size_t content_pos = 0;
    size_t last_content_pos = 0;
    while (content_pos < content.length()) {
        size_t cjs_start = content.find("{{", content_pos);
        if (cjs_start == std::string::npos) {
            break; // No more CHTL-JS
        }

        size_t cjs_end = content.find("}}", cjs_start);
        if (cjs_end == std::string::npos) {
            break; // Malformed CHTL-JS
        }
        cjs_end += 2; // Include the closing braces

        // 1. Handle the JS part before the CHTL-JS
        if (cjs_start > last_content_pos) {
            std::string js_part = content.substr(last_content_pos, cjs_start - last_content_pos);
            if (!js_part.empty()) {
                std::string placeholder = "_JS_CODE_PLACEHOLDER_" + std::to_string(placeholder_id_++) + "_";
                placeholder_map_[placeholder] = js_part;
                chunks_.push_back({ChunkType::Placeholder, placeholder});
            }
        }

        // 2. Handle the CHTL-JS part
        std::string cjs_part = content.substr(cjs_start, cjs_end - cjs_start);
        chunks_.push_back({ChunkType::ChtlJs, cjs_part});

        content_pos = cjs_end;
        last_content_pos = content_pos;
    }

    // 3. Handle any remaining JS part at the end
    if (last_content_pos < content.length()) {
        std::string final_js_part = content.substr(last_content_pos);
        if (!final_js_part.empty()) {
             std::string placeholder = "_JS_CODE_PLACEHOLDER_" + std::to_string(placeholder_id_++) + "_";
             placeholder_map_[placeholder] = final_js_part;
             chunks_.push_back({ChunkType::Placeholder, placeholder});
        }
    }

    // Move the main scanner cursor past this entire block
    current_ = end_of_block + 1;
}

// Handles style blocks by treating their content as a single CSS chunk.
void CHTLUnifiedScanner::handleStyleBlock() {
    size_t start_of_block = source_.find('{', current_);
    if (start_of_block == std::string::npos) {
        // Malformed, just consume keyword and move on
        current_ += 5; // length of "style"
        return;
    }

    size_t end_of_block = findMatchingBrace(source_, start_of_block + 1);
    if (end_of_block == std::string::npos) {
        // Malformed, just consume keyword and move on
        current_ += 5;
        return;
    }

    // Extract the content of the style block and add it as a CSS chunk.
    std::string content = source_.substr(start_of_block + 1, end_of_block - start_of_block - 1);
    chunks_.push_back({ChunkType::Css, content});

    // Move the main scanner cursor past this entire block
    current_ = end_of_block + 1;
}

// Handles [Origin] blocks for embedding raw code.
void CHTLUnifiedScanner::handleOriginBlock() {
    size_t start_of_block = source_.find('{', current_);
    if (start_of_block == std::string::npos) {
        current_ += 8; // Consume "[Origin]"
        return;
    }

    // Find the type, which is between "[Origin]" and "{".
    std::string type_str;
    size_t type_start = current_ + 8; // Right after "[Origin]"
    while(type_start < start_of_block && isspace(source_[type_start])) {
        type_start++;
    }
    size_t type_end = type_start;
    while(type_end < start_of_block && !isspace(source_[type_end])) {
        type_end++;
    }

    type_str = source_.substr(type_start, type_end - type_start);

    size_t end_of_block = findMatchingBrace(source_, start_of_block + 1);
    if (end_of_block == std::string::npos) {
        current_ += 8; // Consume "[Origin]"
        return;
    }

    std::string content = source_.substr(start_of_block + 1, end_of_block - start_of_block - 1);
    ChunkType type;

    if (type_str == "@Style") {
        type = ChunkType::Css;
    } else if (type_str == "@JavaScript") {
        type = ChunkType::JavaScript;
    } else { // Default case for @Html or any other custom origin type
        type = ChunkType::CHTL;
    }

    chunks_.push_back({type, content});
    current_ = end_of_block + 1;
}

// This function is not used in the new structure, but kept for compatibility with the header.
void CHTLUnifiedScanner::handleChtlBlock() {
    // This logic is now handled by the main process() loop.
}

} // namespace CHTL
