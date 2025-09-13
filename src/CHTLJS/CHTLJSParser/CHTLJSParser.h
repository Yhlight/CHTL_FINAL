#ifndef CHTL_JS_PARSER_H
#define CHTL_JS_PARSER_H

#include "../CHTLJSLexer/CHTLJSToken.h"
#include "../CHTLJSNode/CHTLJSNode.h"
#include <vector>
#include <memory>

namespace CHTLJS {

class CHTLJSParser {
public:
    explicit CHTLJSParser(std::vector<CHTLJSToken>& tokens);
    std::unique_ptr<CHTLJSNode> parse();

private:
    const CHTLJSToken& peek() const;
    const CHTLJSToken& advance();
    bool isAtEnd() const;
    std::unique_ptr<CHTLJSNode> parseListenBlock(std::unique_ptr<CHTLJSNode> object);

    std::vector<CHTLJSToken>& tokens_;
    size_t current_ = 0;
};

} // namespace CHTLJS

#endif // CHTL_JS_PARSER_H
