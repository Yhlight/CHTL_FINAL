#ifndef CHTL_JS_LISTEN_NODE_H
#define CHTL_JS_LISTEN_NODE_H

#include "CHTLJSNode.h"
#include <string>
#include <vector>
#include <map>
#include <utility>

namespace CHTLJS {

class ListenNode : public CHTLJSNode {
public:
    explicit ListenNode(std::unique_ptr<CHTLJSNode> object) : object_(std::move(object)) {}

    CHTLJSNodeType getType() const override { return CHTLJSNodeType::Listen; }

    std::unique_ptr<CHTLJSNode> clone() const override {
        auto new_node = std::make_unique<ListenNode>(object_->clone());
        new_node->events_ = this->events_;
        return new_node;
    }

    const CHTLJSNode* getObject() const { return object_.get(); }
    const std::map<std::string, std::string>& getEvents() const { return events_; }

    void addEvent(std::string event_name, std::string callback_body) {
        events_[std::move(event_name)] = std::move(callback_body);
    }

private:
    std::unique_ptr<CHTLJSNode> object_; // The object to attach the listener to (e.g., an EnhancedSelectorNode)
    std::map<std::string, std::string> events_; // map<eventName, callbackBody>
};

} // namespace CHTLJS

#endif // CHTL_JS_LISTEN_NODE_H
