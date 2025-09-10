#ifndef CHTL_RULE_NODE_H
#define CHTL_RULE_NODE_H

#include "BaseNode.h"
#include "DeclarationNode.h"
#include "../CHTLLexer/Token.h"
#include <vector>
#include <memory>

// Represents a CSS rule, e.g., `.box { width: 100px; }`
class RuleNode : public BaseNode {
public:
    std::vector<Token> selectorTokens;
    std::vector<std::unique_ptr<DeclarationNode>> declarations;

    void addDeclaration(std::unique_ptr<DeclarationNode> decl) {
        declarations.push_back(std::move(decl));
    }
};

#endif // CHTL_RULE_NODE_H
