#ifndef CHTL_JS_LISTEN_NODE_H
#define CHTL_JS_LISTEN_NODE_H

#include "CHTLJSBaseNode.h"
#include "SelectorNode.h"
#include <string>
#include <vector>
#include <utility>

namespace CHTL {
namespace CHTL_JS {

class ListenNode : public CHTLJSNode {
public:
    explicit ListenNode(std::unique_ptr<SelectorNode> target) : target_(std::move(target)) {}

    std::unique_ptr<SelectorNode> target_;
    // For simplicity, we'll store the event handlers as raw strings for now.
    // A real implementation would parse the key-value pairs of event names and JS lambda functions.
    std::string event_handlers_raw_string_;
};

} // namespace CHTL_JS
} // namespace CHTL

#endif // CHTL_JS_LISTEN_NODE_H
