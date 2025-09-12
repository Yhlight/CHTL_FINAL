#pragma once

#include "CHTLJSLexer.h"
#include "CHTL/CHTLNode.h"
#include <memory>
#include <vector>

namespace CHTL {

/**
 * @brief CHTL JS语法分析器
 */
class CHTLJSParser {
public:
    CHTLJSParser();
    ~CHTLJSParser() = default;

    /**
     * @brief 解析源代码为AST
     * @param tokens 词法单元列表
     * @return AST根节点
     */
    std::shared_ptr<CHTLNode> parse(const std::vector<CHTLJSToken>& tokens);

    /**
     * @brief 解析源代码为AST
     * @param source_code 源代码
     * @return AST根节点
     */
    std::shared_ptr<CHTLNode> parse(const std::string& source_code);

    /**
     * @brief 重置解析器状态
     */
    void reset();

    /**
     * @brief 设置调试模式
     * @param enabled 是否启用调试模式
     */
    void setDebugMode(bool enabled);

    /**
     * @brief 获取错误信息
     * @return 错误信息列表
     */
    const std::vector<std::string>& getErrors() const;

    /**
     * @brief 清空错误信息
     */
    void clearErrors();

private:
    /**
     * @brief 解析根节点
     * @return 根节点
     */
    std::shared_ptr<CHTLNode> parseRoot();

    /**
     * @brief 解析表达式
     * @return 表达式节点
     */
    std::shared_ptr<CHTLNode> parseExpression();

    /**
     * @brief 解析赋值表达式
     * @return 赋值表达式节点
     */
    std::shared_ptr<CHTLNode> parseAssignmentExpression();

    /**
     * @brief 解析条件表达式
     * @return 条件表达式节点
     */
    std::shared_ptr<CHTLNode> parseConditionalExpression();

    /**
     * @brief 解析逻辑或表达式
     * @return 逻辑或表达式节点
     */
    std::shared_ptr<CHTLNode> parseLogicalOrExpression();

    /**
     * @brief 解析逻辑与表达式
     * @return 逻辑与表达式节点
     */
    std::shared_ptr<CHTLNode> parseLogicalAndExpression();

    /**
     * @brief 解析相等表达式
     * @return 相等表达式节点
     */
    std::shared_ptr<CHTLNode> parseEqualityExpression();

    /**
     * @brief 解析关系表达式
     * @return 关系表达式节点
     */
    std::shared_ptr<CHTLNode> parseRelationalExpression();

    /**
     * @brief 解析加法表达式
     * @return 加法表达式节点
     */
    std::shared_ptr<CHTLNode> parseAdditiveExpression();

    /**
     * @brief 解析乘法表达式
     * @return 乘法表达式节点
     */
    std::shared_ptr<CHTLNode> parseMultiplicativeExpression();

    /**
     * @brief 解析一元表达式
     * @return 一元表达式节点
     */
    std::shared_ptr<CHTLNode> parseUnaryExpression();

    /**
     * @brief 解析主表达式
     * @return 主表达式节点
     */
    std::shared_ptr<CHTLNode> parsePrimaryExpression();

    /**
     * @brief 解析函数调用
     * @return 函数调用节点
     */
    std::shared_ptr<CHTLNode> parseFunctionCall();

    /**
     * @brief 解析成员访问
     * @return 成员访问节点
     */
    std::shared_ptr<CHTLNode> parseMemberAccess();

    /**
     * @brief 解析数组访问
     * @return 数组访问节点
     */
    std::shared_ptr<CHTLNode> parseArrayAccess();

    /**
     * @brief 解析CHTL JS函数
     * @return CHTL JS函数节点
     */
    std::shared_ptr<CHTLNode> parseCHTLJSFunction();

    /**
     * @brief 解析listen函数
     * @return listen函数节点
     */
    std::shared_ptr<CHTLNode> parseListenFunction();

    /**
     * @brief 解析animate函数
     * @return animate函数节点
     */
    std::shared_ptr<CHTLNode> parseAnimateFunction();

    /**
     * @brief 解析delegate函数
     * @return delegate函数节点
     */
    std::shared_ptr<CHTLNode> parseDelegateFunction();

