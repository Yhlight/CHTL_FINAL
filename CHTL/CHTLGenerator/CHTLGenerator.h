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
#include "CHTLNode/TemplateDefinitionNode.h"
#include "CHTLNode/VariableUsageNode.h"
#include <string>
#include <sstream>
#include <variant>
#include <map>

namespace CHTL {

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
    void Visit(const NodePtr& node);
    void VisitElement(const ElementNode* node);
    void VisitText(const TextNode* node);
    void VisitComment(const CommentNode* node);
    void VisitStyle(const StyleNode* node, const ElementNode* context);
    void VisitStyleRule(const StyleRuleNode* node, const ElementNode* context, const std::string& primary_selector);
    void VisitElementTemplateUsage(const ElementTemplateUsageNode* node);
    void VisitTemplateDefinition(const TemplateDefinitionNode* node);
    std::vector<Property> ExpandStyleTemplate(const std::string& templateName, const std::shared_ptr<StyleNode>& specialization);

    // Expression evaluation
    EvaluatedValue EvaluateExpression(const ExpressionNodePtr& expr, const ElementNode* context);
    EvaluatedValue VisitBinaryOp(const BinaryOpNode* node, const ElementNode* context);
    EvaluatedValue VisitConditionalExpr(const ConditionalExprNode* node, const ElementNode* context);
    EvaluatedValue VisitVariableUsage(const VariableUsageNode* node, const ElementNode* context);

    std::string escapeHTML(const std::string& data);

    std::stringstream m_output_body;
    std::stringstream m_output_css;
    std::map<std::string, const TemplateDefinitionNode*> m_template_repo;
};

} // namespace CHTL
