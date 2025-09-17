#ifndef CHTL_JS_DELEGATE_NODE_H
#define CHTL_JS_DELEGATE_NODE_H

#include "CHTLJSNode.h"
#include <string>
#include <vector>
#include <map>
#include <memory>

namespace CHTLJS {

class DelegateNode : public CHTLJSNode {
public:
    explicit DelegateNode(std::unique_ptr<CHTLJSNode> delegator)
        : CHTLJSNode(CHTLJSNodeType::Delegate), delegator_(std::move(delegator)) {}

    const CHTLJSNode* getDelegator() const { return delegator_.get(); }
    const std::vector<std::unique_ptr<CHTLJSNode>>& getTargets() const { return targets_; }
    const std::map<std::string, std::string>& getEvents() const { return events_; }

    void addTarget(std::unique_ptr<CHTLJSNode> target) {
        targets_.push_back(std::move(target));
    }

    void addEvent(std::string event_name, std::string callback_body) {
        events_[std::move(event_name)] = std::move(callback_body);
    }

private:
    std::unique_ptr<CHTLJSNode> delegator_; // The parent element to attach the listener to
    std::vector<std::unique_ptr<CHTLJSNode>> targets_; // The target elements to check for
    std::map<std::string, std::string> events_; // map<eventName, callbackBody>
};

} // namespace CHTLJS

#endif // CHTL_JS_DELEGATE_NODE_H
