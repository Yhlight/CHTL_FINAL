#include "CHTLJSplitter.h"
#include <stdexcept>

namespace CHTLJS {

// These are the keywords that signify the start of a CHTL JS construct.
const std::vector<std::string> CHTLJSplitter::CHTLJS_KEYWORDS = {
    "{{", "listen", "animate", "delegate", "vir", "router", "->"
};

CHTLJSplitter::CHTLJSplitter(const std::string& source) : source_(source) {}

JsSplitResult CHTLJSplitter::split() {
    while (cursor_ < source_.length()) {
        process();
    }
    flushJsChunk(source_.length());
    return result_;
}

void CHTLJSplitter::process() {
    size_t construct_pos = findNextChtlJsConstruct();

    if (construct_pos == std::string::npos) {
        // No more CHTL JS, the rest is pure JS
        flushJsChunk(source_.length());
        cursor_ = source_.length();
        return;
    }

    // We found a CHTL JS construct. Flush everything before it as pure JS.
    flushJsChunk(construct_pos);

    // Now, find the end of the CHTL JS construct and append it to the result.
    // This requires a similar logic to the main scanner's findEndOfBlock/findEndOfConstruct.
    // For now, we'll use a simplified version.

    std::string keyword;
    for(const auto& kw : CHTLJS_KEYWORDS) {
        if (source_.rfind(kw, construct_pos) == construct_pos) {
            keyword = kw;
            break;
        }
    }

    size_t end_of_construct = std::string::npos;
    if (keyword == "{{") {
        end_of_construct = source_.find("}}", construct_pos);
        if (end_of_construct != std::string::npos) end_of_construct += 2; // include the "}}"
    } else if (keyword == "->" || keyword == "vir") {
        // These are more complex and require proper parsing, but for now,
        // we can assume they end at a semicolon or newline for simplicity.
        end_of_construct = source_.find_first_of(";\n", construct_pos);
    } else { // listen, animate, delegate, router
        size_t block_start = source_.find('{', construct_pos);
        if (block_start != std::string::npos) {
            int brace_level = 1;
            size_t temp_cursor = block_start + 1;
            while (temp_cursor < source_.length() && brace_level > 0) {
                if (source_[temp_cursor] == '{') brace_level++;
                else if (source_[temp_cursor] == '}') brace_level--;
                temp_cursor++;
            }
            end_of_construct = temp_cursor;
        }
    }

    if (end_of_construct == std::string::npos) {
        // Could not find end, treat rest of file as JS
        flushJsChunk(source_.length());
        cursor_ = source_.length();
        return;
    }

    // Append the CHTL JS construct directly
    result_.cjs_with_placeholders += source_.substr(construct_pos, end_of_construct - construct_pos);
    cursor_ = end_of_construct;
    last_flush_pos_ = cursor_;
}

size_t CHTLJSplitter::findNextChtlJsConstruct() {
    size_t earliest_pos = std::string::npos;

    for (const auto& keyword : CHTLJS_KEYWORDS) {
        size_t pos = source_.find(keyword, cursor_);
        if (pos != std::string::npos) {
            if (earliest_pos == std::string::npos || pos < earliest_pos) {
                earliest_pos = pos;
            }
        }
    }
    return earliest_pos;
}

void CHTLJSplitter::flushJsChunk(size_t end) {
    if (last_flush_pos_ >= end) return;
    std::string content = source_.substr(last_flush_pos_, end - last_flush_pos_);
    if (!content.empty() && content.find_first_not_of(" \t\n\r") != std::string::npos) {
        std::string placeholder = "__JS_PLACEHOLDER_" + std::to_string(placeholder_id_++) + "__";
        result_.js_placeholder_map[placeholder] = content;
        result_.cjs_with_placeholders += placeholder;
    }
    last_flush_pos_ = end;
}

} // namespace CHTLJS
