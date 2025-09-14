#include "CHTLUnifiedScanner.h"
#include <cctype>
#include <stdexcept>
#include <algorithm>

namespace CHTL {

// Helper function to check for a keyword.
static bool isKeyword(const std::string& source, size_t pos, const std::string& keyword) {
    if (pos + keyword.length() > source.length()) {
        return false;
    }
    if (keyword == "{{" || keyword == "}}") {
         return source.substr(pos, keyword.length()) == keyword;
    }
    if (pos > 0 && isalnum(source[pos - 1])) {
        return false;
    }
    if (source.substr(pos, keyword.length()) != keyword) {
        return false;
    }
    size_t after_keyword = pos + keyword.length();
    if (after_keyword < source.length() && isalnum(source[after_keyword])) {
        return false;
    }
    return true;
}

CHTLUnifiedScanner::CHTLUnifiedScanner(const std::string& source)
    : source_(source),
      chtljs_keywords_({"{{", "listen", "animate", "delegate", "vir", "router"}) {
    chtljs_context_ = std::make_shared<CHTLJS::CHTLJSContext>();
}

ScanningResult CHTLUnifiedScanner::scan() {
    // 1. Isolate the CHTL JS syntax by replacing all pure JS with placeholders.
    std::string chtljs_only_source = isolateChtlJs(source_);

    // 2. Compile the CHTL JS-only source into standard JavaScript.
    CHTLJS::CHTLJSLexer lexer(chtljs_only_source);
    auto tokens = lexer.scanTokens();

    // If there's nothing to parse, just return.
    if (tokens.empty() || (tokens.size() == 1 && tokens[0].type == CHTLJS::CHTLJSTokenType::EndOfFile)) {
        return {"", placeholder_map_};
    }

    CHTLJS::CHTLJSParser parser(tokens, chtljs_context_);
    auto ast = parser.parse();

    CHTLJS::CHTLJSGenerator generator;
    std::string final_js = generator.generate(*ast);

    // 3. The final JS still has placeholders, which will be resolved by the CodeMerger later.
    return {final_js, placeholder_map_};
}

std::string CHTLUnifiedScanner::generatePlaceholder() {
    return "__JS_PLACEHOLDER_" + std::to_string(placeholder_id_++) + "__";
}

size_t CHTLUnifiedScanner::findEndOfChtlJsBlock(const std::string& content, size_t block_start_pos) {
    size_t brace_open_pos = content.find('{', block_start_pos);
    if (brace_open_pos == std::string::npos) return std::string::npos;
    int brace_level = 1;
    size_t scan_pos = brace_open_pos + 1;
    while (scan_pos < content.length() && brace_level > 0) {
        if (content[scan_pos] == '{') brace_level++;
        if (content[scan_pos] == '}') brace_level--;
        scan_pos++;
    }
    return (brace_level == 0) ? scan_pos : std::string::npos;
}

std::string CHTLUnifiedScanner::isolateChtlJs(const std::string& script_content) {
    std::string processed_content;
    size_t cursor = 0;

    while (cursor < script_content.length()) {
        size_t next_kw_pos = std::string::npos;
        std::string found_kw;

        for (const auto& kw : chtljs_keywords_) {
            size_t pos = script_content.find(kw, cursor);
            if (pos != std::string::npos && (next_kw_pos == std::string::npos || pos < next_kw_pos)) {
                if (isKeyword(script_content, pos, kw)) {
                    next_kw_pos = pos;
                    found_kw = kw;
                }
            }
        }

        if (next_kw_pos == std::string::npos) {
            std::string js_part = script_content.substr(cursor);
            if (!js_part.empty()) {
                std::string placeholder = generatePlaceholder();
                placeholder_map_[placeholder] = js_part;
                processed_content += placeholder;
            }
            break;
        }

        if (next_kw_pos > cursor) {
            std::string js_part = script_content.substr(cursor, next_kw_pos - cursor);
            std::string placeholder = generatePlaceholder();
            placeholder_map_[placeholder] = js_part;
            processed_content += placeholder;
        }

        if (found_kw == "{{") {
            size_t construct_end_pos = script_content.find("}}", next_kw_pos);
            if (construct_end_pos != std::string::npos) {
                construct_end_pos += 2;
                processed_content += script_content.substr(next_kw_pos, construct_end_pos - next_kw_pos);
                cursor = construct_end_pos;
            } else {
                 throw std::runtime_error("Unmatched {{ in script.");
            }
        } else { // Block constructs like listen, animate, etc.
            size_t brace_open_pos = script_content.find('{', next_kw_pos);
            size_t construct_end_pos = findEndOfChtlJsBlock(script_content, next_kw_pos);

            if (brace_open_pos == std::string::npos || construct_end_pos == std::string::npos) {
                throw std::runtime_error("Malformed CHTL JS block in script: " + found_kw);
            }

            processed_content += script_content.substr(next_kw_pos, brace_open_pos - next_kw_pos + 1);
            std::string inner_content = script_content.substr(brace_open_pos + 1, construct_end_pos - brace_open_pos - 2);
            processed_content += isolateChtlJs(inner_content);
            processed_content += "}";
            cursor = construct_end_pos;
        }
    }
    return processed_content;
}

} // namespace CHTL
