#ifndef CHTL_JS_PARSER_H
#define CHTL_JS_PARSER_H

#include "../CHTLJSLexer/CHTLJSToken.h"
#include "../CHTLJSNode/CHTLJSNode.h"
#include <vector>
#include <memory>
#include "CHTLJSContext.h"

namespace CHTLJS {

class CHTLJSParser {
public:
    explicit CHTLJSParser(std::vector<CHTLJSToken>& tokens, std::shared_ptr<CHTLJSContext> context);
    std::unique_ptr<CHTLJSNode> parse();

private:
    const CHTLJSToken& peek() const;
    const CHTLJSToken& advance();
    bool isAtEnd() const;
    bool match(const std::vector<CHTLJSTokenType>& types);
    std::unique_ptr<CHTLJSNode> parseListenBlock(std::unique_ptr<CHTLJSNode> object);
    std::unique_ptr<CHTLJSNode> parseDelegateBlock(std::unique_ptr<CHTLJSNode> object);
    std::unique_ptr<CHTLJSNode> parseAnimateBlock();
    std::unique_ptr<CHTLJSNode> parseRouterBlock();

    std::vector<CHTLJSToken>& tokens_;
    std::shared_ptr<CHTLJSContext> context_;
    size_t current_ = 0;
};

} // namespace CHTLJS

#endif // CHTL_JS_PARSER_H
