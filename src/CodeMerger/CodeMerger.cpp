#include "CodeMerger.h"

namespace CHTL {

std::string CodeMerger::substitutePlaceholders(
    const std::string& js_with_placeholders,
    const std::map<std::string, std::string>& placeholder_map) {

    std::string final_js = js_with_placeholders;

    // Substitute placeholders
    for (const auto& pair : placeholder_map) {
        size_t pos = final_js.find(pair.first);
        while (pos != std::string::npos) {
            final_js.replace(pos, pair.first.length(), pair.second);
            // Move past the replaced section to avoid infinite loops with empty replacements
            pos = final_js.find(pair.first, pos + pair.second.length());
        }
    }

    return final_js;
}

} // namespace CHTL
