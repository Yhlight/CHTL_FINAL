#ifndef PARSER_H
#define PARSER_H

#include "../CHTLLexer/Token.h"
#include "../CHTLNode/AST.h"
#include <vector>
#include <memory>
#include <stdexcept>

class Parser {
public:
    explicit Parser(const std::vector<Token>& tokens);

    // The main entry point, returns the root of the AST.
    std::unique_ptr<ProgramNode> parse();

private:
    const std::vector<Token>& tokens;
    size_t current = 0;

    // --- Grammar Rule Methods ---
    std::unique_ptr<BaseNode> declaration();
    std::unique_ptr<ElementNode> elementDeclaration();
    std::unique_ptr<AttributeNode> attributeDeclaration();
    std::unique_ptr<TextNode> textDeclaration();
    std::unique_ptr<CommentNode> commentDeclaration();
    std::unique_ptr<ValueNode> value();

    // Style-related parsing methods
    std::unique_ptr<StyleBlockNode> styleBlockDeclaration();
    std::unique_ptr<StyleContentNode> styleContentDeclaration();
    std::unique_ptr<StylePropertyNode> stylePropertyDeclaration();
    std::unique_ptr<StyleRuleNode> styleRuleDeclaration();

    // Template-related parsing methods
    std::unique_ptr<BaseNode> templateDeclaration();
    std::unique_ptr<TemplateStyleNode> templateStyleDeclaration();
    std::unique_ptr<TemplateElementNode> templateElementDeclaration();
    std::unique_ptr<UseStyleNode> useStyleDeclaration();
    std::unique_ptr<UseElementNode> useElementDeclaration();

    // --- Token Manipulation Helpers ---
    Token advance();
    Token peek() const;
    Token previous() const;
    bool isAtEnd() const;

    // Check if the current token is of a given type without consuming it.
    bool check(TokenType type) const;

    // Check if the current token is one of the given types, and if so, consume it.
    bool match(const std::vector<TokenType>& types);

    // Consume the current token if it's of the expected type, otherwise throw an error.
    Token consume(TokenType type, const std::string& message);

    // --- Error Handling ---
    class ParseError : public std::runtime_error {
    public:
        explicit ParseError(const std::string& message) : std::runtime_error(message) {}
    };

    ParseError error(const Token& token, const std::string& message);
    void synchronize();
};

#endif // PARSER_H
