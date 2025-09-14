#include "Scanner/CHTLUnifiedScanner.h"
#include <cctype> // for isspace
#include <algorithm>
#include <vector>
#include <utility> // For std::pair

namespace CHTL {

// Helper function to find the matching closing brace
size_t find_matching_brace(const std::string& s, size_t start_pos) {
    if (s[start_pos] != '{') {
        return std::string::npos;
    }
    int brace_level = 1;
    for (size_t i = start_pos + 1; i < s.length(); ++i) {
        if (s[i] == '{') {
            brace_level++;
        } else if (s[i] == '}') {
            brace_level--;
            if (brace_level == 0) {
                return i;
            }
        }
    }
    return std::string::npos;
}

CHTLUnifiedScanner::CHTLUnifiedScanner(const std::string& source) : source_(source) {}

std::vector<CodeChunk> CHTLUnifiedScanner::scan() {
    process();
    return chunks_;
}

void CHTLUnifiedScanner::process() {
    size_t last_pos = 0;
    int brace_level = 0;

    while (current_ < source_.length()) {
        if (source_[current_] == '{') {
            brace_level++;
        } else if (source_[current_] == '}') {
            brace_level--;
        } else if (source_.substr(current_, 6) == "script") {
            if (current_ > last_pos) {
                chunks_.push_back({ChunkType::CHTL, source_.substr(last_pos, current_ - last_pos)});
            }
            handleScriptTag(brace_level);
            last_pos = current_;
            continue; // Skip the current_++ at the end
        } else if (source_.substr(current_, 5) == "style" && brace_level <= 1) {
            // Only handle global style blocks. Local styles are part of CHTL.
            if (current_ > last_pos) {
                chunks_.push_back({ChunkType::CHTL, source_.substr(last_pos, current_ - last_pos)});
            }
            handleStyleTag(brace_level);
            last_pos = current_;
            continue; // Skip the current_++ at the end
        }
        current_++;
    }

    // Add the final remaining part of the source as a CHTL chunk
    if (last_pos < source_.length()) {
        chunks_.push_back({ChunkType::CHTL, source_.substr(last_pos)});
    }
}

void CHTLUnifiedScanner::handleScriptTag(int brace_level) {
    size_t keyword_end = current_ + 6;

    size_t brace_open = keyword_end;
    while(brace_open < source_.length() && isspace(source_[brace_open])) {
        brace_open++;
    }

    if (brace_open < source_.length() && source_[brace_open] == '{') {
        size_t brace_close = find_matching_brace(source_, brace_open);
        if (brace_close != std::string::npos) {
            std::string content = source_.substr(brace_open + 1, brace_close - brace_open - 1);
            processScriptContent(content);
            current_ = brace_close + 1;
            return;
        }
    }
    current_ = keyword_end;
}

// Helper to check if a string is only whitespace
bool is_all_whitespace(const std::string& s) {
    return std::all_of(s.begin(), s.end(), isspace);
}

void CHTLUnifiedScanner::processScriptContent(const std::string& content) {
    // This is a simplified island-based parser.
    // It finds all CHTL-JS constructs and treats everything else as JS.

    // 1. Find all CHTL-JS "islands"
    std::vector<std::pair<size_t, size_t>> islands;
    size_t pos = 0;
    while((pos = content.find("{{", pos)) != std::string::npos) {
        size_t end_pos = content.find("}}", pos);
        if (end_pos != std::string::npos) {
            islands.push_back({pos, end_pos + 2});
            pos = end_pos + 2;
        } else {
            // Unterminated, skip to avoid infinite loop
            pos += 2;
        }
    }

    // TODO: Add logic here to find other CHTL-JS keywords like 'listen', 'animate', etc.
    // and add their ranges to the islands vector. Then sort the vector.

    std::string chtl_js_buffer;
    size_t last_pos = 0;

    auto process_js_part = [&](const std::string& js_part) {
        if (is_all_whitespace(js_part)) {
            chtl_js_buffer += js_part;
        } else {
            std::string placeholder = "_JS_CODE_PLACEHOLDER_" + std::to_string(placeholder_id_++) + "_";
            chunks_.push_back({ChunkType::JavaScript, js_part});
            placeholder_map_[placeholder] = js_part;
            chtl_js_buffer += placeholder;
        }
    };

    for (const auto& island : islands) {
        // 2. Process the "sea" of JS before the island
        if (island.first > last_pos) {
            process_js_part(content.substr(last_pos, island.first - last_pos));
        }

        // 3. Process the CHTL-JS island itself
        chtl_js_buffer += content.substr(island.first, island.second - island.first);
        last_pos = island.second;
    }

    // 4. Process any remaining JS "sea" after the last island
    if (last_pos < content.length()) {
        process_js_part(content.substr(last_pos));
    }

    // 5. Add the final CHTL-JS chunk
    if (!chtl_js_buffer.empty()) {
        chunks_.push_back({ChunkType::ChtlJs, chtl_js_buffer});
    } else if (islands.empty() && !content.empty()) {
        // If there were no islands, the whole thing is one JS chunk.
        chunks_.push_back({ChunkType::JavaScript, content});
    }
}


void CHTLUnifiedScanner::handleStyleTag(int brace_level) {
    // This handler is now only called for global style blocks.
    size_t keyword_end = current_ + 5;
    size_t brace_open = source_.find('{', keyword_end);

    if (brace_open != std::string::npos) {
        size_t brace_close = find_matching_brace(source_, brace_open);
        if (brace_close != std::string::npos) {
            std::string content = source_.substr(brace_open + 1, brace_close - brace_open - 1);
            chunks_.push_back({ChunkType::Css, content});
            current_ = brace_close + 1;
            return;
        }
    }
    // If not a valid block, just advance past the keyword
    current_ = keyword_end;
}


void CHTLUnifiedScanner::handleChtlBlock(int brace_level) {}

} // namespace CHTL
