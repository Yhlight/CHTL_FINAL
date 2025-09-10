#pragma once

#include "../CHTLJSLexer/CHTLJSLexer.h"
#include "../CHTLJSNode/CHTLJSNode.h"
#include <vector>
#include <memory>

class CHTLJSParser {
public:
    CHTLJSParser(CHTLJSLexer& lexer);
    std::vector<CHTLJSNodePtr> parse();

private:
    CHTLJSLexer& lexer;
    CHTLJSToken currentToken;
    CHTLJSToken previousToken;

    void advance();
    bool match(CHTLJSTokenType type);
    void consume(CHTLJSTokenType type, const std::string& message);

    CHTLJSNodePtr parseStatement();
    CHTLJSNodePtr parseListenStatement();
    CHTLJSNodePtr parseSelector();
};
