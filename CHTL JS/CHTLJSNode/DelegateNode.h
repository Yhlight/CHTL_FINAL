#ifndef DELEGATE_NODE_H
#define DELEGATE_NODE_H

#include "CHTLJSBaseNode.h"
#include "EnhancedSelectorNode.h"
#include <string>
#include <vector>
#include <map>

namespace CHTL_JS {

class DelegateNode : public CHTLJSBaseNode {
public:
    DelegateNode(const ParsedSelector& parent_selector)
        : parent_selector(parent_selector) {
        type = CHTLJSNodeType::Delegate;
    }

    void accept(Visitor& visitor) override {
        visitor.visit(*this);
    }

    ParsedSelector parent_selector;
    std::vector<ParsedSelector> target_selectors;
    std::map<std::string, std::string> events;
};

} // namespace CHTL_JS

#endif // DELEGATE_NODE_H
