#ifndef CHTL_EVALUATOR_H
#define CHTL_EVALUATOR_H

#include "../CHTLNode/ExpressionNode.h"
#include "../CHTLNode/LiteralNode.h"
#include "../CHTLNode/BinaryOpNode.h"
#include <string>

class Evaluator {
public:
    // Main entry point for evaluation
    std::string evaluate(ExpressionNode* node);

private:
    // Specific evaluation functions for each node type
    std::string evaluateLiteralNode(const LiteralNode* node);
    std::string evaluateBinaryOpNode(const BinaryOpNode* node);
};

#endif //CHTL_EVALUATOR_H
