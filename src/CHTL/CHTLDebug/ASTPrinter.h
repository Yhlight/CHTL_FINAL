#ifndef CHTL_AST_PRINTER_H
#define CHTL_AST_PRINTER_H

#include <string>
#include <sstream>

#include "../CHTLNode/BaseNode.h"
#include "../CHTLNode/RootNode.h"
#include "../CHTLNode/PropertyValue.h"
#include "../CHTLParser/ParserContext.h"

namespace CHTL {

// Forward declare all node types to avoid a circular dependency hell
// and to keep this header self-contained for now.
class ElementNode;
class TextNode;
class CommentNode;
class StyleBlockNode;
class ScriptBlockNode;
class CssRuleNode;
class AttributeNode;
class StyleTemplateNode;
class ElementTemplateNode;
class VarTemplateNode;
class OriginNode;
class PropertyReferenceNode;


class ASTPrinter {
public:
    std::string print(const RootNode& root, std::shared_ptr<ParserContext> context);

private:
    void printNode(const Node* node, int indent);

    // Specific print methods for each node type
    void printElement(const ElementNode* node, int indent);
    void printText(const TextNode* node, int indent);
    void printComment(const CommentNode* node, int indent);
    void printStyleBlock(const StyleBlockNode* node, int indent);
    void printCssRule(const CssRuleNode* node, int indent);
    void printStyleTemplate(const StyleTemplateNode* node, int indent);
    void printElementTemplate(const ElementTemplateNode* node, int indent);
    void printVarTemplate(const VarTemplateNode* node, int indent);
    void printOrigin(const OriginNode* node, int indent);
    void printScriptBlock(const ScriptBlockNode* node, int indent);
    void printPropertyReference(const PropertyReferenceNode* node, int indent);

    void printIndentation(int indent);

    std::stringstream out_;
};

} // namespace CHTL

#endif // CHTL_AST_PRINTER_H
