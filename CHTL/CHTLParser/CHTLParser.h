#ifndef CHTL_PARSER_H
#define CHTL_PARSER_H

#include "../CHTLLexer/CHTLLexer.h"
#include "../CHTLNode/BaseNode.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLContext/CHTLContext.h" // Added
#include <memory>
#include <vector>

namespace CHTL {

class CHTLParser {
public:
    // Updated constructor to accept a context
    CHTLParser(CHTLLexer& lexer, CHTLContext& context);

    std::unique_ptr<BaseNode> parse();

private:
    CHTLLexer& m_lexer;
    CHTLContext& m_context; // Added
    Token m_currentToken;

    void advance();
    void expect(TokenType type);

    // Main parsing methods
    std::unique_ptr<BaseNode> parseStatement();
    std::unique_ptr<BaseNode> parseElement();
    void parseElementContent(ElementNode* element);

    // Specific block/node parsers
    std::unique_ptr<BaseNode> parseTextElement();
    void parseAttributes(ElementNode* element);
    std::unique_ptr<BaseNode> parseStyleNode();
    void parseTemplateDefinition(); // Added: Note: returns void as it populates the context directly

};

} // namespace CHTL

#endif // CHTL_PARSER_H
