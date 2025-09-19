#include "CHTLJSParser.h"
#include "../CHTLJSNode/RawJSNode.h"
#include "../CHTLJSNode/EnhancedSelectorNode.h"

namespace CHTL_JS {

CHTLJSParser::CHTLJSParser(const std::string& source) : source(source) {}

std::vector<std::unique_ptr<CHTLJSBaseNode>> CHTLJSParser::parse() {
    std::vector<std::unique_ptr<CHTLJSBaseNode>> nodes;
    size_t last_pos = 0;

    while (last_pos < source.length()) {
        size_t start_delim = source.find("{{", last_pos);

        if (start_delim == std::string::npos) {
            // No more delimiters, the rest is raw JS
            if (last_pos < source.length()) {
                nodes.push_back(std::make_unique<RawJSNode>(source.substr(last_pos)));
            }
            break;
        }

        // Add the raw JS part before the delimiter
        if (start_delim > last_pos) {
            nodes.push_back(std::make_unique<RawJSNode>(source.substr(last_pos, start_delim - last_pos)));
        }

        size_t end_delim = source.find("}}", start_delim);
        if (end_delim == std::string::npos) {
            // Unterminated selector, treat the rest as raw JS
            nodes.push_back(std::make_unique<RawJSNode>(source.substr(start_delim)));
            break;
        }

        // Add the enhanced selector node
        std::string selector_content = source.substr(start_delim + 2, end_delim - (start_delim + 2));
        nodes.push_back(std::make_unique<EnhancedSelectorNode>(selector_content));

        last_pos = end_delim + 2;
    }

    return nodes;
}

} // namespace CHTL_JS
