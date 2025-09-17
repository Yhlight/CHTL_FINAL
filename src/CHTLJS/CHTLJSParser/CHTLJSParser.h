#ifndef CHTL_JS_PARSER_H
#define CHTL_JS_PARSER_H

#include "../CHTLJSLexer/CHTLJSToken.h"
#include "../CHTLJSNode/CHTLJSNode.h"
#include <vector>
#include <memory>
#include "CHTLJSContext.h"

#include "../CHTLJSNode/SequenceNode.h"
#include "../CHTLJSNode/RouterNode.h"
#include "../CHTLJSNode/ScriptLoaderNode.h"

namespace CHTLJS {

class CHTLJSParser {
public:
    explicit CHTLJSParser(std::vector<CHTLJSToken>& tokens, std::shared_ptr<CHTLJSContext> context);
    std::unique_ptr<SequenceNode> parse();

private:
    // Statement-level parsing
    std::unique_ptr<CHTLJSNode> parseStatement();
    std::unique_ptr<CHTLJSNode> parseVirDeclaration();

    // Expression-level parsing
    std::unique_ptr<CHTLJSNode> parseExpression();
    std::unique_ptr<CHTLJSNode> parsePrimaryExpression();
    std::unique_ptr<CHTLJSNode> parseMemberAccessExpression(std::unique_ptr<CHTLJSNode> object);

    // Block parsing
    std::unique_ptr<CHTLJSNode> parseListenBlock(std::unique_ptr<CHTLJSNode> object);
    std::unique_ptr<CHTLJSNode> parseDelegateBlock(std::unique_ptr<CHTLJSNode> object);
    std::unique_ptr<CHTLJSNode> parseAnimateBlock();
    std::unique_ptr<CHTLJSNode> parseRouterBlock();
    std::unique_ptr<CHTLJSNode> parseScriptLoaderBlock();

    // Helpers
    const CHTLJSToken& peek() const;
    const CHTLJSToken& advance();
    bool isAtEnd() const;
    const CHTLJSToken& consume(CHTLJSTokenType type, const std::string& error_message);
    bool match(CHTLJSTokenType type);


    std::vector<CHTLJSToken>& tokens_;
    std::shared_ptr<CHTLJSContext> context_;
    size_t current_ = 0;
};

} // namespace CHTLJS

#endif // CHTL_JS_PARSER_H
