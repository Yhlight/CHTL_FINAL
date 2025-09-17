#pragma once

#include "Node.h"
#include "../CHTLLexer/Token.h"
#include <string>

namespace CHTL {

class VarSubstitutionNode : public Expression {
public:
    VarSubstitutionNode(Token group, Token varName)
        : m_groupName(std::move(group)),
          m_variableName(std::move(varName)) {}

    std::string toString(int indent = 0) const override {
        return std::string(indent, ' ') + m_groupName.literal + "(" + m_variableName.literal + ")";
    }

    Token m_groupName;
    Token m_variableName;
};

} // namespace CHTL
