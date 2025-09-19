#ifndef CHTL_JS_PARSER_H
#define CHTL_JS_PARSER_H

#include "../CHTLJSLexer/Token.h"
#include "../CHTLJSNode/CHTLJSBaseNode.h"
#include "../CHTLJSNode/ListenNode.h"
#include "../CHTLJSNode/EnhancedSelectorNode.h"
#include "../CHTLJSNode/EventHandlerNode.h"
#include <vector>
#include <memory>

namespace CHTL_JS {

class CHTLJSParser {
public:
    explicit CHTLJSParser(const std::vector<Token>& tokens, const std::string& source);
    std::vector<std::unique_ptr<CHTLJSBaseNode>> parse();

private:
    const std::vector<Token>& tokens;
    const std::string& source;
    size_t current = 0;

    bool isAtEnd();
    Token peek();
    Token previous();
    Token advance();
    bool check(TokenType type);
    Token consume(TokenType type, const std::string& message);
    bool match(const std::vector<TokenType>& types);

    std::unique_ptr<CHTLJSBaseNode> parseStatement();
    std::unique_ptr<EnhancedSelectorNode> parseEnhancedSelector();
    std::unique_ptr<ListenNode> parseListenExpression(std::unique_ptr<EnhancedSelectorNode> selector);
    std::unique_ptr<EventHandlerNode> parseEventHandlerExpression(std::unique_ptr<EnhancedSelectorNode> selector);
    std::unique_ptr<ListenNode> parseListenBlock(std::unique_ptr<EnhancedSelectorNode> selector);
};

} // namespace CHTL_JS

#endif // CHTL_JS_PARSER_H
