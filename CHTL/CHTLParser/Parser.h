#pragma once

#include "../CHTLLexer/Lexer.h"
#include "../CHTLNode/BaseNode.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/TextNode.h"
#include "../CHTLNode/StyleNode.h"
#include "../CHTLNode/TemplateUsageNode.h"
#include <vector>
#include <memory>
#include <map>
#include "../CHTLNode/ElementTemplateNode.h"
#include "../CHTLNode/StyleTemplateNode.h"
#include "../CHTLNode/VarTemplateNode.h"

class Parser {
public:
    Parser(Lexer& lexer);
    std::unique_ptr<ElementNode> parse();

    std::map<std::string, std::unique_ptr<ElementTemplateNode>> elementTemplates;
    std::map<std::string, std::unique_ptr<StyleTemplateNode>> styleTemplates;
    std::map<std::string, std::unique_ptr<VarTemplateNode>> varTemplates;

private:
    Lexer& lexer;
    Token currentToken;
    Token nextToken;

    void advance();
    bool match(TokenType type);
    void consume(TokenType type, const std::string& message);
    bool check(TokenType type);
    bool checkNext(TokenType type);

    std::unique_ptr<BaseNode> declaration();
    std::unique_ptr<BaseNode> templateDeclaration();
    std::unique_ptr<ElementNode> element();
    std::unique_ptr<TextNode> textNode();
    std::unique_ptr<StyleNode> styleNode();
    void attributes(ElementNode& element);
    std::string parseValue();
};
