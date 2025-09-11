#pragma once

#include "../CHTLNode/Node.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/AttributeNode.h"
#include "../CHTLNode/TextNode.h"
#include "../CHTLNode/StyleNode.h"
#include "../CHTLNode/ExpressionNode.h"
#include "../CHTLObject/Object.h"
#include <string>
#include <sstream>
#include <map>

namespace CHTL {

using EvalContext = std::map<std::string, ObjectPtr>;

class CHTLGenerator {
public:
    CHTLGenerator(NodePtr rootNode);

    std::string Generate();

private:
    // Main visitor methods
    void visit(const NodePtr& node);
    void visitElement(const std::shared_ptr<ElementNode>& node);
    void visitText(const std::shared_ptr<TextNode>& node);
    void visitAttribute(const std::shared_ptr<AttributeNode>& node);

    // Expression generation (for calc() etc.)
    std::string generateExpression(const ExpressionPtr& node, bool is_sub_expression = false);

    // Expression evaluation (for conditionals)
    ObjectPtr evaluate(const ExpressionPtr& node, EvalContext& context);
    ObjectPtr evaluateInfixExpression(const std::string& op, const ObjectPtr& left, const ObjectPtr& right);

    // Member variables
    NodePtr m_rootNode;
    std::stringstream m_output;
    std::stringstream m_global_css;
};

} // namespace CHTL
