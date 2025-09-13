#include "Scanner/CHTLUnifiedScanner.h"

namespace CHTL {

CHTLUnifiedScanner::CHTLUnifiedScanner(const std::string& source) : source_(source) {}

std::vector<CodeChunk> CHTLUnifiedScanner::scan() {
    size_t pos = 0;
    size_t last_pos = 0;
    int brace_level = 0;

    while (pos < source_.length()) {
        if (source_[pos] == '{') {
            brace_level++;
        } else if (source_[pos] == '}') {
            brace_level--;
        } else if (brace_level == 1 && source_.substr(pos, 6) == "script") {
            // Check if it's a standalone word
            if ((pos + 6 == source_.length() || isspace(source_[pos + 6]) || source_[pos+6] == '{')) {
                size_t script_keyword_end = pos + 6;
                size_t brace_open_pos = source_.find('{', script_keyword_end);

                if (brace_open_pos != std::string::npos) {
                    // Check for only whitespace between keyword and brace
                    bool only_whitespace = true;
                    for (size_t i = script_keyword_end; i < brace_open_pos; ++i) {
                        if (!isspace(source_[i])) {
                            only_whitespace = false;
                            break;
                        }
                    }

                    if (only_whitespace) {
                        // We found a script block. Finalize the CHTL chunk before it.
                        if (pos > last_pos) {
                            chunks_.push_back({ChunkType::CHTL, source_.substr(last_pos, pos - last_pos)});
                        }

                        // Find matching closing brace
                        int script_brace_level = 1;
                        size_t script_end_pos = brace_open_pos + 1;
                        while(script_end_pos < source_.length() && script_brace_level > 0) {
                            if(source_[script_end_pos] == '{') script_brace_level++;
                            if(source_[script_end_pos] == '}') script_brace_level--;
                            script_end_pos++;
                        }

                        if (script_brace_level == 0) {
                            // Extract content and add as ChtlJs chunk
                            std::string content = source_.substr(brace_open_pos + 1, script_end_pos - brace_open_pos - 2);
                            chunks_.push_back({ChunkType::ChtlJs, content});
                            pos = script_end_pos;
                            last_pos = pos;
                            continue;
                        }
                    }
                }
            }
        }
        pos++;
    }

    if (last_pos < source_.length()) {
        chunks_.push_back({ChunkType::CHTL, source_.substr(last_pos)});
    }

    return chunks_;
}

// Private methods will be implemented in future steps.
void CHTLUnifiedScanner::process() {}
void CHTLUnifiedScanner::handleScriptTag() {}
void CHTLUnifiedScanner::handleStyleTag() {}
void CHTLUnifiedScanner::handleChtlBlock() {}

} // namespace CHTL
