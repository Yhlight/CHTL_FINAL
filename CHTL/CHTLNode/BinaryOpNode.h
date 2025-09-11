#pragma once

#include "ExpressionNode.h"
#include "CHTLLexer/Token.h"

namespace CHTL {

class BinaryOpNode : public ExpressionNode {
public:
    BinaryOpNode(ExpressionNodePtr left, Token op, ExpressionNodePtr right)
        : m_left(std::move(left)), m_operator(op), m_right(std::move(right)) {}

    ExpressionNodeType GetType() const override { return ExpressionNodeType::BinaryOp; }

    const ExpressionNodePtr& GetLeft() const { return m_left; }
    const ExpressionNodePtr& GetRight() const { return m_right; }
    TokenType GetOperator() const { return m_operator.type; }

private:
    ExpressionNodePtr m_left;
    Token m_operator;
    ExpressionNodePtr m_right;
};

} // namespace CHTL
