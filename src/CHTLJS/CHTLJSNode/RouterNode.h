#ifndef CHTL_JS_ROUTER_NODE_H
#define CHTL_JS_ROUTER_NODE_H

#include "CHTLJSNode.h"
#include "ValueNode.h"
#include "EnhancedSelectorNode.h"
#include <string>
#include <vector>
#include <memory>
#include <optional>

namespace CHTLJS {

// Represents a single route mapping, e.g., { url: "/home", page: {{selector}} }
struct RoutePair {
    std::unique_ptr<ValueNode> url;
    std::unique_ptr<EnhancedSelectorNode> page;
};

// Represents the root configuration, e.g., { root: "/", container: {{app}} }
struct RootConfig {
    std::optional<std::unique_ptr<ValueNode>> path;
    std::optional<std::unique_ptr<EnhancedSelectorNode>> container;
};

class RouterNode : public CHTLJSNode {
public:
    RouterNode() : CHTLJSNode(CHTLJSNodeType::Router) {}

    // A list of all defined routes
    std::vector<RoutePair> routes;

    // The overall root configuration
    std::optional<RootConfig> root_config;

    // The routing mode, e.g., "history" or "hash"
    std::optional<std::string> mode;
};

} // namespace CHTLJS

#endif // CHTL_JS_ROUTER_NODE_H
