#ifndef CHTLJS_PARSER_H
#define CHTLJS_PARSER_H

#include "../CHTLJSLexer/CHTLJSToken.h"
#include "../CHTLJSNode/CHTLJS_SelectorNode.h"
#include "../CHTLJSNode/CHTLJS_ListenNode.h"
#include "../CHTLJSNode/CHTLJS_RawJSNode.h"
#include <vector>
#include <memory>

namespace CHTLJS {

class CHTLJSParser {
public:
    explicit CHTLJSParser(const std::vector<Token>& tokens);
    std::vector<std::unique_ptr<CHTLJS_BaseNode>> parse();

private:
    const std::vector<Token>& tokens;
    int current = 0;

    // --- Helper Methods ---
    bool isAtEnd();
    Token peek();
    Token previous();
    Token advance();
    bool check(TokenType type);
    Token consume(TokenType type, const std::string& message);
    bool match(const std::vector<TokenType>& types);
    void error(const Token& token, const std::string& message);

    // --- Parsing Methods ---
    std::unique_ptr<CHTLJS_BaseNode> parseExpression();
    std::unique_ptr<CHTLJS_BaseNode> parsePrimary();
    std::unique_ptr<CHTLJS_ListenNode> parseListenExpression(std::unique_ptr<CHTLJS_BaseNode> target);

};

} // namespace CHTLJS

#endif // CHTLJS_PARSER_H
