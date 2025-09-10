#pragma once

#include "Node.h"
#include "TemplateNode.h"
#include <string>
#include <vector>
#include <memory>

// Forward declarations
class CustomStyleDefinitionNode;
class CustomElementDefinitionNode;
class CustomVarDefinitionNode;
class DeleteNode;
class InsertNode;



// --- Custom Definitions ---
// These inherit from their template counterparts.
class CustomStyleDefinitionNode : public StyleTemplateDefinitionNode {
public:
    using StyleTemplateDefinitionNode::StyleTemplateDefinitionNode;
    void accept(Visitor& visitor) override { visitor.visit(this); }
};

class CustomElementDefinitionNode : public ElementTemplateDefinitionNode {
public:
    using ElementTemplateDefinitionNode::ElementTemplateDefinitionNode;
    void accept(Visitor& visitor) override { visitor.visit(this); }
};

class CustomVarDefinitionNode : public VarTemplateDefinitionNode {
public:
    using VarTemplateDefinitionNode::VarTemplateDefinitionNode;
    void accept(Visitor& visitor) override { visitor.visit(this); }
};


// --- Specialization Nodes ---
class SpecializationNode : public Node {};

class DeleteNode : public SpecializationNode {
public:
    // The target could be a property name, an element selector, etc.
    DeleteNode(const std::string& target) : target(target) {}
    void accept(Visitor& visitor) override { visitor.visit(this); }
    std::string target;
};

class InsertNode : public SpecializationNode {
public:
    enum Position { After, Before, Replace, AtTop, AtBottom };
    InsertNode(Position position, const std::string& selector, NodeList children)
        : position(position), selector(selector), children(std::move(children)) {}

    void accept(Visitor& visitor) override { visitor.visit(this); }

    Position position;
    std::string selector;
    NodeList children;
};
