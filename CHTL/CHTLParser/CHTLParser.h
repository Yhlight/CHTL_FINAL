#pragma once

#include "../CHTLLexer/CHTLLexer.h"
#include "../CHTLNode/BaseNode.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/TextNode.h"
#include "../CHTLNode/CommentNode.h"

#include <vector>
#include <string>
#include <memory>

namespace CHTL {

class CHTLParser {
public:
    CHTLParser(CHTLLexer& lexer);

    std::unique_ptr<Program> ParseProgram();

    const std::vector<std::string>& Errors() const { return m_errors; }

private:
    void nextToken();
    void peekError(TokenType expected);
    bool expectPeek(TokenType expected);

    std::shared_ptr<Statement> parseStatement();
    std::shared_ptr<ElementNode> parseElementStatement();
    std::shared_ptr<TextNode> parseTextStatement();
    // ... other parsing methods will be added here

    CHTLLexer& m_lexer;
    Token m_curToken;
    Token m_peekToken;

    std::vector<std::string> m_errors;
};

} // namespace CHTL
