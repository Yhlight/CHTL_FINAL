#pragma once

#include "../CHTLJSLexer/CHTLJSLexer.h"
#include "../CHTLJSNode/CHTLJSBaseNode.h"
#include "../CHTLJSNode/CHTLJSNodes.h"
#include <vector>
#include <string>
#include <memory>

namespace CHTL {
namespace CHTL_JS {

class CHTLJSParser {
public:
    CHTLJSParser(CHTLJSLexer& lexer);

    std::unique_ptr<CHTLJSProgram> parseProgram();
    const std::vector<std::string>& getErrors() const;

private:
    void nextToken();
    bool expectPeek(CHTLJSTokenType t);

    std::unique_ptr<CHTLJSStatement> parseStatement();
    std::unique_ptr<EnhancedSelectorNode> parseEnhancedSelector();
    std::unique_ptr<ListenNode> parseListenStatement();
    // In the future: parseAnimate, parseRouter, etc.

    CHTLJSLexer& m_lexer;
    std::vector<std::string> m_errors;

    CHTLJSToken m_currentToken;
    CHTLJSToken m_peekToken;
};

} // namespace CHTL_JS
} // namespace CHTL
