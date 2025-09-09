#include "CHTL/ASTVisitor.h"

namespace CHTL {

void ASTVisitor::visitNode(ASTNode& node) {
    switch (node.getType()) {
        case ASTNodeType::ELEMENT:
            visitElement(static_cast<ElementNode&>(node));
            break;
        case ASTNodeType::TEXT:
            visitText(static_cast<TextNode&>(node));
            break;
        case ASTNodeType::STYLE_BLOCK:
            visitStyle(static_cast<StyleNode&>(node));
            break;
        case ASTNodeType::SCRIPT_BLOCK:
            visitScript(static_cast<ScriptNode&>(node));
            break;
        case ASTNodeType::TEMPLATE:
            visitTemplate(static_cast<TemplateNode&>(node));
            break;
        case ASTNodeType::CUSTOM:
            visitCustom(static_cast<CustomNode&>(node));
            break;
        case ASTNodeType::ORIGIN:
            visitOrigin(static_cast<OriginNode&>(node));
            break;
        case ASTNodeType::IMPORT:
            visitImport(static_cast<ImportNode&>(node));
            break;
        case ASTNodeType::NAMESPACE:
            visitNamespace(static_cast<NamespaceNode&>(node));
            break;
        case ASTNodeType::CONFIGURATION:
            visitConfiguration(static_cast<ConfigurationNode&>(node));
            break;
        case ASTNodeType::CONSTRAINT:
            visitConstraint(static_cast<ConstraintNode&>(node));
            break;
        case ASTNodeType::USE:
            visitUse(static_cast<UseNode&>(node));
            break;
        case ASTNodeType::STYLE_RULE:
            visitStyleRule(static_cast<StyleRuleNode&>(node));
            break;
        case ASTNodeType::EXPRESSION:
            visitExpression(static_cast<ExpressionNode&>(node));
            break;
        case ASTNodeType::BINARY_OP:
            visitBinaryOp(static_cast<BinaryOpNode&>(node));
            break;
        case ASTNodeType::CONDITIONAL:
            visitConditional(static_cast<ConditionalNode&>(node));
            break;
        case ASTNodeType::REFERENCE:
            visitReference(static_cast<ReferenceNode&>(node));
            break;
        case ASTNodeType::SELECTOR:
            visitSelector(static_cast<SelectorNode&>(node));
            break;
        case ASTNodeType::CHTLJS_FUNCTION:
            visitCHTLJSFunction(static_cast<CHTLJSFunctionNode&>(node));
            break;
        case ASTNodeType::CHTLJS_VIR:
            visitCHTLJSVir(static_cast<CHTLJSVirNode&>(node));
            break;
        default:
            // 对于其他类型的节点，遍历子节点
            for (auto& child : node.getChildren()) {
                visitNode(*child);
            }
            break;
    }
}

} // namespace CHTL