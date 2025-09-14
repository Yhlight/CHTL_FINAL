#ifndef CHTL_JS_ROUTER_NODE_H
#define CHTL_JS_ROUTER_NODE_H

#include "CHTLJSNode.h"
#include "ValueNode.h"
#include <string>
#include <vector>
#include <map>

namespace CHTLJS {

class RouterNode : public CHTLJSNode {
public:
    RouterNode() = default;

    CHTLJSNodeType getType() const override { return CHTLJSNodeType::Router; }

    std::unique_ptr<CHTLJSNode> clone() const override {
        auto new_node = std::make_unique<RouterNode>();
        new_node->mode_ = this->mode_;
        new_node->root_path_ = this->root_path_;
        if (this->root_container_) {
            new_node->root_container_ = std::unique_ptr<ValueNode>(static_cast<ValueNode*>(this->root_container_->clone().release()));
        }
        for (const auto& route : this->routes_) {
            new_node->addRoute(route.first, std::unique_ptr<ValueNode>(static_cast<ValueNode*>(route.second->clone().release())));
        }
        return new_node;
    }

    void setMode(std::string mode) { mode_ = std::move(mode); }
    const std::string& getMode() const { return mode_; }

    void setRootPath(std::string path) { root_path_ = std::move(path); }
    const std::string& getRootPath() const { return root_path_; }

    void setRootContainer(std::unique_ptr<ValueNode> container) { root_container_ = std::move(container); }
    const ValueNode* getRootContainer() const { return root_container_.get(); }

    void addRoute(const std::string& url, std::unique_ptr<ValueNode> page) {
        routes_[url] = std::move(page);
    }
    const std::map<std::string, std::unique_ptr<ValueNode>>& getRoutes() const { return routes_; }


private:
    std::string mode_; // "history" or "hash"
    std::string root_path_;
    std::unique_ptr<ValueNode> root_container_;
    std::map<std::string, std::unique_ptr<ValueNode>> routes_; // map<url, page_selector>
};

} // namespace CHTLJS

#endif // CHTL_JS_ROUTER_NODE_H
