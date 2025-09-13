#include "Scanner/CHTLUnifiedScanner.h"
#include <iostream> // For debugging
#include <cctype>   // For isspace
#include <vector>   // For std::vector

namespace CHTL {

CHTLUnifiedScanner::CHTLUnifiedScanner(const std::string& source) : source_(source), current_(0) {}

std::vector<CodeChunk> CHTLUnifiedScanner::scan() {
    process();
    return chunks_;
}

void CHTLUnifiedScanner::process() {
    size_t last_pos = 0;

    while (current_ < source_.length()) {
        size_t next_script = source_.find("script", current_);
        size_t next_style = source_.find("style", current_);

        size_t next_tag = std::min(next_script, next_style);

        if (next_tag == std::string::npos) {
            // No more script/style tags, the rest is CHTL
            break;
        }

        // Check if the found keyword is a standalone word
        if (next_tag > 0 && !isspace(source_[next_tag - 1])) {
             current_ = next_tag + 1;
             continue;
        }

        // We found a potential tag. Add the preceding text as a CHTL chunk.
        if (next_tag > last_pos) {
            chunks_.push_back({ChunkType::CHTL, source_.substr(last_pos, next_tag - last_pos)});
        }

        current_ = next_tag;

        if (current_ == next_script) {
            handleScriptTag();
        } else if (current_ == next_style) {
            handleStyleTag();
        }

        last_pos = current_;
    }

    if (last_pos < source_.length()) {
        chunks_.push_back({ChunkType::CHTL, source_.substr(last_pos)});
    }
}


void CHTLUnifiedScanner::handleScriptTag() {
    current_ += 6; // Move past "script"

    size_t brace_open_pos = source_.find('{', current_);
    if (brace_open_pos == std::string::npos) return;

    for(size_t i = current_; i < brace_open_pos; ++i) {
        if (!isspace(source_[i])) return;
    }

    int brace_level = 1;
    size_t content_start_pos = brace_open_pos + 1;
    size_t search_pos = content_start_pos;

    while (search_pos < source_.length() && brace_level > 0) {
        if (source_[search_pos] == '{') brace_level++;
        else if (source_[search_pos] == '}') brace_level--;
        search_pos++;
    }

    if (brace_level != 0) {
        current_ = search_pos;
        return; // Unmatched brace
    }

    size_t content_end_pos = search_pos - 1;
    std::string script_content = source_.substr(content_start_pos, content_end_pos - content_start_pos);

    // State machine implementation
    size_t pos = 0;
    size_t chunk_start = 0;
    ChunkType current_chunk_type = ChunkType::JavaScript;

    const std::vector<std::string> chtl_js_keywords = {"listen", "delegate", "animate", "router", "fileloader", "util"};

    auto finalize_chunk = [&](size_t end_pos) {
        if (end_pos > chunk_start) {
            chunks_.push_back({current_chunk_type, script_content.substr(chunk_start, end_pos - chunk_start)});
        }
    };

    while (pos < script_content.length()) {
        bool pattern_found = false;

        // Check for {{
        if (script_content.substr(pos, 2) == "{{") {
            finalize_chunk(pos);
            current_chunk_type = ChunkType::ChtlJs;
            chunk_start = pos;

            size_t end_brace = script_content.find("}}", pos);
            if (end_brace != std::string::npos) {
                pos = end_brace + 2;

                // --- START OF FIX ---
                // Peek ahead for -> operator
                size_t peek_pos = pos;
                while(peek_pos < script_content.length() && isspace(script_content[peek_pos])) {
                    peek_pos++;
                }

                // --- START: Lookahead for chained CHTL-JS calls like {{...}}->listen{...} ---
                size_t lookahead_pos = pos;
                while(lookahead_pos < script_content.length() && isspace(script_content[lookahead_pos])) {
                    lookahead_pos++;
                }

                if (script_content.substr(lookahead_pos, 2) == "->") {
                    lookahead_pos += 2; // Consume '->'
                    while(lookahead_pos < script_content.length() && isspace(script_content[lookahead_pos])) {
                        lookahead_pos++;
                    }

                    for(const auto& keyword : chtl_js_keywords) {
                        if (script_content.substr(lookahead_pos, keyword.length()) == keyword) {
                            size_t block_start = script_content.find('{', lookahead_pos + keyword.length());
                            if (block_start != std::string::npos) {
                                int inner_brace_level = 1;
                                size_t block_end = block_start + 1;
                                while(block_end < script_content.length() && inner_brace_level > 0) {
                                    if(script_content[block_end] == '{') inner_brace_level++;
                                    if(script_content[block_end] == '}') inner_brace_level--;
                                    block_end++;
                                }
                                if (inner_brace_level == 0) {
                                   // The entire {{...}}->keyword{...} is one ChtlJs chunk.
                                   // We update the main 'pos' to the end of this entire construct.
                                   pos = block_end;
                                }
                            }
                            break; // Found matching keyword, stop searching
                        }
                    }
                }
                // --- END: Lookahead ---
                // --- END OF FIX ---
            } else {
                pos += 2; // Malformed, skip
            }
            finalize_chunk(pos);
            current_chunk_type = ChunkType::JavaScript;
            chunk_start = pos;
            pattern_found = true;
        }

        // Check for keywords
        if (!pattern_found) {
            for (const auto& keyword : chtl_js_keywords) {
                if (script_content.substr(pos, keyword.length()) == keyword &&
                    (pos + keyword.length() == script_content.length() || !isalnum(script_content[pos + keyword.length()]))) {

                    finalize_chunk(pos);
                    current_chunk_type = ChunkType::ChtlJs;
                    chunk_start = pos;

                    size_t block_start = script_content.find('{', pos + keyword.length());
                    if (block_start != std::string::npos) {
                        int inner_brace_level = 1;
                        size_t block_end = block_start + 1;
                        while(block_end < script_content.length() && inner_brace_level > 0) {
                            if(script_content[block_end] == '{') inner_brace_level++;
                            if(script_content[block_end] == '}') inner_brace_level--;
                            block_end++;
                        }
                        if(inner_brace_level == 0) {
                            pos = block_end;
                        } else {
                            pos = block_start + 1; // Malformed
                        }
                    } else {
                        pos += keyword.length(); // Keyword without block
                    }

                    finalize_chunk(pos);
                    current_chunk_type = ChunkType::JavaScript;
                    chunk_start = pos;
                    pattern_found = true;
                    break;
                }
            }
        }

        if (!pattern_found) {
            pos++;
        }
    }

    finalize_chunk(pos);

    current_ = search_pos;
}

void CHTLUnifiedScanner::handleStyleTag() {
    current_ += 5; // Move past "style"

    // Skip whitespace until '{'
    size_t brace_open_pos = source_.find('{', current_);
    if (brace_open_pos == std::string::npos) {
        // Malformed, but we'll just stop processing this tag
        return;
    }

    for(size_t i = current_; i < brace_open_pos; ++i) {
        if (!isspace(source_[i])) {
            // Not a style block, maybe just a variable named "style"
            // Let the CHTL chunk handle it.
            return;
        }
    }

    int brace_level = 1;
    size_t content_start_pos = brace_open_pos + 1;
    size_t search_pos = content_start_pos;

    while (search_pos < source_.length() && brace_level > 0) {
        if (source_[search_pos] == '{') {
            brace_level++;
        } else if (source_[search_pos] == '}') {
            brace_level--;
        }
        search_pos++;
    }

    if (brace_level == 0) {
        size_t content_end_pos = search_pos - 1;
        std::string content = source_.substr(content_start_pos, content_end_pos - content_start_pos);
        chunks_.push_back({ChunkType::Css, content});
        current_ = search_pos;
    } else {
        // Unmatched brace, treat as an error or just stop processing
        // For now, we just advance past the point we've scanned.
        current_ = search_pos;
    }
}

void CHTLUnifiedScanner::handleChtlBlock() {
    // This will handle general CHTL content
}

} // namespace CHTL
