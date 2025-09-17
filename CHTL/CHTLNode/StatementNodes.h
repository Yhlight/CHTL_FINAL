#pragma once

#include "Node.h"
#include "ExpressionNodes.h"
#include "../CHTLLexer/Token.h"
#include <string>
#include <vector>
#include <memory>

// Represents a key-value pair, like an HTML attribute or a CSS property.
// It's a general-purpose node used inside other nodes.
class AttributeNode : public Node {
public:
    AttributeNode(Token token, std::string key, std::shared_ptr<Expression> value)
        : token(std::move(token)), key(std::move(key)), value(std::move(value)) {}

    std::string toString() const override;

    Token token;
    std::string key;
    std::shared_ptr<Expression> value;
};


// Represents an element node, like `div { ... }`
class ElementNode : public Statement {
public:
    ElementNode(Token token, std::string tagName)
        : token(std::move(token)), tagName(std::move(tagName)) {}

    std::string toString() const override;

    Token token;
    std::string tagName;
    std::vector<std::shared_ptr<AttributeNode>> attributes;
    std::vector<std::shared_ptr<Node>> children; // Can contain other elements, text, styles, etc.
};


// Represents a text block, like `text { "content" }`
class TextNode : public Statement {
public:
    TextNode(Token token, std::string content)
        : token(std::move(token)), content(std::move(content)) {}

    std::string toString() const override {
        return "text { \"" + content + "\" }";
    }

    Token token;
    std::string content;
};


// Represents a single rule within a style block, e.g., `.selector { prop: val; }`
class CssRuleNode : public Node {
public:
    explicit CssRuleNode(std::string selector) : selector(std::move(selector)) {}

    std::string toString() const override;

    std::string selector;
    std::vector<std::shared_ptr<AttributeNode>> properties;
};


// Represents a style block, like `style { ... }`
class StyleNode : public Statement {
public:
    explicit StyleNode(Token token) : token(std::move(token)) {}

    std::string toString() const override;

    Token token;
    // A style block can contain direct properties (inline) and nested rules.
    std::vector<std::shared_ptr<CssRuleNode>> rules;
};


// Represents a generator comment, like `# a comment`
class CommentNode : public Statement {
public:
    CommentNode(Token token, std::string content)
        : token(std::move(token)), content(std::move(content)) {}

    std::string toString() const override {
        return content;
    }

    Token token;
    std::string content;
};
