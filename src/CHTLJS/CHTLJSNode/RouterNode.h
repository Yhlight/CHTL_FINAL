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

    std::vector<Route> routes_;
    std::unique_ptr<CHTLJSNode> root_container_;
    std::optional<std::string> root_path_;
    std::optional<std::string> mode_;
};

} // namespace CHTLJS

#endif // CHTL_JS_ROUTER_NODE_H
