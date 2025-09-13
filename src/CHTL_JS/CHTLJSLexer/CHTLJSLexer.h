#ifndef CHTL_JS_LEXER_H
#define CHTL_JS_LEXER_H

#include "CHTLJSToken.h"
#include <string>
#include <vector>

namespace CHTL {
namespace CHTL_JS {

class CHTLJSLexer {
public:
    explicit CHTLJSLexer(const std::string& source);
    std::vector<JSToken> scanTokens();

private:
    const std::string& source_;
    size_t pos_ = 0;
};

} // namespace CHTL_JS
} // namespace CHTL

#endif // CHTL_JS_LEXER_H
