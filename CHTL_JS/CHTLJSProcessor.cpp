#include "CHTLJSProcessor.h"
#include <stdexcept>

namespace CHTL {

CHTLJSProcessor::CHTLJSProcessor(const std::string& source) : source(source) {}

std::string CHTLJSProcessor::process() {
    while (current_pos < source.length()) {
        size_t start_delim = source.find("{{", current_pos);

        if (start_delim == std::string::npos) {
            // No more selectors found, append the rest of the string
            output_buffer += source.substr(current_pos);
            break;
        }

        // Append the plain JS part before the selector
        output_buffer += source.substr(current_pos, start_delim - current_pos);

        size_t end_delim = source.find("}}", start_delim);
        if (end_delim == std::string::npos) {
            throw std::runtime_error("Unmatched '{{' found in script block.");
        }

        // Extract the selector content
        std::string selector = source.substr(start_delim + 2, end_delim - (start_delim + 2));

        // Trim whitespace from selector
        selector.erase(0, selector.find_first_not_of(" \t\n\r"));
        selector.erase(selector.find_last_not_of(" \t\n\r") + 1);

        // Transform and append
        output_buffer += transformSelector(selector);

        // Move past the processed part
        current_pos = end_delim + 2;
    }
    return output_buffer;
}

std::string CHTLJSProcessor::transformSelector(const std::string& selector_str) {
    std::string selector = selector_str; // Make a mutable copy

    // Check for indexed access, e.g., button[0]
    size_t bracket_pos = selector.find('[');
    if (bracket_pos != std::string::npos) {
        size_t end_bracket_pos = selector.find(']');
        if (end_bracket_pos != std::string::npos) {
            std::string index_str = selector.substr(bracket_pos + 1, end_bracket_pos - bracket_pos - 1);
            std::string base_selector = selector.substr(0, bracket_pos);
            // Basic sanitation, not a full parser
            base_selector.erase(base_selector.find_last_not_of(" \t\n\r") + 1);
            return "document.querySelectorAll('" + base_selector + "')[" + index_str + "]";
        }
    }

    // Check for descendant selectors or simple class/tag selectors
    if (selector.find(' ') != std::string::npos || selector[0] == '.' || (isalpha(selector[0]) && selector.find_first_of(".#[]") == std::string::npos)) {
        return "document.querySelector('" + selector + "')";
    }

    // Handle ID selectors for performance
    if (selector[0] == '#') {
        return "document.getElementById('" + selector.substr(1) + "')";
    }

    // Fallback for any other cases
    return "document.querySelector('" + selector + "')";
}

} // namespace CHTL
