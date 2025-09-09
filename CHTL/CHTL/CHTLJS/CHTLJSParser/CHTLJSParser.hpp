#pragma once

#include <CHTL/CHTLJS/CHTLJSLexer/CHTLJSLexer.hpp>
#include <CHTL/CHTLJS/CHTLJSNode/CHTLJSBaseNode.hpp>
#include <CHTL/CHTLJS/CHTLJSNode/FileLoaderNode.hpp>
#include <CHTL/CHTLJS/CHTLJSNode/EnhancedSelectorNode.hpp>
#include <CHTL/CHTLJS/CHTLJSNode/StatementNode.hpp>
#include <memory>
#include <vector>
#include <string>

namespace CHTL {

/**
 * @brief CHTL JS 解析器
 * 
 * 根据 CHTL.md 文档第 1200-1800 行实现
 * 支持 CHTL JS 扩展语法的解析
 */
class CHTLJSParser {
public:
    CHTLJSParser(std::istream& input);
    
    // 解析 CHTL JS 代码
    std::shared_ptr<CHTLJSBaseNode> parse();
    
    // 获取错误信息
    const std::string& getErrorMessage() const { return errorMessage_; }
    
    // 检查是否有错误
    bool hasError() const { return hasError_; }
    
private:
    CHTLJSLexer lexer_;
    CHTLJSToken current_;
    bool hasError_;
    std::string errorMessage_;
    
    // 消费当前词法单元
    bool consume(CHTLJSTokenType type);
    
    // 解析文件加载器
    std::shared_ptr<CHTLJSBaseNode> parseFileLoader();
    
    // 解析脚本块
    std::shared_ptr<CHTLJSBaseNode> parseScript();
    
    // 解析增强选择器
    std::shared_ptr<CHTLJSBaseNode> parseEnhancedSelector();
    
    // 解析动画
    std::shared_ptr<CHTLJSBaseNode> parseAnimate();
    
    // 解析监听器
    std::shared_ptr<CHTLJSBaseNode> parseListen();
    
    // 解析工具表达式
    std::shared_ptr<CHTLJSBaseNode> parseUtil();
    
    // 解析表达式
    std::shared_ptr<CHTLJSBaseNode> parseExpression();
    
    // 解析语句
    std::shared_ptr<CHTLJSBaseNode> parseStatement();
    
    // 表达式解析方法
    std::shared_ptr<CHTLJSBaseNode> parseLogicalOrExpression();
    std::shared_ptr<CHTLJSBaseNode> parseLogicalAndExpression();
    std::shared_ptr<CHTLJSBaseNode> parseEqualityExpression();
    std::shared_ptr<CHTLJSBaseNode> parseRelationalExpression();
    std::shared_ptr<CHTLJSBaseNode> parseAdditiveExpression();
    std::shared_ptr<CHTLJSBaseNode> parseMultiplicativeExpression();
    std::shared_ptr<CHTLJSBaseNode> parseUnaryExpression();
    std::shared_ptr<CHTLJSBaseNode> parsePrimaryExpression();
    
    // 语句解析方法
    std::shared_ptr<CHTLJSBaseNode> parseVariableDeclaration();
    std::shared_ptr<CHTLJSBaseNode> parseFunctionDeclaration();
    std::shared_ptr<CHTLJSBaseNode> parseIfStatement();
    std::shared_ptr<CHTLJSBaseNode> parseWhileStatement();
    std::shared_ptr<CHTLJSBaseNode> parseForStatement();
    std::shared_ptr<CHTLJSBaseNode> parseReturnStatement();
    
    // 报告错误
    void reportError(const std::string& message);
    
    // 同步到下一个语句
    void synchronize();
};

} // namespace CHTL