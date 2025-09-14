#include "Scanner/CHTLUnifiedScanner.h"
#include <cctype>

namespace CHTL {

namespace {
// Helper function to find the end of a balanced brace block
size_t find_matching_brace(const std::string& s, size_t start_pos) {
    if (start_pos >= s.length() || s[start_pos] != '{') {
        return std::string::npos;
    }
    int balance = 1;
    size_t pos = start_pos + 1;
    while (pos < s.length()) {
        if (s[pos] == '{') {
            balance++;
        } else if (s[pos] == '}') {
            balance--;
        }
        if (balance == 0) {
            return pos;
        }
        pos++;
    }
    return std::string::npos; // Not found
}
} // anonymous namespace

CHTLUnifiedScanner::CHTLUnifiedScanner(const std::string& source) : source_(source) {}

std::vector<CodeChunk> CHTLUnifiedScanner::scan() {
    // This scanner is now specialized for processing the content of a script block.
    // It finds CHTL JS expressions {{...}} and separates them from pure JS.
    size_t last_pos = 0;

    while (last_pos < source_.length()) {
        size_t chtl_js_start = source_.find("{{", last_pos);

        if (chtl_js_start == std::string::npos) {
            // No more CHTL JS, the rest is pure JavaScript
            break;
        }

        // Add the JavaScript part before the CHTL JS `{{`
        if (chtl_js_start > last_pos) {
            chunks_.push_back({ChunkType::JavaScript, source_.substr(last_pos, chtl_js_start - last_pos)});
        }

        size_t chtl_js_end = source_.find("}}", chtl_js_start);
        if (chtl_js_end == std::string::npos) {
            // Unterminated CHTL JS, treat the rest as JS
            last_pos = chtl_js_start;
            break;
        }
        chtl_js_end += 2; // include the '}}'

        // Greedily consume the rest of the CHTL JS expression
        while (chtl_js_end < source_.length()) {
            char c = source_[chtl_js_end];
            if (isspace(c)) break;
            if (c == '(') {
                size_t paren_end = source_.find(')', chtl_js_end);
                if(paren_end != std::string::npos) {
                    chtl_js_end = paren_end + 1;
                    continue;
                }
            }
            if (isalnum(c) || c == '_' || c == '.' || c == ';' || c == '[' || c == ']') {
                 chtl_js_end++;
                 continue;
            }
            if (c == '-' && chtl_js_end + 1 < source_.length() && source_[chtl_js_end + 1] == '>') {
                chtl_js_end += 2;
                continue;
            }
            break;
        }

        chunks_.push_back({ChunkType::ChtlJs, source_.substr(chtl_js_start, chtl_js_end - chtl_js_start)});
        last_pos = chtl_js_end;
    }

    // Add the final JavaScript part
    if (last_pos < source_.length()) {
        std::string js_part = source_.substr(last_pos);
        if (!js_part.empty()) {
            chunks_.push_back({ChunkType::JavaScript, js_part});
        }
    }

    return chunks_;
}

// The methods below are now obsolete as the scanner is specialized.
void CHTLUnifiedScanner::process() {}
void CHTLUnifiedScanner::handleScriptTag() {}
void CHTLUnifiedScanner::handleStyleTag() {}

} // namespace CHTL
