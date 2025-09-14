#ifndef CHTL_JS_ROUTER_NODE_H
#define CHTL_JS_ROUTER_NODE_H

#include "CHTLJS/CHTLJSNode/CHTLJSNode.h"
#include <string>
#include <vector>
#include <utility>
#include <memory>
#include <optional>

namespace CHTLJS {

struct Route {
    std::string url;
    std::unique_ptr<CHTLJSNode> page_node;
};

class RouterNode : public CHTLJSNode {
public:
    RouterNode() : CHTLJSNode(CHTLJSNodeType::Router) {}

    std::unique_ptr<CHTLJSNode> clone() const override {
        auto new_node = std::make_unique<RouterNode>();
        for (const auto& route : routes_) {
            new_node->routes_.push_back({route.url, route.page_node ? route.page_node->clone() : nullptr});
        }
        if (root_container_) {
            new_node->root_container_ = root_container_->clone();
        }
        new_node->root_path_ = root_path_;
        new_node->mode_ = mode_;
        return new_node;
    }

    std::vector<Route> routes_;
    std::unique_ptr<CHTLJSNode> root_container_;
    std::optional<std::string> root_path_;
    std::optional<std::string> mode_;
};

} // namespace CHTLJS

#endif // CHTL_JS_ROUTER_NODE_H
