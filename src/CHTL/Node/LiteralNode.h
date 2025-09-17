#ifndef CHTL_LITERAL_NODE_H
#define CHTL_LITERAL_NODE_H

#include "BaseNode.h"
#include <string>

namespace CHTL {

// Represents a simple literal value (string, unquoted literal, number, etc.)
class LiteralNode : public BaseNode {
public:
    std::string value;

    std::shared_ptr<BaseNode> clone() const override {
        auto node = std::make_shared<LiteralNode>();
        node->value = this->value;
        return node;
    }
};

} // namespace CHTL

#endif // CHTL_LITERAL_NODE_H
