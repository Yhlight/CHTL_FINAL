#pragma once
#include "../CHTLLexer/Lexer.h"
#include "../CHTLNode/Node.h"
#include <memory>
#include <vector>
namespace CHTL {
class Parser {
public:
    explicit Parser(Lexer& lexer);
    std::unique_ptr<RootNode> parse_root();
private:
    void next_token();
    std::unique_ptr<Node> parse_statement();
    std::unique_ptr<ElementNode> parse_element_statement();
    std::unique_ptr<AttributeNode> parse_attribute_statement();
    std::unique_ptr<TextNode> parse_text_statement();
    std::unique_ptr<StyleBlockNode> parse_style_statement();
    std::unique_ptr<CssRuleNode> parse_css_rule_statement();
    std::unique_ptr<ValueNode> parse_value();
    Lexer& m_lexer; Token m_token;
    RootNode* m_root_node = nullptr;
};
}
