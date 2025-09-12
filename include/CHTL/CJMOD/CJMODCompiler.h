#pragma once

#include "CHTL/CHTLContext.h"
#include "CHTL/CJMOD/CJMODAPI.h"
#include "CHTL/Token.h"
#include <string>
#include <memory>
#include <vector>

namespace CHTL {

/**
 * @brief CJMOD编译器类
 * 
 * 负责处理CHTL JS扩展语法
 */
class CJMODCompiler {
public:
    /**
     * @brief 构造函数
     * @param context CHTL上下文
     */
    explicit CJMODCompiler(std::shared_ptr<CHTLContext> context);
    
    /**
     * @brief 析构函数
     */
    ~CJMODCompiler();
    
    /**
     * @brief 编译CHTL JS代码
     * @param sourceCode 源代码
     * @return 编译后的JavaScript代码
     */
    std::string compile(const std::string& sourceCode);
    
    /**
     * @brief 编译Token序列
     * @param tokens Token列表
     * @return 编译后的JavaScript代码
     */
    std::string compile(const TokenList& tokens);
    
    /**
     * @brief 重置编译器状态
     */
    void reset();
    
    /**
     * @brief 设置调试模式
     * @param debug 是否启用调试模式
     */
    void setDebugMode(bool debug);
    
    /**
     * @brief 是否处于调试模式
     * @return 是否调试模式
     */
    bool isDebugMode() const;
    
    /**
     * @brief 获取CJMOD API
     * @return CJMOD API引用
     */
    CJMODAPI& getAPI();
    
    /**
     * @brief 获取统计信息
     * @return 统计信息字符串
     */
    std::string getStatistics() const;

private:
    /**
     * @brief 编译VIR语句
     * @param tokens Token列表
     * @param pos 当前位置
     * @return 编译结果
     */
    std::string compileVIR(const TokenList& tokens, size_t& pos);
    
    /**
     * @brief 编译LISTEN语句
     * @param tokens Token列表
     * @param pos 当前位置
     * @return 编译结果
     */
    std::string compileLISTEN(const TokenList& tokens, size_t& pos);
    
    /**
     * @brief 编译ANIMATE语句
     * @param tokens Token列表
     * @param pos 当前位置
     * @return 编译结果
     */
    std::string compileANIMATE(const TokenList& tokens, size_t& pos);
    
    /**
     * @brief 编译ROUTER语句
     * @param tokens Token列表
     * @param pos 当前位置
     * @return 编译结果
     */
    std::string compileROUTER(const TokenList& tokens, size_t& pos);
    
    /**
     * @brief 编译DELEGATE语句
     * @param tokens Token列表
     * @param pos 当前位置
     * @return 编译结果
     */
    std::string compileDELEGATE(const TokenList& tokens, size_t& pos);
    
    /**
     * @brief 编译FILELOADER语句
     * @param tokens Token列表
     * @param pos 当前位置
     * @return 编译结果
     */
    std::string compileFILELOADER(const TokenList& tokens, size_t& pos);
    
    /**
     * @brief 解析参数列表
     * @param tokens Token列表
     * @param pos 当前位置
     * @return 参数列表
     */
    std::vector<std::string> parseParameters(const TokenList& tokens, size_t& pos);
    
    /**
     * @brief 跳过空白Token
     * @param tokens Token列表
     * @param pos 当前位置
     */
    void skipWhitespace(const TokenList& tokens, size_t& pos);
    
    /**
     * @brief 检查Token类型
     * @param tokens Token列表
     * @param pos 当前位置
     * @param type Token类型
     * @return 是否匹配
     */
    bool checkToken(const TokenList& tokens, size_t pos, TokenType type);
    
    /**
     * @brief 获取当前Token
     * @param tokens Token列表
     * @param pos 当前位置
     * @return 当前Token
     */
    const Token& getCurrentToken(const TokenList& tokens, size_t pos);
    
    /**
     * @brief 添加错误信息
     * @param message 错误信息
     */
    void addError(const std::string& message);
    
    /**
     * @brief 添加警告信息
     * @param message 警告信息
     */
    void addWarning(const std::string& message);

private:
    std::shared_ptr<CHTLContext> m_context;
    CJMODAPI m_api;
    bool m_debugMode;
    
    // 统计信息
    size_t m_compilationCount;
    size_t m_successfulCompilations;
    size_t m_failedCompilations;
};

} // namespace CHTL