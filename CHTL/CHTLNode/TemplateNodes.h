#pragma once

#include "Node.h"
#include "StatementNodes.h"
#include <string>
#include <vector>
#include <memory>

// A base class for all template definitions, e.g., [Template] @Style MyTemplate { ... }
class BaseTemplateNode : public Statement {
public:
    BaseTemplateNode(Token token, std::string name)
        : token(std::move(token)), name(std::move(name)) {}

    const std::string& getName() const { return name; }

protected:
    Token token;
    std::string name;
};

// Represents a [Template] @Style { ... } block
class StyleTemplateNode : public BaseTemplateNode {
public:
    using BaseTemplateNode::BaseTemplateNode; // Inherit constructor

    std::string toString() const override { return "[StyleTemplate: " + name + "]"; }

    std::vector<std::shared_ptr<AttributeNode>> properties;
};

// Represents a [Template] @Element { ... } block
class ElementTemplateNode : public BaseTemplateNode {
public:
    using BaseTemplateNode::BaseTemplateNode; // Inherit constructor

    std::string toString() const override { return "[ElementTemplate: " + name + "]"; }

    std::vector<std::shared_ptr<Node>> children;
};

// Represents a [Template] @Var { ... } block
class VarTemplateNode : public BaseTemplateNode {
public:
    using BaseTemplateNode::BaseTemplateNode; // Inherit constructor

    std::string toString() const override { return "[VarTemplate: " + name + "]"; }

    std::vector<std::shared_ptr<AttributeNode>> variables;
};


#include "CustomNodes.h"

// Represents the usage of a template or custom, e.g., @Style MyStyles; or @Style MyCustom { ... }
class TemplateUsageNode : public Statement {
public:
    TemplateUsageNode(Token token, std::string name)
        : token(std::move(token)), name(std::move(name)) {}

    std::string toString() const override { return "[TemplateUsage: @" + token.literal + " " + name + "]"; }

    Token token; // The token for the type, e.g., @Style
    std::string name;

    // For specialization, e.g. @Style MyCustom { color: red; delete border; }
    std::vector<std::shared_ptr<Statement>> specializations;
};
