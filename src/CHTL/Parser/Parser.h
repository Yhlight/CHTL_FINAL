#ifndef CHTL_PARSER_H
#define CHTL_PARSER_H

#include "../Lexer/Token.h"
#include "../Node/BaseNode.h"
#include "../Node/RootNode.h"
#include "../Node/ElementNode.h"
#include "../Node/AttributeNode.h"
#include "../Node/TextNode.h"
#include "../Node/StyleNode.h"
#include "../Node/TemplateNode.h"
#include "../Node/TemplateUsageNode.h"
#include "../Node/CssPropertyNode.h"
#include "../Node/VarDeclarationNode.h"
#include "../Node/VarUsageNode.h"
#include "../Node/LiteralNode.h"
#include "../Node/CustomNode.h"
#include "../Node/CustomUsageNode.h"
#include "../Node/DeleteNode.h"
#include "../Node/InsertNode.h"
#include "../Context/TemplateRegistry.h"

#include <vector>
#include <memory>

namespace CHTL {

class Parser {
public:
    explicit Parser(const std::vector<Token>& tokens);
    std::shared_ptr<RootNode> parse();

private:
    std::shared_ptr<BaseNode> parseNode();
    std::shared_ptr<ElementNode> parseElement();
    std::shared_ptr<AttributeNode> parseAttribute();
    std::shared_ptr<TextNode> parseTextBlock();
    std::shared_ptr<StyleNode> parseStyleBlock();
    std::shared_ptr<BaseNode> parseDefinitionBlock();
    std::shared_ptr<BaseNode> parseUsage();
    std::shared_ptr<BaseNode> parseValue();
    std::shared_ptr<VarUsageNode> parseVarUsage();
    std::shared_ptr<DeleteNode> parseDelete();
    std::shared_ptr<InsertNode> parseInsert();

    const Token& currentToken() const;
    const Token& peekToken() const;
    void advance();
    bool match(TokenType type);
    const Token& consume(TokenType type, const std::string& errorMessage);

    std::vector<Token> m_tokens;
    size_t m_current = 0;
};

} // namespace CHTL

#endif // CHTL_PARSER_H
