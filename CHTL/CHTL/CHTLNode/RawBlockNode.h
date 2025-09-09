#pragma once

#include "BaseNode.h"
#include <string>

namespace CHTL {

enum class RawBlockType {
    CSS,
    JS
};

class RawBlockNode : public Statement {
public:
    Token token;
    RawBlockType type;
    std::string content;
    std::string tokenLiteral() const override { return token.literal; }
    std::string toString() const override;
};

}
