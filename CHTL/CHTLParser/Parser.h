#pragma once

#include "../CHTLLexer/Lexer.h"
#include "../CHTLNode/BaseNode.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/TextNode.h"
#include <vector>
#include <memory>
#include <map>
#include "../CHTLNode/ElementTemplateNode.h"
#include "../CHTLNode/StyleTemplateNode.h"
#include "../CHTLNode/VarTemplateNode.h"
#include "../CHTLNode/CustomStyleNode.h"
#include "../CHTLNode/CustomElementNode.h"

class StyleNode; // Forward declaration

class Parser {
public:
    Parser(Lexer& lexer);
    std::unique_ptr<ElementNode> parse();

    // Make template stores public for the generator to access
    std::map<std::string, std::unique_ptr<ElementTemplateNode>> elementTemplates;
    std::map<std::string, std::unique_ptr<StyleTemplateNode>> styleTemplates;
    std::map<std::string, std::unique_ptr<VarTemplateNode>> varTemplates;
    std::map<std::string, std::unique_ptr<CustomStyleNode>> customStyleTemplates;
    std::map<std::string, std::unique_ptr<CustomElementNode>> customElementTemplates;

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
    std::unique_ptr<BaseNode> customDeclaration();
    std::unique_ptr<BaseNode> originDeclaration();
    std::unique_ptr<ElementNode> element();
    std::unique_ptr<TextNode> textNode();
    std::unique_ptr<StyleNode> styleNode();
    void attributes(ElementNode& element);
    std::string parseValue();
    void handleCustomElementUsage(const std::string& templateName, ElementNode* parentNode);
};
