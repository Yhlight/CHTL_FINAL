#ifndef CHTL_JS_PARSER_H
#define CHTL_JS_PARSER_H

#include "../CHTLJSLexer/CHTLJSToken.h"
#include "../CHTLJSNode/CHTLJSBaseNode.h"
#include <vector>
#include <memory>

namespace CHTL {
namespace CHTL_JS {

class CHTLJSParser {
public:
    explicit CHTLJSParser(std::vector<JSToken>& tokens);
    std::vector<std::unique_ptr<CHTLJSNode>> parse();

private:
    std::vector<JSToken>& tokens_;
    size_t current_ = 0;
};

} // namespace CHTL_JS
} // namespace CHTL

#endif // CHTL_JS_PARSER_H
