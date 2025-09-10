#pragma once

#include "CHTLNode/ExprNode.h"
#include <string>
#include <sstream>

class ExprGenerator : public ExprVisitor {
public:
    std::string generate(ExprNode* expr);

    void visit(LiteralExprNode* node) override;
    void visit(BinaryExprNode* node) override;
    void visit(TernaryExprNode* node) override;

private:
    void parenthesize(const std::string& name, ExprNode* expr);

    std::stringstream output;
};
