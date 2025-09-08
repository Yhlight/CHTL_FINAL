#ifndef CHTL_PARSER_H
#define CHTL_PARSER_H

#include "CHTL/CHTLLexer/CHTLLexer.h"
#include "CHTL/CHTLNode/BaseNode.h"
#include "CHTL/CHTLNode/ElementNode.h"
#include "CHTL/CHTLContext/CHTLContext.h"
#include <memory>
#include <vector>

namespace CHTL {

// Forward declaration
class TemplateUsageNode;

class CHTLParser {
public:
    CHTLParser(CHTLLexer& lexer, CHTLContext& context);
    std::unique_ptr<BaseNode> parse();

private:
    CHTLLexer& m_lexer;
    CHTLContext& m_context;
    Token m_currentToken;

    void advance();
    void expect(TokenType type);

    std::unique_ptr<BaseNode> parseStatement();
    std::unique_ptr<BaseNode> parseElement();
    void parseElementContent(ElementNode* element);
    std::unique_ptr<BaseNode> parseTextElement();
    void parseAttributes(ElementNode* element);
    std::unique_ptr<BaseNode> parseStyleNode();
    void parseDefinitionBlock();
    void parseSpecializationBlock(TemplateUsageNode* usageNode);
    std::unique_ptr<BaseNode> parseInsertStatement();
    std::unique_ptr<BaseNode> parseDeleteStatement();
};

} // namespace CHTL

#endif // CHTL_PARSER_H
