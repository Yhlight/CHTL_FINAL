#ifndef CHTL_PROPERTY_REFERENCE_NODE_H
#define CHTL_PROPERTY_REFERENCE_NODE_H

#include "BaseNode.h"
#include "../CHTLLexer/Token.h"
#include <string>
#include <vector>

namespace CHTL {

class PropertyReferenceNode : public Node {
public:
    PropertyReferenceNode(const Token& selector, const Token& property)
        : selector_(selector), property_(property) {}

    NodeType getType() const override { return NodeType::PropertyReference; }

    std::unique_ptr<Node> clone() const override {
        return std::make_unique<PropertyReferenceNode>(selector_, property_);
    }

    Token selector_;
    Token property_;
};

} // namespace CHTL

#endif // CHTL_PROPERTY_REFERENCE_NODE_H
