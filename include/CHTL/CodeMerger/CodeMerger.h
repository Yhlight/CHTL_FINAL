#pragma once

#include "CHTL/CHTLContext.h"
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

namespace CHTL {

/**
 * @brief 代码片段类型
 */
enum class CodeFragmentType {
    CHTL,           // CHTL代码片段
    CHTL_JS,        // CHTL JS代码片段
    CSS,            // CSS代码片段
    JS,             // JavaScript代码片段
    HTML,           // HTML代码片段
    UNKNOWN         // 未知类型
};

/**
 * @brief 代码片段结构
 */
struct CodeFragment {
    CodeFragmentType type;
    std::string content;
    size_t line;
    size_t column;
    std::string sourceFile;
    bool processed;
    
    CodeFragment(CodeFragmentType t, const std::string& c, size_t l = 0, size_t col = 0, const std::string& file = "")
        : type(t), content(c), line(l), column(col), sourceFile(file), processed(false) {}
};

/**
 * @brief 代码合并器类
 * 
 * 负责将编译后的代码片段合并为最终输出
 */
class CodeMerger {
public:
    /**
     * @brief 构造函数
     * @param context CHTL上下文
     */
    explicit CodeMerger(std::shared_ptr<CHTLContext> context);
    
    /**
     * @brief 析构函数
     */
    ~CodeMerger();
    
    /**
     * @brief 添加代码片段
     * @param fragment 代码片段
     */
    void addFragment(const CodeFragment& fragment);
    
    /**
     * @brief 添加代码片段
     * @param type 片段类型
     * @param content 内容
     * @param line 行号
     * @param column 列号
     * @param sourceFile 源文件
     */
    void addFragment(CodeFragmentType type, const std::string& content, size_t line = 0, size_t column = 0, const std::string& sourceFile = "");
    
    /**
     * @brief 合并所有代码片段
     * @return 合并后的完整代码
     */
    std::string merge();
    
    /**
     * @brief 合并指定类型的代码片段
     * @param type 片段类型
     * @return 合并后的代码
     */
    std::string mergeByType(CodeFragmentType type);
    
    /**
     * @brief 生成HTML输出
     * @param includeDefaultStructure 是否包含默认HTML结构
     * @return 生成的HTML代码
     */
    std::string generateHTML(bool includeDefaultStructure = false);
    
    /**
     * @brief 生成CSS输出
     * @return 生成的CSS代码
     */
    std::string generateCSS();
    
    /**
     * @brief 生成JavaScript输出
     * @return 生成的JavaScript代码
     */
    std::string generateJavaScript();
    
    /**
     * @brief 清除所有代码片段
     */
    void clear();
    
    /**
     * @brief 获取代码片段数量
     * @return 片段数量
     */
    size_t getFragmentCount() const;
    
    /**
     * @brief 获取指定类型的代码片段数量
     * @param type 片段类型
     * @return 片段数量
     */
    size_t getFragmentCount(CodeFragmentType type) const;
    
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
     * @brief 获取统计信息
     * @return 统计信息字符串
     */
    std::string getStatistics() const;

private:
    /**
     * @brief 处理占位符
     * @param content 内容
     * @return 处理后的内容
     */
    std::string processPlaceholders(const std::string& content);
    
    /**
     * @brief 解码占位符
     * @param content 内容
     * @return 解码后的内容
     */
    std::string decodePlaceholders(const std::string& content);
    
    /**
     * @brief 生成HTML头部
     * @return HTML头部
     */
    std::string generateHTMLHead();
    
    /**
     * @brief 生成HTML主体
     * @return HTML主体
     */
    std::string generateHTMLBody();
    
    /**
     * @brief 生成HTML尾部
     * @return HTML尾部
     */
    std::string generateHTMLTail();
    
    /**
     * @brief 获取片段类型名称
     * @param type 片段类型
     * @return 类型名称
     */
    static std::string getFragmentTypeName(CodeFragmentType type);
    
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
    std::vector<CodeFragment> m_fragments;
    std::unordered_map<CodeFragmentType, std::vector<CodeFragment*>> m_fragmentsByType;
    bool m_debugMode;
    
    // 统计信息
    size_t m_totalFragments;
    size_t m_processedFragments;
    size_t m_mergeCount;
    size_t m_successfulMerges;
    size_t m_failedMerges;
};

} // namespace CHTL