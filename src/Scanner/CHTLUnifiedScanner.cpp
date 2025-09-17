#include "CHTLUnifiedScanner.h"
#include <stdexcept>
#include <cctype>
#include <algorithm>
#include <iostream>
#include <sstream>

namespace CHTL {

// --- Keyword Definitions ---
// These are the CHTL JS keywords that the scanner should NOT treat as plain JS.
const std::vector<std::string> CHTLJS_KEYWORDS = {
    "Listen", "Delegate", "Animate", "Vir", "Router", "ScriptLoader",
    "util", "iNeverAway", "printMylove"
};

// --- Constructor ---
CHTLUnifiedScanner::CHTLUnifiedScanner(const std::string& script_content)
    : script_content_(script_content) {}

// --- Public Methods ---
ScannerResult CHTLUnifiedScanner::scan() {
    // This is a simplified placeholder implementation. A robust implementation
    // would require a much more sophisticated JS parser to perfectly distinguish
    // JS blocks from other constructs. This version focuses on replacing content
    // inside curly braces that don't follow a CHTL-JS keyword.

    std::stringstream modified_content;
    size_t cursor = 0;

    while (cursor < script_content_.length()) {
        // Find the next CHTL-JS construct `{{...}}`
        size_t cjjs_open = script_content_.find("{{", cursor);
        if (cjjs_open != std::string::npos) {
            // Everything before this is potentially JS, process it.
            modified_content << createPlaceholder(script_content_.substr(cursor, cjjs_open - cursor));

            // Find the end of the CHTL-JS construct
            size_t cjjs_close = script_content_.find("}}", cjjs_open);
            if (cjjs_close == std::string::npos) {
                throw std::runtime_error("Unmatched '{{' in script block.");
            }
            // Append the CHTL-JS construct directly.
            modified_content << script_content_.substr(cjjs_open, cjjs_close - cjjs_open + 2);
            cursor = cjjs_close + 2;
        } else {
            // No more CHTL-JS constructs found, treat the rest as a single block.
            modified_content << createPlaceholder(script_content_.substr(cursor));
            break;
        }
    }

    return {modified_content.str(), placeholder_map_};
}

// --- Private Helper Methods ---
std::string CHTLUnifiedScanner::createPlaceholder(const std::string& content) {
    if (content.empty()) {
        return "";
    }
    // A simple heuristic: if content is only whitespace, don't create a placeholder.
    if (std::all_of(content.begin(), content.end(), isspace)) {
        return content;
    }

    std::string placeholder = "__CHTL_JS_PLACEHOLDER_" + std::to_string(placeholder_id_++) + "__";
    placeholder_map_[placeholder] = content;
    return placeholder;
}

} // namespace CHTL
