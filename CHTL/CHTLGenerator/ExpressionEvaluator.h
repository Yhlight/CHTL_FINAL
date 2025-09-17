#ifndef EXPRESSION_EVALUATOR_H
#define EXPRESSION_EVALUATOR_H

#include "../CHTLNode/AST.h"
#include <string>
#include <stdexcept>
#include <stack>

// Represents a calculated CSS value with a number and a unit.
struct CssValue {
    double value = 0.0;
    std::string unit = "";
};

// This visitor walks an expression tree and calculates a final CssValue.
class ExpressionEvaluator : public Visitor {
public:
    // The main entry point to evaluate an expression tree.
    CssValue evaluate(ExpressionNode& node);

    // Expression-related visitors
    void visit(BinaryOpNode& node) override;
    void visit(DimensionNode& node) override;

    // All other visit methods are irrelevant for this visitor and will be empty.
    void visit(ProgramNode& node) override {}
    void visit(ElementNode& node) override {}
    void visit(AttributeNode& node) override {}
    void visit(TextNode& node) override {}
    void visit(CommentNode& node) override {}
    void visit(StringLiteralNode& node) override {}
    void visit(UnquotedLiteralNode& node) override {}
    void visit(NumberLiteralNode& node) override {}
    void visit(StyleBlockNode& node) override {}
    void visit(StylePropertyNode& node) override {}
    void visit(StyleRuleNode& node) override {}
    void visit(TemplateStyleNode& node) override {}
    void visit(TemplateElementNode& node) override {}
    void visit(UseStyleNode& node) override {}
    void visit(UseElementNode& node) override {}

private:
    // We use a stack to store intermediate results during the tree traversal.
    std::stack<CssValue> value_stack;
};

#endif // EXPRESSION_EVALUATOR_H
