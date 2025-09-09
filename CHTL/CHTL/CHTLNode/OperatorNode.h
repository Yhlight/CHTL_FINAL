#pragma once

#include "BaseNode.h"
#include <string>
#include <vector>

namespace CHTL {

// Represents a `delete` statement, e.g., `delete prop1, prop2;`
class DeleteNode : public Statement {
public:
    Token token; // The 'delete' token
    std::vector<Token> targets; // The properties/templates to delete

    std::string tokenLiteral() const override { return token.literal; }
    std::string toString() const override { return "delete ...;"; }
};

} // namespace CHTL
