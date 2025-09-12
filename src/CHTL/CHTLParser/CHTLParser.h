#ifndef CHTL_PARSER_H
#define CHTL_PARSER_H

#include "CHTL/CHTLLexer/Token.h"
#include "CHTL/CHTLNode/BaseNode.h"
#include "CHTL/CHTLNode/ElementNode.h"
#include <vector>
#include <memory>
#include "CHTL/CHTLContext/CHTLContext.h"

namespace CHTL {

class CHTLParser {
public:
    explicit CHTLParser(std::vector<Token>&& tokens, CHTLContext& context);
    std::unique_ptr<BaseNode> parse();

private:
    std::unique_ptr<ElementNode> parseElement();
    void parseBlock(ElementNode* element);
    void parseProperty(ElementNode* element);
    void parseStyleBlock(ElementNode* element);
    std::string parseCssBlock();

    // Expression parsing
    std::unique_ptr<ExprNode> parseExpression();
    std::unique_ptr<ExprNode> parseTerm();
    std::unique_ptr<ExprNode> parseFactor();
    std::unique_ptr<ExprNode> parsePrimary();

    bool isAtEnd() const;
    Token advance();
    Token peek() const;
    Token peekNext() const;
    Token consume(TokenType type, const std::string& message);
    bool check(TokenType type) const;

    CHTLContext& context;
    const std::vector<Token> tokens;
    int current = 0;
};

} // namespace CHTL

#endif // CHTL_PARSER_H
