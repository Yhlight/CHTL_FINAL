#ifndef CHTL_PARSER_H
#define CHTL_PARSER_H

#include "../CHTLLexer/Token.h"
#include "../CHTLNode/BaseNode.h"
#include "../CHTLNode/RootNode.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/TextNode.h"
#include "../CHTLNode/CommentNode.h"
#include "../CHTLNode/StyleBlockNode.h"
#include "../CHTLNode/StyleTemplateNode.h"
#include <vector>
#include <memory>
#include <unordered_map>

namespace CHTL {

class CHTLParser {
public:
    explicit CHTLParser(const std::vector<Token>& tokens);
    std::unique_ptr<RootNode> parse();

private:
    std::unique_ptr<Node> parseDeclaration();
    void parseTemplateDefinition();
    std::unique_ptr<ElementNode> parseElement();
    void parseElementBody(ElementNode& element);
    void parseAttributes(ElementNode& element);
    std::unique_ptr<TextNode> parseText();
    std::unique_ptr<StyleBlockNode> parseStyleBlock();
    std::unique_ptr<CommentNode> parseGeneratorComment();


    const Token& peek() const;
    const Token& previous() const;
    const Token& advance();
    bool isAtEnd() const;
    bool check(TokenType type) const;
    bool match(const std::vector<TokenType>& types);
    const Token& peekNext() const;
    const Token& consume(TokenType type, const std::string& message);
    void synchronize();

    const std::vector<Token>& tokens_;
    std::unordered_map<std::string, std::shared_ptr<StyleTemplateNode>> style_templates_;
    size_t current_ = 0;
    bool hadError_ = false;
};

} // namespace CHTL

#endif // CHTL_PARSER_H
