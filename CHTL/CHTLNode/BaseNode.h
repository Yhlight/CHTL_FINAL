#pragma once

#include "../CHTLLexer/Token.h"
#include <string>
#include <vector>
#include <memory>

namespace CHTL {

// Forward declarations for node types
class Statement;
class Expression;
class ElementNode;
// ... add other node types as they are created

// Base class for all nodes in the AST
class BaseNode {
public:
    virtual ~BaseNode() = default;

    // For debugging: returns a string representation of the node
    virtual std::string ToString() const = 0;

    // Returns the token literal associated with this node.
    // Useful for getting tag names, identifiers, etc.
    virtual std::string GetTokenLiteral() const = 0;
};

// Base class for all statement nodes
class Statement : public BaseNode {
public:
    // A dummy method to satisfy the abstract base class requirements for now.
    // In a real compiler, statements might have specific interfaces.
    std::string GetTokenLiteral() const override { return ""; }
};

// Base class for all expression nodes
class Expression : public BaseNode {
public:
    // A dummy method to satisfy the abstract base class requirements for now.
    // In a real compiler, expressions might have specific interfaces.
    std::string GetTokenLiteral() const override { return ""; }
};

// Represents a program, which is just a collection of statements
class Program : public BaseNode {
public:
    std::vector<std::shared_ptr<Statement>> statements;

    std::string GetTokenLiteral() const override {
        if (!statements.empty()) {
            return statements[0]->GetTokenLiteral();
        }
        return "";
    }

    std::string ToString() const override {
        std::string out;
        for (const auto& s : statements) {
            out += s->ToString();
        }
        return out;
    }
};

} // namespace CHTL
