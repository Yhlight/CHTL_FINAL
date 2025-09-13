#ifndef CHTL_EXPRESSION_NODE_H
#define CHTL_EXPRESSION_NODE_H

#include "BaseNode.h"
#include "PropertyValue.h"
#include <memory>
#include <vector>
#include <variant>
#include "../CHTLLexer/Token.h"
#include "PropertyReferenceNode.h"

namespace CHTL {

// Forward declaration for recursive definition
struct ArithmeticNode;
struct ConditionalNode;

// Using a visitor to clone the variant
struct PropertyValueCloner {
    std::variant<Token, PropertyReferenceNode, std::unique_ptr<ArithmeticNode>, std::unique_ptr<ConditionalNode>> operator()(const Token& t) const;
    std::variant<Token, PropertyReferenceNode, std::unique_ptr<ArithmeticNode>, std::unique_ptr<ConditionalNode>> operator()(const PropertyReferenceNode& prn) const;
    std::variant<Token, PropertyReferenceNode, std::unique_ptr<ArithmeticNode>, std::unique_ptr<ConditionalNode>> operator()(const std::unique_ptr<ArithmeticNode>& an) const;
    std::variant<Token, PropertyReferenceNode, std::unique_ptr<ArithmeticNode>, std::unique_ptr<ConditionalNode>> operator()(const std::unique_ptr<ConditionalNode>& cn) const;
};


// Represents a binary arithmetic operation
struct ArithmeticNode : public Node {
    std::vector<PropertyValue> left;
    Token op;
    std::vector<PropertyValue> right;

    NodeType getType() const override { return NodeType::Expression; }
    std::unique_ptr<Node> clone() const override;
};

// Represents a ternary conditional operation
struct ConditionalNode : public Node {
    std::vector<PropertyValue> condition;
    std::vector<PropertyValue> true_branch;
    std::vector<PropertyValue> false_branch;

    NodeType getType() const override { return NodeType::Expression; }
    std::unique_ptr<Node> clone() const override;
};

// Inline implementations for the cloner after structs are defined
inline std::variant<Token, PropertyReferenceNode, std::unique_ptr<ArithmeticNode>, std::unique_ptr<ConditionalNode>> PropertyValueCloner::operator()(const Token& t) const {
     return t;
}

inline std::variant<Token, PropertyReferenceNode, std::unique_ptr<ArithmeticNode>, std::unique_ptr<ConditionalNode>> PropertyValueCloner::operator()(const PropertyReferenceNode& prn) const {
     return prn;
}

inline std::variant<Token, PropertyReferenceNode, std::unique_ptr<ArithmeticNode>, std::unique_ptr<ConditionalNode>> PropertyValueCloner::operator()(const std::unique_ptr<ArithmeticNode>& an) const {
    if (!an) return std::unique_ptr<ArithmeticNode>(nullptr);
    return std::unique_ptr<ArithmeticNode>(static_cast<ArithmeticNode*>(an->clone().release()));
}

inline std::variant<Token, PropertyReferenceNode, std::unique_ptr<ArithmeticNode>, std::unique_ptr<ConditionalNode>> PropertyValueCloner::operator()(const std::unique_ptr<ConditionalNode>& cn) const {
    if (!cn) return std::unique_ptr<ConditionalNode>(nullptr);
    return std::unique_ptr<ConditionalNode>(static_cast<ConditionalNode*>(cn->clone().release()));
}


} // namespace CHTL

#endif // CHTL_EXPRESSION_NODE_H
