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
#include <string>
#include <sstream>
#include <map>
#include <variant>

namespace CHTL {

class TemplateDefinitionNode; // Forward declaration
struct StyleProcessingResult; // Forward declaration

// A struct to hold the result of an expression evaluation
enum class ValueType { Number, String };
struct EvaluatedValue {
    ValueType type;
    std::variant<double, std::string> value;
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

    // Style processing helper
    void ProcessStyleNodes(const ElementNode* node, StyleProcessingResult& result);

    // Element lookup
    const ElementNode* FindElement(const NodePtr& searchRoot, const std::string& selector);


    // Expression evaluation
    EvaluatedValue EvaluateExpression(const ExpressionNodePtr& expr);
    EvaluatedValue VisitExpression(const ExpressionNode* expr);
    EvaluatedValue VisitBinaryOp(const BinaryOpNode* node);
    EvaluatedValue VisitNumberLiteral(const NumberLiteralNode* node);
    EvaluatedValue VisitStringLiteral(const StringLiteralNode* node);
    EvaluatedValue VisitPropertyReference(const PropertyReferenceNode* node);

    NodePtr m_astRoot; // Root of the tree, for lookups
    std::map<std::string, const TemplateDefinitionNode*> m_template_repo;
    std::stringstream m_output_body;
    std::stringstream m_output_css;
};

} // namespace CHTL
