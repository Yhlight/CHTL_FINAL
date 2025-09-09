#pragma once

#include "CHTLJSToken.h"
#include "CHTL/CHTLNode/BaseNode.h"
#include <vector>
#include <memory>
#include <stdexcept>

namespace CHTL {

class CHTLJSParser {
public:
    explicit CHTLJSParser(const std::vector<CHTLJSToken>& tokens);
    std::shared_ptr<BaseNode> parse();
    
private:
    const std::vector<CHTLJSToken>& tokens;
    size_t currentTokenIndex;
    
    // 辅助方法
    CHTLJSToken peek(int offset = 0) const;
    CHTLJSToken consume(CHTLJSTokenType type);
    CHTLJSToken consumeIdentifier();
    bool match(CHTLJSTokenType type);
    void throwError(const std::string& message) const;
    
    // 解析方法
    std::shared_ptr<BaseNode> parseProgram();
    std::shared_ptr<BaseNode> parseStatement();
    std::shared_ptr<BaseNode> parseExpression();
    std::shared_ptr<BaseNode> parseFileLoader();
    std::shared_ptr<BaseNode> parseListen();
    std::shared_ptr<BaseNode> parseDelegate();
    std::shared_ptr<BaseNode> parseAnimate();
    std::shared_ptr<BaseNode> parseVir();
    std::shared_ptr<BaseNode> parseRouter();
    std::shared_ptr<BaseNode> parseUtil();
    std::shared_ptr<BaseNode> parseSelector();
    std::shared_ptr<BaseNode> parseFunctionCall();
    std::shared_ptr<BaseNode> parseObjectLiteral();
    std::shared_ptr<BaseNode> parseArrayLiteral();
    std::shared_ptr<BaseNode> parseConditionalExpression();
    std::shared_ptr<BaseNode> parseLogicalExpression();
    std::shared_ptr<BaseNode> parseEqualityExpression();
    std::shared_ptr<BaseNode> parseRelationalExpression();
    std::shared_ptr<BaseNode> parseAdditiveExpression();
    std::shared_ptr<BaseNode> parseMultiplicativeExpression();
    std::shared_ptr<BaseNode> parseUnaryExpression();
    std::shared_ptr<BaseNode> parsePrimaryExpression();
    
    // 辅助解析方法
    std::shared_ptr<BaseNode> parseProperty();
    std::shared_ptr<BaseNode> parseEventHandlers();
    std::shared_ptr<BaseNode> parseAnimationConfig();
    std::shared_ptr<BaseNode> parseRouteConfig();
    std::shared_ptr<BaseNode> parseVirtualObjectConfig();
};

} // namespace CHTL