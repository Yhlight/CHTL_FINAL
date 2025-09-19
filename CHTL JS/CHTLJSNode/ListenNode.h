#ifndef LISTEN_NODE_H
#define LISTEN_NODE_H

#include "CHTLJSBaseNode.h"
#include "EnhancedSelectorNode.h"
#include <string>
#include <map>

namespace CHTL_JS {

class ListenNode : public CHTLJSBaseNode {
public:
    ListenNode(const ParsedSelector& selector) : selector(selector) {
        type = CHTLJSNodeType::Listen;
    }

    void accept(Visitor& visitor) override {
        visitor.visit(*this);
    }

    ParsedSelector selector;
    std::map<std::string, std::string> events;
};

} // namespace CHTL_JS

#endif // LISTEN_NODE_H
