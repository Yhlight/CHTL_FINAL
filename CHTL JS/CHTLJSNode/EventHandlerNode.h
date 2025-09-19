#ifndef EVENT_HANDLER_NODE_H
#define EVENT_HANDLER_NODE_H

#include "CHTLJSBaseNode.h"
#include "EnhancedSelectorNode.h"
#include <string>

namespace CHTL_JS {

class EventHandlerNode : public CHTLJSBaseNode {
public:
    EventHandlerNode(const ParsedSelector& selector, const std::string& event_name, const std::string& handler)
        : selector(selector), event_name(event_name), handler(handler) {
        type = CHTLJSNodeType::EventHandler;
    }

    ParsedSelector selector;
    std::string event_name;
    std::string handler;
};

} // namespace CHTL_JS

#endif // EVENT_HANDLER_NODE_H
