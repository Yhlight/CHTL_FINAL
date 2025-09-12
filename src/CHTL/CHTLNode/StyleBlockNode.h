#ifndef CHTL_STYLE_BLOCK_NODE_H
#define CHTL_STYLE_BLOCK_NODE_H

#include "BaseNode.h"
#include <string>
#include <vector>
#include <utility> // For std::pair

namespace CHTL {

class StyleBlockNode : public Node {
public:
    StyleBlockNode() = default;

    NodeType getType() const override { return NodeType::StyleBlock; }

    // Using a simple vector of pairs for now.
    // Key is the CSS property, Value is the CSS value.
    std::vector<std::pair<std::string, std::string>> properties_;
};

} // namespace CHTL

#endif // CHTL_STYLE_BLOCK_NODE_H
