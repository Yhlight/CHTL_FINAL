#pragma once

#include <memory>

namespace CHTL {

// Forward declaration
class ExpressionVisitor;

enum class ExpressionNodeType {
    NumberLiteral,
    StringLiteral,
    PropertyReference,
    TemplateUsage,
    VariableUsage,
    BinaryOp
};

class ExpressionNode {
public:
    virtual ~ExpressionNode() = default;
    virtual ExpressionNodeType GetType() const = 0;
    // virtual Value Accept(ExpressionVisitor* visitor) = 0;
};

using ExpressionNodePtr = std::shared_ptr<ExpressionNode>;

} // namespace CHTL
