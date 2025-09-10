#ifndef CHTL_STYLE_NODE_H
#define CHTL_STYLE_NODE_H

#include "BaseNode.h"
#include <vector>
#include <memory>

// Represents a style { ... } block.
// Its children can be DeclarationNodes or RuleNodes.
class StyleNode : public BaseNode {
public:
    std::vector<std::unique_ptr<BaseNode>> children;

    void addChild(std::unique_ptr<BaseNode> child) {
        children.push_back(std::move(child));
    }
};

#endif // CHTL_STYLE_NODE_H
