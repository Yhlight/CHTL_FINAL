#pragma once

#include <memory>
#include <vector>
#include <string>

class ElementNode;
class TextNode;
class StyleNode;
class TemplateUsageNode;
class CustomStyleTemplateNode;
class StyleRuleNode;
class OriginNode;
class ScriptNode;

class AstVisitor {
public:
    virtual ~AstVisitor() = default;
    virtual void visit(ElementNode& node) = 0;
    virtual void visit(TextNode& node) = 0;
    virtual void visit(StyleNode& node) = 0;
    virtual void visit(TemplateUsageNode& node) = 0;
    virtual void visit(CustomStyleTemplateNode& node) = 0;
    virtual void visit(StyleRuleNode& node) = 0;
    virtual void visit(OriginNode& node) = 0;
    virtual void visit(ScriptNode& node) = 0;
};

class BaseNode {
public:
    BaseNode* parent = nullptr;
    virtual ~BaseNode() = default;
    virtual void accept(AstVisitor& visitor) = 0;
    virtual std::unique_ptr<BaseNode> clone() const = 0;
};
