#pragma once

#include <memory>
#include <string>
#include <vector>

// Forward declare all node types and the visitor
class Node;
class ElementNode;
class TextNode;
class CommentNode;
class PropertyNode;
class StyleNode;
class ScriptNode;
class StyleTemplateDefinitionNode;
class ElementTemplateDefinitionNode;
class VarTemplateDefinitionNode;
class TemplateDefinitionNode;
class StyleUsageNode;
class ElementUsageNode;
class CustomStyleDefinitionNode;
class CustomElementDefinitionNode;
class CustomVarDefinitionNode;
class DeleteNode;
class InsertNode;
class ImportNode;
class ConfigurationNode;
class NamespaceNode;
class UseNode;
class ConstraintNode;
class ExportNode;

class Visitor {
public:
    virtual ~Visitor() = default;
    virtual void visit(ElementNode* node) = 0;
    virtual void visit(TextNode* node) = 0;
    virtual void visit(CommentNode* node) = 0;
    virtual void visit(PropertyNode* node) = 0;
    virtual void visit(StyleNode* node) = 0;
    virtual void visit(ScriptNode* node) = 0;
    virtual void visit(StyleTemplateDefinitionNode* node) = 0;
    virtual void visit(ElementTemplateDefinitionNode* node) = 0;
    virtual void visit(VarTemplateDefinitionNode* node) = 0;
    virtual void visit(StyleUsageNode* node) = 0;
    virtual void visit(ElementUsageNode* node) = 0;
    virtual void visit(CustomStyleDefinitionNode* node) = 0;
    virtual void visit(CustomElementDefinitionNode* node) = 0;
    virtual void visit(CustomVarDefinitionNode* node) = 0;
    virtual void visit(DeleteNode* node) = 0;
    virtual void visit(InsertNode* node) = 0;
    virtual void visit(ImportNode* node) = 0;
    virtual void visit(ConfigurationNode* node) = 0;
    virtual void visit(NamespaceNode* node) = 0;
    virtual void visit(UseNode* node) = 0;
    virtual void visit(ConstraintNode* node) = 0;
    virtual void visit(ExportNode* node) = 0;
};

class Node {
public:
    virtual ~Node() = default;
    virtual void accept(Visitor& visitor) = 0;
};

using NodePtr = std::unique_ptr<Node>;
using NodeList = std::vector<NodePtr>;
