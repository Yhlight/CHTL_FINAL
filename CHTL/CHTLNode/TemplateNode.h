#pragma once

#include "Node.h"
#include "PropertyNode.h"
#include <string>
#include <vector>
#include <memory>

// Forward declarations
class StyleTemplateDefinitionNode;
class ElementTemplateDefinitionNode;
class VarTemplateDefinitionNode;
class StyleUsageNode;
class ElementUsageNode;




// --- Template Definitions ---

class TemplateDefinitionNode : public Node {
public:
    TemplateDefinitionNode(const std::string& name) : name(name) {}
    std::string name;
};

class StyleTemplateDefinitionNode : public TemplateDefinitionNode {
public:
    using TemplateDefinitionNode::TemplateDefinitionNode;

    void accept(Visitor& visitor) override { visitor.visit(this); }

    std::vector<std::unique_ptr<PropertyNode>> properties;
};

class ElementTemplateDefinitionNode : public TemplateDefinitionNode {
public:
    using TemplateDefinitionNode::TemplateDefinitionNode;

    void accept(Visitor& visitor) override { visitor.visit(this); }

    NodeList children;
};

class VarTemplateDefinitionNode : public TemplateDefinitionNode {
public:
    using TemplateDefinitionNode::TemplateDefinitionNode;

    void accept(Visitor& visitor) override { visitor.visit(this); }

    // Vars are like properties, but for now we'll just store them as PropertyNodes.
    std::vector<std::unique_ptr<PropertyNode>> variables;
};


// --- Template Usages ---

class TemplateUsageNode : public Node {
public:
    TemplateUsageNode(const std::string& name) : name(name) {}
    std::string name;
};

class StyleUsageNode : public TemplateUsageNode {
public:
    using TemplateUsageNode::TemplateUsageNode;
    void accept(Visitor& visitor) override { visitor.visit(this); }
};

class ElementUsageNode : public TemplateUsageNode {
public:
    using TemplateUsageNode::TemplateUsageNode;
    void accept(Visitor& visitor) override { visitor.visit(this); }
};
