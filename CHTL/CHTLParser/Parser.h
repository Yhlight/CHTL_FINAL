#ifndef CHTL_PARSER_H
#define CHTL_PARSER_H

#include "../CHTLLexer/Token.h"
#include "../CHTLNode/BaseNode.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/StyleNode.h"
#include "../CHTLNode/ScriptNode.h"
#include "../CHTLNode/TemplateNode.h"
#include "../CHTLNode/TemplateUsageNode.h"
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

    // --- Parser Helper Methods ---
    bool isAtEnd();
    Token peek();
    Token peekNext();
    Token previous();
    Token advance();
    bool check(TokenType type);
    bool match(const std::vector<TokenType>& types);

    // --- Grammar Parsing Methods ---
    std::unique_ptr<ElementNode> parseElement();
    void parseChildren(ElementNode* parent);
    void parseAttribute(ElementNode* element);
    std::string parseValue();
    std::unique_ptr<TextNode> parseTextNode();
    std::unique_ptr<CommentNode> parseCommentNode();
    std::unique_ptr<StyleNode> parseStyleNode(ElementNode* parent);
    std::unique_ptr<TemplateNode> parseTemplateDefinition();
    std::unique_ptr<TemplateUsageNode> parseTemplateUsage();
};

#endif // CHTL_PARSER_H
