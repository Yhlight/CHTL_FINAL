#ifndef CHTLJSPARSER_H
#define CHTLJSPARSER_H

#include "../CHTLJSLexer/Token.h"
#include "../CHTLJSNode/CHTLJSBaseNode.h"
#include <vector>
#include <memory>

namespace CHTLJS {

/**
 * CHTL JS解析器
 * 负责将令牌序列解析为AST
 */
class CHTLJSParser {
public:
    CHTLJSParser(const std::vector<Token>& tokens);
    ~CHTLJSParser();
    
    // 主要方法
    std::shared_ptr<CHTLJSBaseNode> parse();
    std::shared_ptr<CHTLJSBaseNode> parseStatement();
    std::shared_ptr<CHTLJSBaseNode> parseExpression();
    
    // 状态查询
    bool hasMoreTokens() const;
    const Token& currentToken() const;
    const Token& peekToken() const;
    
private:
    std::vector<Token> tokens_;
    size_t position_;
    
    // 辅助方法
    void advance();
    bool match(TokenType type);
    bool match(const std::vector<TokenType>& types);
    Token consume(TokenType type);
    Token consume(const std::vector<TokenType>& types);
    
    // 解析方法
    std::shared_ptr<CHTLJSBaseNode> parseScriptLoader();
    std::shared_ptr<CHTLJSBaseNode> parseListen();
    std::shared_ptr<CHTLJSBaseNode> parseAnimate();
    std::shared_ptr<CHTLJSBaseNode> parseRouter();
    std::shared_ptr<CHTLJSBaseNode> parseVir();
    std::shared_ptr<CHTLJSBaseNode> parseINeverAway();
    std::shared_ptr<CHTLJSBaseNode> parseUtilThen();
    std::shared_ptr<CHTLJSBaseNode> parsePrintMylove();
    std::shared_ptr<CHTLJSBaseNode> parseSelector();
    std::shared_ptr<CHTLJSBaseNode> parseResponsiveValue();
    std::shared_ptr<CHTLJSBaseNode> parseBlock();
    std::shared_ptr<CHTLJSBaseNode> parseFunctionCall();
    
    // 参数解析
    void parseParameters(std::shared_ptr<CHTLJSBaseNode> functionNode);
    std::string parseStringValue();
    std::shared_ptr<CHTLJSBaseNode> parseNodeValue();
    
    // 错误处理
    void error(const std::string& message);
    void error(const std::string& message, const Token& token);
};

} // namespace CHTLJS

#endif // CHTLJSPARSER_H