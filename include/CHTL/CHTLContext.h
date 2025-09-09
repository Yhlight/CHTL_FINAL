#pragma once

#include <string>
#include <memory>
#include <unordered_map>
#include <vector>
#include <fstream>

namespace CHTL {

// Forward declarations
class CHTLLexer;
class CHTLParser;
class CHTLGenerator;
class CHTLNode;
class CHTLState;

/**
 * @brief CHTL编译器上下文，管理整个编译过程
 * 
 * CHTLContext是CHTL编译器的核心类，负责协调各个组件的工作，
 * 管理编译状态，并提供统一的接口供外部调用。
 */
class CHTLContext {
public:
    CHTLContext();
    ~CHTLContext();

    /**
     * @brief 编译CHTL文件
     * @param inputFile 输入文件路径
     * @param outputFile 输出文件路径
     * @return 编译是否成功
     */
    bool compile(const std::string& inputFile, const std::string& outputFile = "");

    /**
     * @brief 编译CHTL字符串
     * @param input 输入字符串
     * @return 编译结果HTML字符串
     */
    std::string compileString(const std::string& input);

    /**
     * @brief 设置编译选项
     * @param option 选项名称
     * @param value 选项值
     */
    void setOption(const std::string& option, const std::string& value);

    /**
     * @brief 获取编译选项
     * @param option 选项名称
     * @return 选项值
     */
    std::string getOption(const std::string& option) const;

    /**
     * @brief 获取编译错误信息
     * @return 错误信息列表
     */
    const std::vector<std::string>& getErrors() const;

    /**
     * @brief 获取编译警告信息
     * @return 警告信息列表
     */
    const std::vector<std::string>& getWarnings() const;

    /**
     * @brief 清空错误和警告信息
     */
    void clearMessages();

    /**
     * @brief 设置调试模式
     * @param debug 是否启用调试模式
     */
    void setDebugMode(bool debug);

    /**
     * @brief 获取当前编译状态
     * @return 编译状态
     */
    CHTLState* getState() const;

private:
    std::unique_ptr<CHTLLexer> m_lexer;
    std::unique_ptr<CHTLParser> m_parser;
    std::unique_ptr<CHTLGenerator> m_generator;
    std::unique_ptr<CHTLState> m_state;
    
    std::unordered_map<std::string, std::string> m_options;
    std::vector<std::string> m_errors;
    std::vector<std::string> m_warnings;
    
    bool m_debugMode;
    
    /**
     * @brief 初始化编译器组件
     */
    void initializeComponents();
    
    /**
     * @brief 添加错误信息
     * @param error 错误信息
     */
    void addError(const std::string& error);
    
    /**
     * @brief 添加警告信息
     * @param warning 警告信息
     */
    void addWarning(const std::string& warning);
    
    /**
     * @brief 验证输入文件
     * @param inputFile 输入文件路径
     * @return 文件是否有效
     */
    bool validateInputFile(const std::string& inputFile);
    
    /**
     * @brief 生成输出文件名
     * @param inputFile 输入文件路径
     * @return 输出文件路径
     */
    std::string generateOutputFileName(const std::string& inputFile);
};

} // namespace CHTL