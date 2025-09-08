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
#include "../CHTLNode/CustomElementNode.h"
#include "../CHTLNode/CustomStyleTemplateNode.h"

class Parser {
public:
    Parser(Lexer& lexer);
    std::unique_ptr<ElementNode> parse();

    std::map<std::string, std::unique_ptr<ElementTemplateNode>> elementTemplates;
    std::map<std::string, std::unique_ptr<StyleTemplateNode>> styleTemplates;
    std::map<std::string, std::unique_ptr<VarTemplateNode>> varTemplates;
    std::map<std::string, std::unique_ptr<CustomElementNode>> customElementTemplates;
    std::map<std::string, std::unique_ptr<CustomStyleTemplateNode>> customStyleTemplates;

private:
    Lexer& lexer;
    Token currentToken;
    Token nextToken;
    Token previousToken; // Needed for inherit keyword check

    void advance();
    bool match(TokenType type);
    void consume(TokenType type, const std::string& message);
    bool check(TokenType type);
    bool checkNext(TokenType type);

    std::unique_ptr<BaseNode> declaration();
    std::unique_ptr<BaseNode> templateDeclaration();
    std::unique_ptr<BaseNode> customDeclaration();
    std::unique_ptr<ElementNode> element();
    std::unique_ptr<TextNode> textNode();
    std::unique_ptr<StyleNode> styleNode();
    void attributes(ElementNode& element);
    std::string parseValue();
    void handleCustomElementUsage(const std::string& templateName, ElementNode* parentNode);

    struct Selector {
        std::string tagName;
        int index = -1;
    };
    Selector parseSelector();
    BaseNode* findNodeByTag(BaseNode* root, const std::string& tagName);
    void mergeStyles(ElementNode* targetNode, ElementNode* specNode);
};
