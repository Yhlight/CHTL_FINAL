#pragma once

#include "Node.h"
#include "StatementNodes.h"
#include <string>
#include <vector>
#include <memory>

// A base class for all custom definitions, e.g., [Custom] @Style MyCustom { ... }
class BaseCustomNode : public Statement {
public:
    BaseCustomNode(Token token, std::string name)
        : token(std::move(token)), name(std::move(name)) {}

    const std::string& getName() const { return name; }

protected:
    Token token;
    std::string name;
};

// Represents a [Custom] @Style { ... } block
class StyleCustomNode : public BaseCustomNode {
public:
    using BaseCustomNode::BaseCustomNode; // Inherit constructor

    std::string toString() const override { return "[StyleCustom: " + name + "]"; }

    // Can contain properties, nested rules, delete statements, etc.
    std::vector<std::shared_ptr<Statement>> statements;
};

// Represents a `delete` statement, e.g., `delete prop1, prop2;`
class DeletePropertyNode : public Statement {
public:
    explicit DeletePropertyNode(Token token) : token(std::move(token)) {}

    std::string toString() const override {
        std::string out = "delete ";
        for(size_t i = 0; i < propertiesToDelete.size(); ++i) {
            out += propertiesToDelete[i];
            if (i < propertiesToDelete.size() - 1) out += ", ";
        }
        return out + ";";
    }

    Token token;
    std::vector<std::string> propertiesToDelete;
};
