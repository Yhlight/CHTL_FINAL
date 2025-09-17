#pragma once

#include "../CHTLNode/BaseNode.h"
#include "../CHTLNode/RootNode.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/AttributeNode.h"
#include "../CHTLNode/TextNode.h"
#include "../CHTLNode/StyleNode.h"
#include "../CHTLNode/PropertyNode.h"
#include "../CHTLNode/RuleNode.h"
#include "../CHTLNode/TemplateDefinitionNode.h"
#include "../CHTLNode/TemplateUsageNode.h"
#include "../CHTLNode/Expression/BinaryExprNode.h"
#include "../CHTLNode/Expression/LiteralExprNode.h"
#include <memory>

class ExprCloner : public ExprVisitor {
public:
    std::shared_ptr<ExprNode> clonedExpr;
    std::shared_ptr<ExprNode> clone(std::shared_ptr<ExprNode> expr) {
        if (expr) expr->accept(*this);
        return clonedExpr;
    }
    void visit(BinaryExprNode& expr) override {
        auto left = clone(expr.left);
        auto right = clone(expr.right);
        clonedExpr = std::make_shared<BinaryExprNode>(left, expr.op, right);
    }
    void visit(LiteralExprNode& expr) override {
        clonedExpr = std::make_shared<LiteralExprNode>(expr.value);
    }
};

class AstCloner : public AstVisitor {
public:
    std::shared_ptr<BaseNode> clone(std::shared_ptr<BaseNode> node) {
        if (node) {
            node->accept(*this);
        }
        return lastClonedNode;
    }

    void visit(RootNode& node) override {
        auto newNode = std::make_shared<RootNode>();
        for (const auto& child : node.children) {
            newNode->addChild(clone(child));
        }
        lastClonedNode = newNode;
    }

    void visit(ElementNode& node) override {
        auto newNode = std::make_shared<ElementNode>(node.tagName);
        for (const auto& attr : node.attributes) {
            newNode->addAttribute(std::dynamic_pointer_cast<AttributeNode>(clone(attr)));
        }
        for (const auto& child : node.children) {
            newNode->addChild(clone(child));
        }
        lastClonedNode = newNode;
    }

    void visit(AttributeNode& node) override {
        lastClonedNode = std::make_shared<AttributeNode>(node.key, node.value);
    }

    void visit(TextNode& node) override {
        lastClonedNode = std::make_shared<TextNode>(node.text);
    }

    void visit(StyleNode& node) override {
        auto newNode = std::make_shared<StyleNode>();
        for (const auto& child : node.children) {
            newNode->addChild(clone(child));
        }
        lastClonedNode = newNode;
    }

    void visit(PropertyNode& node) override {
        ExprCloner exprCloner;
        auto clonedValue = exprCloner.clone(node.value);
        lastClonedNode = std::make_shared<PropertyNode>(node.key, clonedValue);
    }

    void visit(RuleNode& node) override {
        auto newNode = std::make_shared<RuleNode>(node.selector);
        for (const auto& prop : node.properties) {
            newNode->addProperty(std::dynamic_pointer_cast<PropertyNode>(clone(prop)));
        }
        lastClonedNode = newNode;
    }

    void visit(TemplateDefinitionNode& node) override { lastClonedNode = nullptr; }
    void visit(TemplateUsageNode& node) override { lastClonedNode = nullptr; }

private:
    std::shared_ptr<BaseNode> lastClonedNode;
};
