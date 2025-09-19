#ifndef ENHANCED_SELECTOR_NODE_H
#define ENHANCED_SELECTOR_NODE_H

#include "CHTLJSBaseNode.h"
#include <optional>
#include <string>

namespace CHTL_JS {

enum class SelectorType {
    Query,
    IndexedQuery
};

struct ParsedSelector {
    SelectorType type;
    std::string selector_string;
    std::optional<int> index;
};

class EnhancedSelectorNode : public CHTLJSBaseNode {
public:
    explicit EnhancedSelectorNode(const std::string& selector_content)
        : raw_content(selector_content) {
        type = CHTLJSNodeType::EnhancedSelector;
    }

    std::string raw_content;
    ParsedSelector parsed_selector;
};

} // namespace CHTL_JS

#endif // ENHANCED_SELECTOR_NODE_H
