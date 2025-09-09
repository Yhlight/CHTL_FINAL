#ifndef CHTL_PARSER_H
#define CHTL_PARSER_H

#include "CHTLLexer/Token.h"
#include "CHTLNode/BaseNode.h"
#include "CHTLNode/ElementNode.h"
#include "CHTLNode/TextNode.h"
#include "CHTLNode/OriginNode.h"
#include "CHTLContext/CHTLContext.h"
#include <vector>
#include <memory>
#include <string>

class CHTLParser {
public:
    CHTLParser(const std::vector<Token>& tokens, const std::string& source, CHTLContext* context);
    void parse();
    ElementNode* getRootNode() { return root; }

private:
    const std::vector<Token>& tokens;
    const std::string& source;
    CHTLContext* context;
    size_t current = 0;
    ElementNode* root = nullptr;

    // Token stream management
    const Token& peek() const;
    const Token& previous() const;
    bool isAtEnd() const;
    const Token& advance();
    bool check(TokenType type) const;
    bool match(const std::vector<TokenType>& types);
    const Token& consume(TokenType type, const std::string& message);
    bool checkNext(TokenType type) const;

    // Parsing rules
    void parseTopLevelStatement();
    std::unique_ptr<ElementNode> parseElement();
    std::unique_ptr<TextNode> parseText();
    std::unique_ptr<OriginNode> parseOrigin();
    void parseTemplateDefinition();
    void parseBlock(ElementNode* element);
    void parseAttribute(ElementNode* element);
    void parseStyleBlock(ElementNode* element);
};

#endif // CHTL_PARSER_H
