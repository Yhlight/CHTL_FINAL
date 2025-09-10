#ifndef CHTL_EVALUATOR_H
#define CHTL_EVALUATOR_H

#include "../CHTLNode/ExpressionNode.h"
#include "../CHTLNode/LiteralNode.h"
#include "../CHTLNode/BinaryOpNode.h"
#include "../CHTLNode/CallExpressionNode.h"
#include "../CHTLStore/TemplateStore.h"
#include <string>

class Evaluator {
public:
    explicit Evaluator(const TemplateStore& templateStore);
    // Main entry point for evaluation
    std::string evaluate(ExpressionNode* node);

private:
    const TemplateStore& m_templateStore;
    // Specific evaluation functions for each node type
    std::string evaluateLiteralNode(const LiteralNode* node);
    std::string evaluateBinaryOpNode(const BinaryOpNode* node);
    std::string evaluateCallExpression(const CallExpressionNode* node);
};

#endif //CHTL_EVALUATOR_H
