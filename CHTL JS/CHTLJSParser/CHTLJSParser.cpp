#include "CHTLJSParser.h"
#include "../CHTLJSNode/RawJSNode.h"
#include "../CHTLJSNode/EnhancedSelectorNode.h"
#include <algorithm>
#include <string>
#include <stdexcept>

namespace CHTL_JS {

// Helper function to trim whitespace from the start and end of a string
std::string trim(const std::string& s) {
    size_t first = s.find_first_not_of(" \t\n\r");
    if (std::string::npos == first) {
        return s;
    }
    size_t last = s.find_last_not_of(" \t\n\r");
    return s.substr(first, (last - first + 1));
}


CHTLJSParser::CHTLJSParser(const std::string& source) : source(source) {}

std::vector<std::unique_ptr<CHTLJSBaseNode>> CHTLJSParser::parse() {
    std::vector<std::unique_ptr<CHTLJSBaseNode>> nodes;
    size_t last_pos = 0;

    while (last_pos < source.length()) {
        size_t start_delim = source.find("{{", last_pos);

        if (start_delim == std::string::npos) {
            if (last_pos < source.length()) {
                nodes.push_back(std::make_unique<RawJSNode>(source.substr(last_pos)));
            }
            break;
        }

        if (start_delim > last_pos) {
            nodes.push_back(std::make_unique<RawJSNode>(source.substr(last_pos, start_delim - last_pos)));
        }

        size_t end_delim = source.find("}}", start_delim);
        if (end_delim == std::string::npos) {
            nodes.push_back(std::make_unique<RawJSNode>(source.substr(start_delim)));
            break;
        }

        std::string selector_content = source.substr(start_delim + 2, end_delim - (start_delim + 2));
        auto selector_node = std::make_unique<EnhancedSelectorNode>(selector_content);

        // Parse the selector content
        std::string trimmed_content = trim(selector_content);
        size_t bracket_pos = trimmed_content.find('[');
        if (bracket_pos != std::string::npos) {
            selector_node->parsed_selector.type = SelectorType::IndexedQuery;
            selector_node->parsed_selector.selector_string = trim(trimmed_content.substr(0, bracket_pos));
            size_t end_bracket_pos = trimmed_content.find(']', bracket_pos);
            if (end_bracket_pos != std::string::npos) {
                std::string index_str = trimmed_content.substr(bracket_pos + 1, end_bracket_pos - (bracket_pos + 1));
                selector_node->parsed_selector.index = std::stoi(index_str);
            }
        } else {
            selector_node->parsed_selector.type = SelectorType::Query;
            selector_node->parsed_selector.selector_string = trimmed_content;
        }

        nodes.push_back(std::move(selector_node));
        last_pos = end_delim + 2;
    }

    return nodes;
}

} // namespace CHTL_JS
