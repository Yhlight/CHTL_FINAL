#pragma once

#include "Node.h"
#include "../CHTLLexer/Token.h"
#include <string>

// Represents an identifier when used as a value (e.g., `red` in `color: red;`).
class IdentifierNode : public Expression {
public:
    IdentifierNode(const Token& token, std::string value)
        : token(token), value(std::move(value)) {}

    std::string toString() const override { return value; }

    Token token; // The token::{ type: IDENTIFIER, literal: "red" }
    std::string value;
};

// Represents a string literal (e.g., `"some text"`).
class StringLiteralNode : public Expression {
public:
    StringLiteralNode(const Token& token, std::string value)
        : token(token), value(std::move(value)) {}

    std::string toString() const override { return "\"" + value + "\""; }

    Token token; // The token::{ type: STRING_LITERAL, literal: "some text" }
    std::string value;
};

// Represents the usage of a variable from a Var template, e.g., ThemeColor(tableColor)
class VarUsageNode : public Expression {
public:
    VarUsageNode(const Token& token, std::string groupName, std::string variableName)
        : token(token), groupName(std::move(groupName)), variableName(std::move(variableName)) {}

    std::string toString() const override { return groupName + "(" + variableName + ")"; }

    Token token;
    std::string groupName;
    std::string variableName;
};
