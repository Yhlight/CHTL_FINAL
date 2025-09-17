#pragma once

#include "../CHTLJSLexer/Token.h"
#include "../CHTLJSNode/CjsBaseNode.h"
#include <vector>
#include <memory>

class ChtlJsParser {
public:
    ChtlJsParser(const std::vector<ChtlJsToken>& tokens);
    std::vector<std::shared_ptr<CjsBaseNode>> parse();

private:
    std::vector<ChtlJsToken> tokens;
    size_t current = 0;

    ChtlJsToken peek() const;
    ChtlJsToken advance();
    bool isAtEnd() const;
};
