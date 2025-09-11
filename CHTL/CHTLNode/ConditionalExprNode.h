#pragma once

#include "ExpressionNode.h"

namespace CHTL {

class ConditionalExprNode : public ExpressionNode {
public:
    ConditionalExprNode(ExpressionNodePtr condition, ExpressionNodePtr trueBranch, ExpressionNodePtr falseBranch)
        : m_condition(std::move(condition)),
          m_trueBranch(std::move(trueBranch)),
          m_falseBranch(std::move(falseBranch)) {}

    ExpressionNodeType GetType() const override { return ExpressionNodeType::Conditional; }

    const ExpressionNodePtr& GetCondition() const { return m_condition; }
    const ExpressionNodePtr& GetTrueBranch() const { return m_trueBranch; }
    const ExpressionNodePtr& GetFalseBranch() const { return m_falseBranch; }

    ExpressionNodePtr Clone() const override {
        return std::make_shared<ConditionalExprNode>(m_condition->Clone(), m_trueBranch->Clone(), m_falseBranch ? m_falseBranch->Clone() : nullptr);
    }

private:
    ExpressionNodePtr m_condition;
    ExpressionNodePtr m_trueBranch;
    ExpressionNodePtr m_falseBranch;
};

} // namespace CHTL