    /**
     * @brief 解析router函数
     * @return router函数节点
     */
    std::shared_ptr<CHTLNode> parseRouterFunction();

    /**
     * @brief 解析fileloader函数
     * @return fileloader函数节点
     */
    std::shared_ptr<CHTLNode> parseFileloaderFunction();

    /**
     * @brief 解析vir声明
     * @return vir声明节点
     */
    std::shared_ptr<CHTLNode> parseVirDeclaration();

    /**
     * @brief 解析iNeverAway函数
     * @return iNeverAway函数节点
     */
    std::shared_ptr<CHTLNode> parseINeverAwayFunction();

    /**
     * @brief 解析util表达式
     * @return util表达式节点
     */
    std::shared_ptr<CHTLNode> parseUtilExpression();

    /**
     * @brief 解析增强选择器
     * @return 增强选择器节点
     */
    std::shared_ptr<CHTLNode> parseEnhancedSelector();

    /**
     * @brief 解析对象字面量
     * @return 对象字面量节点
     */
    std::shared_ptr<CHTLNode> parseObjectLiteral();

    /**
     * @brief 解析数组字面量
     * @return 数组字面量节点
     */
    std::shared_ptr<CHTLNode> parseArrayLiteral();

    /**
     * @brief 解析函数声明
     * @return 函数声明节点
     */
    std::shared_ptr<CHTLNode> parseFunctionDeclaration();

    /**
     * @brief 解析变量声明
     * @return 变量声明节点
     */
    std::shared_ptr<CHTLNode> parseVariableDeclaration();

    /**
     * @brief 解析if语句
     * @return if语句节点
     */
    std::shared_ptr<CHTLNode> parseIfStatement();

    /**
     * @brief 解析for循环
     * @return for循环节点
     */
    std::shared_ptr<CHTLNode> parseForStatement();

    /**
     * @brief 解析while循环
     * @return while循环节点
     */
    std::shared_ptr<CHTLNode> parseWhileStatement();

    /**
     * @brief 解析标识符
     * @return 标识符字符串
     */
    std::string parseIdentifier();

    /**
     * @brief 解析字符串
     * @return 字符串
     */
    std::string parseString();

    /**
     * @brief 解析数字
     * @return 数字值
     */
    double parseNumber();

    /**
     * @brief 检查当前词法单元类型
     * @param type 期望的类型
     * @return 是否匹配
     */
    bool check(CHTLJSTokenType type) const;

    /**
     * @brief 检查当前词法单元类型并消费
     * @param type 期望的类型
     * @return 是否匹配
     */
    bool match(CHTLJSTokenType type);

    /**
     * @brief 消费指定类型的词法单元
     * @param type 期望的类型
     * @return 词法单元
     */
    CHTLJSToken consume(CHTLJSTokenType type);

    /**
     * @brief 获取前一个词法单元
     * @return 前一个词法单元
     */
    const CHTLJSToken& previous() const;

    /**
     * @brief 同步到下一个语句
     */
    void synchronize();

    /**
     * @brief 报告错误
     * @param message 错误信息
     */
    void reportError(const std::string& message);

    /**
     * @brief 报告错误
     * @param token 出错的词法单元
     * @param message 错误信息
     */
    void reportError(const CHTLJSToken& token, const std::string& message);

    /**
     * @brief 报告错误并抛出异常
     * @param message 错误信息
     */
    void error(const std::string& message);

    /**
     * @brief 报告错误并抛出异常
     * @param token 出错的词法单元
     * @param message 错误信息
     */
    void error(const CHTLJSToken& token, const std::string& message);

public:
    std::vector<CHTLJSToken> tokens_;
    size_t current_token_;
    
    /**
     * @brief 获取当前词法单元
     * @return 当前词法单元
     */
    const CHTLJSToken& current() const;

    /**
     * @brief 检查是否到达末尾
     * @return 是否到达末尾
     */
    bool isAtEnd() const;
    
    /**
     * @brief 解析语句
     * @return 语句节点
     */
    std::shared_ptr<CHTLNode> parseStatement();
    
    /**
     * @brief 消费当前词法单元
     * @return 当前词法单元
     */
    CHTLJSToken consume();

private:
    bool debug_mode_;
    std::vector<std::string> errors_;
};

} // namespace CHTL