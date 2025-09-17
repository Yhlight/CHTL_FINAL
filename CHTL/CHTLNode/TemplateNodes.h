#pragma once

#include "BaseNode.h"
#include "AttributeNode.h"
#include <vector>
#include <memory>
#include <string>

namespace CHTL {

// Represents a template definition, e.g., [Template] @Style MyStyles { ... }
class TemplateDefinitionNode : public Statement {
public:
    Token templateToken; // The [Template] token
    Token typeToken;     // The @Style, @Element, etc. token
    Token nameToken;     // The name of the template

    // For now, the body is specific to @Style templates
    std::vector<std::shared_ptr<AttributeNode>> properties;

    TemplateDefinitionNode(Token temp, Token type, Token name)
        : templateToken(std::move(temp)), typeToken(std::move(type)), nameToken(std::move(name)) {}

    std::string GetTokenLiteral() const override { return templateToken.literal; }
    std::string ToString() const override {
        return "[Template] @" + typeToken.literal + " " + nameToken.literal + " { ... }";
    }
};


// Represents the usage of a template, e.g., @Style MyStyles;
class TemplateUsageNode : public Statement {
public:
    Token atToken;     // The @ token
    Token typeToken; // The Style, Element, etc. token
    Token nameToken;   // The name of the template being used

    TemplateUsageNode(Token at, Token type, Token name)
        : atToken(std::move(at)), typeToken(std::move(type)), nameToken(std::move(name)) {}

    std::string GetTokenLiteral() const override { return atToken.literal; }
    std::string ToString() const override {
        return "@" + typeToken.literal + " " + nameToken.literal + ";";
    }
};

} // namespace CHTL
