#pragma once

#include "BaseNode.h"
#include <string>
#include <vector>
#include <memory>

namespace CHTL {

class StylePropertyNode;

class StyleRuleNode : public Statement {
public:
    Token selector;
    std::vector<std::unique_ptr<StylePropertyNode>> properties;
    std::string tokenLiteral() const override { return selector.literal; }
    std::string toString() const override;
};

class StylePropertyNode : public Statement {
public:
    Token key;
    std::unique_ptr<Expression> value;
    std::string tokenLiteral() const override { return key.literal; }
    std::string toString() const override;
};

class StyleNode : public Statement {
public:
    Token token;
    std::vector<std::unique_ptr<Statement>> children;
    std::string tokenLiteral() const override { return token.literal; }
    std::string toString() const override;
};

}
