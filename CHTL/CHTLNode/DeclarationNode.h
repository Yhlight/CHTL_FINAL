#ifndef CHTL_DECLARATION_NODE_H
#define CHTL_DECLARATION_NODE_H

#include "BaseNode.h"
#include "../CHTLLexer/Token.h"
#include <string>
#include <vector>

// Represents a CSS declaration, e.g., `color: red;`
class DeclarationNode : public BaseNode {
public:
    std::string property;
    std::vector<Token> valueTokens;

    explicit DeclarationNode(const std::string& prop) : property(prop) {}
};

#endif // CHTL_DECLARATION_NODE_H
