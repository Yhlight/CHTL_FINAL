#pragma once

#include "BaseNode.h"
#include <string>
#include <vector>
#include <memory>

namespace CHTL {

class TemplateNode : public Statement {
public:
    Token token;
    Token type;
    Token name;
    std::vector<std::unique_ptr<Statement>> body;
    std::string tokenLiteral() const override { return token.literal; }
    std::string toString() const override;
};

class TemplateInstantiationNode : public Statement {
public:
    Token token;
    Token name;
    std::string tokenLiteral() const override { return token.literal; }
    std::string toString() const override;
};

}
