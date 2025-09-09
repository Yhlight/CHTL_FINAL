#pragma once

#include "BaseNode.h"
#include <string>
#include <vector>
#include <memory>

namespace CHTL {

class NamespaceNode : public Statement {
public:
    Token token;
    Token name;
    std::vector<std::unique_ptr<Statement>> body;
    std::string tokenLiteral() const override { return token.literal; }
    std::string toString() const override;
};

class ImportNode : public Statement {
public:
    Token token;
    Token type;
    Token target;
    std::string path;
    Token alias;
    std::string tokenLiteral() const override { return token.literal; }
    std::string toString() const override;
};

}
