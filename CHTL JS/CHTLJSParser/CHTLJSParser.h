#pragma once

#include "../CHTLJSLexer/CHTLJSLexer.h"
#include "../CHTLJSNode/Node.h"
#include <vector>

namespace CHTL_JS {

class CHTLJSParser {
public:
    CHTLJSParser(CHTLJSLexer& lexer);

    NodeList Parse();

private:
    void nextToken();

    CHTLJSLexer& m_lexer;
    Token m_currentToken;
};

} // namespace CHTL_JS
