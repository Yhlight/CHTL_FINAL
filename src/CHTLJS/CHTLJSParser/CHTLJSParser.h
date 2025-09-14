#ifndef CHTL_JS_PARSER_H
#define CHTL_JS_PARSER_H

#include "../CHTLJSLexer/CHTLJSToken.h"
#include "../CHTLJSNode/CHTLJSNode.h"
#include "../CHTLJSNode/RootNode.h"
#include "../CHTLJSNode/EnhancedSelectorNode.h"
#include "../CHTLJSNode/ListenNode.h"
#include "../CHTLJSNode/AnimateNode.h"
#include "../CHTLJSNode/DelegateNode.h"
#include "../CHTLJSNode/RouterNode.h"
#include "../CHTLJSNode/VirNode.h"
#include "../CHTLJSNode/ValueNode.h"
#include "CHTLJSContext.h"
#include <vector>
#include <memory>
#include <stdexcept>

namespace CHTLJS {

class CHTLJSParser {
public:
    CHTLJSParser(std::vector<CHTLJSToken>& tokens, std::shared_ptr<CHTLJSContext> context);
    std::unique_ptr<RootNode> parse();

private:
    std::unique_ptr<CHTLJSNode> parseStatement();
    std::unique_ptr<CHTLJSNode> parseExpression();
    std::unique_ptr<ListenNode> parseListenExpression(std::unique_ptr<CHTLJSNode> object);
    std::unique_ptr<EnhancedSelectorNode> parseEnhancedSelector();
    std::unique_ptr<CHTLJSNode> parsePrimary();
    std::unique_ptr<ValueNode> parseValue();
    std::unique_ptr<VirNode> parseVirDeclaration();
    std::unique_ptr<RouterNode> parseRouterStatement();
    std::unique_ptr<AnimateNode> parseAnimateExpression();
    std::unique_ptr<DelegateNode> parseDelegateExpression(std::unique_ptr<CHTLJSNode> object);

    // Helper methods
    const CHTLJSToken& previous() const;
    const CHTLJSToken& advance();
    bool match(const std::vector<CHTLJSTokenType>& types);
    const CHTLJSToken& consume(CHTLJSTokenType type, const std::string& message);
    bool check(CHTLJSTokenType type) const;
    const CHTLJSToken& peek() const;
    const CHTLJSToken& peekNext() const;
    bool isAtEnd() const;
    void synchronize();
    std::string parseRawBlock();

    std::vector<CHTLJSToken>& tokens_;
    std::shared_ptr<CHTLJSContext> context_;
    size_t current_ = 0;
    bool hadError_ = false;
};

} // namespace CHTLJS

#endif // CHTL_JS_PARSER_H
