#pragma once

#include "CHTLNode/BaseNode.h"
#include "CHTLNode/ElementNode.h"
#include "CHTLNode/TextNode.h"
#include "CHTLNode/CommentNode.h"
#include "CHTLNode/StyleNode.h"
#include "CHTLNode/ExpressionNode.h"
#include "CHTLNode/BinaryOpNode.h"
#include "CHTLNode/NumberLiteralNode.h"
#include "CHTLNode/PropertyReferenceNode.h"
#include "CHTLNode/StringLiteralNode.h"
#include "CHTLNode/ElementTemplateUsageNode.h"
#include "CHTLNode/VariableUsageNode.h"
#include "CHTLNode/ConditionalExprNode.h"
#include "CHTLNode/SelfPropertyReferenceNode.h"
#include <string>
#include <sstream>
#include <map>
#include <variant>

namespace CHTL {

class TemplateDefinitionNode; // Forward declaration
struct StyleProcessingResult; // Forward declaration

// A struct to hold the result of an expression evaluation
enum class ValueType { Number, String, Bool };
struct EvaluatedValue {
    ValueType type;
    std::variant<double, std::string, bool> value;
    std::string unit; // for numbers
};


class CHTLGenerator {
public:
    std::string Generate(const NodeList& ast);

private:
    // Main AST traversal
    void Visit(const NodePtr& node);
    void VisitElement(const ElementNode* node);
    void VisitText(const TextNode* node);
    void VisitComment(const CommentNode* node);
    void VisitTemplateDefinition(const TemplateDefinitionNode* node);
    void VisitElementTemplateUsage(const ElementTemplateUsageNode* node);

    // Style processing helper
    void ProcessStyleNodes(const ElementNode* node, StyleProcessingResult& result);
    std::vector<Property> ExpandStyleTemplate(const std::string& templateName);

    // Element lookup
    const ElementNode* FindElement(const NodePtr& searchRoot, const std::string& selector);


    // Expression evaluation
    EvaluatedValue EvaluateExpression(const ExpressionNodePtr& expr, const ElementNode* currentContext);
    EvaluatedValue VisitExpression(const ExpressionNode* expr, const ElementNode* currentContext);
    EvaluatedValue VisitBinaryOp(const BinaryOpNode* node, const ElementNode* currentContext);
    EvaluatedValue VisitConditionalExpr(const ConditionalExprNode* node, const ElementNode* currentContext);
    EvaluatedValue VisitNumberLiteral(const NumberLiteralNode* node);
    EvaluatedValue VisitStringLiteral(const StringLiteralNode* node);
    EvaluatedValue VisitPropertyReference(const PropertyReferenceNode* node);
    EvaluatedValue VisitVariableUsage(const VariableUsageNode* node);
    EvaluatedValue VisitSelfPropertyReference(const SelfPropertyReferenceNode* node, const ElementNode* currentContext);

    NodePtr m_astRoot; // Root of the tree, for lookups
    std::map<std::string, const TemplateDefinitionNode*> m_template_repo;
    std::stringstream m_output_body;
    std::stringstream m_output_css;
};

} // namespace CHTL
