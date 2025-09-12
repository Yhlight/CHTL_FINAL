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
#include "../CHTLNode/ElementTemplateNode.h"
#include "../CHTLNode/VarTemplateNode.h"
#include "../CHTLLoader/CHTLLoader.h"
#include <vector>
#include <memory>
#include <unordered_map>

namespace CHTL {

class CHTLParser {
public:
    CHTLParser(std::vector<Token>& tokens, CHTLLoader& loader, const std::string& initial_path);
    std::unique_ptr<RootNode> parse();

private:
    std::vector<std::unique_ptr<Node>> parseDeclaration();
    void parseTemplateDefinition(bool is_custom);
    std::unique_ptr<ElementNode> parseElement();
    void parseElementBody(ElementNode& element);
    void parseAttributes(ElementNode& element);
    std::vector<Token> parsePropertyValue();
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

    std::vector<Token>& tokens_; // Now non-const so we can splice
    CHTLLoader& loader_;
    std::string current_path_;
    std::unordered_map<std::string, std::shared_ptr<StyleTemplateNode>> style_templates_;
    std::unordered_map<std::string, std::shared_ptr<ElementTemplateNode>> element_templates_;
    std::unordered_map<std::string, std::shared_ptr<VarTemplateNode>> var_templates_;
    size_t current_ = 0;
    bool hadError_ = false;
};

} // namespace CHTL

#endif // CHTL_PARSER_H
