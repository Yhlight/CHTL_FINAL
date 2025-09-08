#pragma once

#include "../CHTLNode/BaseNode.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/TextNode.h"
#include "../CHTLNode/StyleNode.h"
#include "../CHTLNode/TemplateUsageNode.h"
#include "../CHTLNode/ExpressionNode.h"
#include "../CHTLJS/Preprocessor.h"
#include <string>
#include <sstream>

class Parser;
class ConfigurationState;

class Generator : public AstVisitor {
public:
    Generator(Parser& parser, std::shared_ptr<ConfigurationState> config);
    std::string generate(ElementNode& root);

    void visit(ElementNode& node) override;
    void visit(TextNode& node) override;
    void visit(StyleNode& node) override;
    void visit(TemplateUsageNode& node) override;
    void visit(class CustomStyleTemplateNode& node) override;
    void visit(class StyleRuleNode& node) override;
    void visit(class OriginNode& node) override;
    void visit(class ScriptNode& node) override;

private:
    Parser& parser;
    CHTLJS_Preprocessor js_preprocessor;
    std::stringstream output;
    std::stringstream global_css;
    std::stringstream global_js;
    ElementNode* currentElement = nullptr; // Context for expression evaluation

    // Expression Evaluation
    struct Value {
        double number = 0;
        std::string string;
        bool isNumeric = false;
    };

    Value evaluateExpression(ExpressionNode& node);
    Value evaluateBinaryOp(BinaryOpNode& node);
    Value evaluateTernaryOp(TernaryOpNode& node);
    Value evaluatePropertyAccess(PropertyAccessNode& node);
    Value evaluateLiteral(LiteralNode& node);
};
