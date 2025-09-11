#ifndef CHTL_PARSER_H
#define CHTL_PARSER_H

#include "../CHTLLexer/Token.h"
#include "../CHTLNode/BaseNode.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/OriginNode.h"
#include "../CHTLNode/OriginUsageNode.h"
#include "../CHTLContext.h"
#include <vector>
#include <memory>

class Parser {
public:
    explicit Parser(const std::vector<Token>& tokens, CHTLContext& context);
    std::vector<std::unique_ptr<BaseNode>> parse();

private:
    const std::vector<Token>& tokens;
    CHTLContext& context;
    int current = 0;

    // Helper methods
    bool isAtEnd();
    Token peek();
    Token peekNext();
    Token advance();
    bool match(TokenType type);
    bool check(TokenType type);

    // Grammar parsing methods
    std::unique_ptr<ElementNode> parseElement();
    std::unique_ptr<OriginNode> parseOriginBlock();
    std::unique_ptr<OriginUsageNode> parseOriginUsage();
    void parseImportStatement();
};

#endif // CHTL_PARSER_H
