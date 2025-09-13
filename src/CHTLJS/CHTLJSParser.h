#ifndef CHTL_JS_PARSER_H
#define CHTL_JS_PARSER_H

#include "CHTLJSLexer.h"
#include "CHTLJSNode.h"
#include <vector>
#include <memory>

namespace CHTL {
namespace CHTLJS {

class CHTLJSParser {
public:
    explicit CHTLJSParser(std::vector<JSToken> tokens);
    std::vector<std::unique_ptr<JSNode>> parse();

private:
    const JSToken& peek() const;
    const JSToken& advance();
    bool isAtEnd() const;

    std::vector<JSToken> tokens_;
    size_t current_ = 0;
};

} // namespace CHTLJS
} // namespace CHTL

#endif // CHTL_JS_PARSER_H
