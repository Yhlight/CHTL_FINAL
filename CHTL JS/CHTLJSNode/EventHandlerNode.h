#ifndef EVENT_HANDLER_NODE_H
#define EVENT_HANDLER_NODE_H

#include "CHTLJSBaseNode.h"
#include "EnhancedSelectorNode.h"
#include <string>
#include <vector>

namespace CHTL_JS {

class EventHandlerNode : public CHTLJSBaseNode {
public:
    EventHandlerNode(const ParsedSelector& selector, const std::vector<std::string>& event_names, const std::string& handler)
        : selector(selector), event_names(event_names), handler(handler) {
        type = CHTLJSNodeType::EventHandler;
    }

    void accept(Visitor& visitor) override {
        visitor.visit(*this);
    }

    ParsedSelector selector;
    std::vector<std::string> event_names;
    std::string handler;
};

} // namespace CHTL_JS

#endif // EVENT_HANDLER_NODE_H
