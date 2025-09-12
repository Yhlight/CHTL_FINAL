#ifndef CHTL_PARSER_H
#define CHTL_PARSER_H

#include "../CHTLLexer/Token.h"
#include "../CHTLNode/BaseNode.h"
#include <vector>
#include <memory>
#include <string>

// Forward declarations
class CHTLContext;

class Parser {
public:
    Parser(const std::string& source, const std::vector<Token>& tokens, CHTLContext& context);

    // The main entry point for parsing. Returns the root of the AST.
    std::shared_ptr<BaseNode> parse();
    bool hasError() const { return hadError; }

private:
    // Recursive descent parsing methods
    void templateDefinition();
    std::shared_ptr<BaseNode> declaration();
    std::shared_ptr<BaseNode> element();
    std::shared_ptr<BaseNode> textBlock();
    std::shared_ptr<BaseNode> styleBlock();
    void attributes(std::shared_ptr<class ElementNode> node);

    // Token stream helper methods
    bool isAtEnd();
    Token advance();
    Token peek();
    Token previous();
    bool check(TokenType type);
    bool match(const std::vector<TokenType>& types);
    Token consume(TokenType type, const std::string& message);
    void synchronize();

    const std::string& source;
    CHTLContext& context;
    const std::vector<Token>& tokens;
    size_t current = 0;
    bool hadError = false;
};

#endif // CHTL_PARSER_H
