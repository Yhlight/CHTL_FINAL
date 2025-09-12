#ifndef CHTL_EXPRPRINTER_H
#define CHTL_EXPRPRINTER_H

#include "CHTL/CHTLNode/ExprVisitor.h"
#include "CHTL/CHTLNode/ExprNode.h"
#include <string>

namespace CHTL {

class ExprPrinter : public ExprVisitor {
public:
    std::string print(ExprNode* expr);

    void visit(BinaryExpr& expr) override;
    void visit(NumberLiteralExpr& expr) override;
    void visit(GroupingExpr& expr) override;

private:
    std::string result;
};

} // namespace CHTL

#endif // CHTL_EXPRPRINTER_H
