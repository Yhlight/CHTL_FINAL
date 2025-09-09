#pragma once

#include <string>
#include <memory>
#include <vector>
#include "../CHTLLexer/Token.h"

namespace CHTL {

class Statement;
class Expression;

class Node {
public:
    virtual ~Node() = default;
    virtual std::string tokenLiteral() const = 0;
    virtual std::string toString() const = 0;
};

class Statement : public Node {
public:
    std::string tokenLiteral() const override { return ""; }
    std::string toString() const override { return ""; }
};

class Expression : public Node {
public:
    std::string tokenLiteral() const override { return ""; }
    std::string toString() const override { return ""; }
};

class Program : public Node {
public:
    std::vector<std::unique_ptr<Statement>> statements;
    std::string tokenLiteral() const override { return "Program"; }
    std::string toString() const override;
};

}
