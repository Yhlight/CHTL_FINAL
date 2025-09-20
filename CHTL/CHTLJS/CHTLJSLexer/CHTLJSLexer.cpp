#include "CHTLJSLexer.h"

namespace CHTL_JS {

CHTMLJSLexer::CHTMLJSLexer(const std::string& source) : source(source), current(0), line(1) {}

std::vector<Token> CHTMLJSLexer::scanTokens() {
    // Stub implementation. Returns an empty vector for now.
    std::vector<Token> tokens;
    // In the future, this will scan the CHTL JS source and produce tokens.
    return tokens;
}

} // namespace CHTL_JS
