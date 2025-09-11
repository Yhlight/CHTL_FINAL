#pragma once

#include "CHTLNode/BaseNode.h"
#include "CHTLNode/ElementNode.h"
#include "CHTLNode/TextNode.h"
#include "CHTLNode/CommentNode.h"
#include "CHTLNode/StyleNode.h"
#include "CHTLNode/ExpressionNode.h"
#include "CHTLNode/TemplateDefinitionNode.h"
#include "CHTLNode/OriginNode.h"
#include "CHTLNode/ElementTemplateUsageNode.h"
#include "CHTLNode/ElementModificationNode.h"
#include "CHTLNode/ElementDeleteNode.h"
#include "CHTLNode/ElementInsertNode.h"
#include "CHTLNode/PropertyReferenceNode.h"
#include "CHTLNode/SelfPropertyReferenceNode.h"
#include "CHTLNode/BinaryOpNode.h"
#include "CHTLNode/ConditionalExprNode.h"
#include "CHTLNode/VariableUsageNode.h"
#include <string>
#include <sstream>
#include <variant>
#include <map>

namespace CHTL {

class CompilationContext; // Forward declaration

// A struct to hold the result of an expression evaluation
enum class ValueType { Number, String, Bool };
struct EvaluatedValue {
    ValueType type;
    std::variant<double, std::string, bool> value;
    std::string unit; // for numbers
};


class CHTLGenerator {
public:
    explicit CHTLGenerator(const CompilationContext& context);
    std::string Generate(const NodeList& ast);

private:
    void Visit(const NodePtr& node);
    void VisitElement(const ElementNode* node);
    void VisitText(const TextNode* node);
    void VisitComment(const CommentNode* node);
    void VisitStyle(const StyleNode* node, const ElementNode* context);
    void VisitStyleRule(const StyleRuleNode* node, const ElementNode* context, const std::string& primary_selector);
    void VisitTemplateDefinition(const TemplateDefinitionNode* node);
    void VisitOrigin(const OriginNode* node);
    void VisitElementTemplateUsage(const ElementTemplateUsageNode* node);

    // Element Specialization
    void ApplyModification(NodeList& nodes, const ElementModificationNode* mod);
    void ApplyDeletion(NodeList& nodes, const ElementDeleteNode* del);
    void ApplyInsertion(NodeList& nodes, const ElementInsertNode* ins);
    NodePtr FindNodeBySelector(NodeList& nodes, const std::string& selector);
    const ElementNode* FindElement(const NodePtr& root, const std::string& selector);

    // Expression evaluation
    EvaluatedValue EvaluateExpression(const ExpressionNodePtr& expr, const ElementNode* context);
    EvaluatedValue VisitBinaryOp(const BinaryOpNode* node, const ElementNode* context);
    EvaluatedValue VisitConditionalExpr(const ConditionalExprNode* node, const ElementNode* context);
    EvaluatedValue VisitVariableUsage(const VariableUsageNode* node, const ElementNode* context);
    EvaluatedValue VisitPropertyReference(const PropertyReferenceNode* node);
    EvaluatedValue VisitSelfPropertyReference(const SelfPropertyReferenceNode* node, const ElementNode* context);
    std::vector<Property> ExpandStyleTemplate(const std::string& templateName, const std::shared_ptr<StyleNode>& specialization);

    std::string escapeHTML(const std::string& data);

    NodePtr m_astRoot;
    const CompilationContext& m_context;
    std::stringstream m_output_body;
    std::stringstream m_output_css;
    std::stringstream m_output_js;
};

} // namespace CHTL
