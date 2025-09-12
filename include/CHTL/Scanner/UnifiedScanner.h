#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include "CHTL/CodeMerger/CodeMerger.h"

namespace CHTL {

/**
 * @brief 统一扫描器类
 * 
 * 负责将混合的CHTL代码分离为不同类型的代码片段
 * 这是CHTL项目的核心组件，需要极其精妙的算法来支持
 */
class UnifiedScanner {
public:
    /**
     * @brief 构造函数
     * @param context CHTL上下文
     */
    explicit UnifiedScanner(std::shared_ptr<CHTLContext> context = nullptr);
    
    /**
     * @brief 析构函数
     */
    ~UnifiedScanner();
    
    /**
     * @brief 扫描代码并分离片段
     * @param sourceCode 源代码
     * @return 代码片段列表
     */
    std::vector<CodeFragment> scan(const std::string& sourceCode);
    
    /**
     * @brief 设置占位符前缀
     * @param prefix 占位符前缀
     */
    void setPlaceholderPrefix(const std::string& prefix);
    
    /**
     * @brief 获取占位符前缀
     * @return 占位符前缀
     */
    const std::string& getPlaceholderPrefix() const;
    
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

private:
    /**
     * @brief 预扫描代码，识别语法边界
     * @param sourceCode 源代码
     * @return 语法边界信息
     */
    std::vector<std::pair<size_t, CodeFragmentType>> preScan(const std::string& sourceCode);
    
    /**
     * @brief 识别CHTL语法边界
     * @param sourceCode 源代码
     * @param startPos 开始位置
     * @return 结束位置，未找到返回-1
     */
    int identifyCHTLBoundary(const std::string& sourceCode, size_t startPos);
    
    /**
     * @brief 识别CHTL JS语法边界
     * @param sourceCode 源代码
     * @param startPos 开始位置
     * @return 结束位置，未找到返回-1
     */
    int identifyCHTLJSBoundary(const std::string& sourceCode, size_t startPos);
    
    /**
     * @brief 识别CSS语法边界
     * @param sourceCode 源代码
     * @param startPos 开始位置
     * @return 结束位置，未找到返回-1
     */
    int identifyCSSBoundary(const std::string& sourceCode, size_t startPos);
    
    /**
     * @brief 识别JS语法边界
     * @param sourceCode 源代码
     * @param startPos 开始位置
     * @return 结束位置，未找到返回-1
     */
    int identifyJSBoundary(const std::string& sourceCode, size_t startPos);
    
    /**
     * @brief 使用占位符替换代码片段
     * @param sourceCode 源代码
     * @param fragments 代码片段列表
     * @return 替换后的代码
     */
    std::string replaceWithPlaceholders(const std::string& sourceCode, 
                                      const std::vector<CodeFragment>& fragments);
    
    /**
     * @brief 生成占位符
     * @param type 代码片段类型
     * @param index 索引
     * @return 占位符
     */
    std::string generatePlaceholder(CodeFragmentType type, size_t index);
    
    /**
     * @brief 检查是否为CHTL关键字
     * @param word 单词
     * @return 是否为CHTL关键字
     */
    bool isCHTLKeyword(const std::string& word) const;
    
    /**
     * @brief 检查是否为CHTL JS关键字
     * @param word 单词
     * @return 是否为CHTL JS关键字
     */
    bool isCHTLJSKeyword(const std::string& word) const;
    
    /**
     * @brief 跳过空白字符
     * @param sourceCode 源代码
     * @param pos 当前位置
     * @return 跳过后的位置
     */
    size_t skipWhitespace(const std::string& sourceCode, size_t pos) const;
    
    /**
     * @brief 跳过注释
     * @param sourceCode 源代码
     * @param pos 当前位置
     * @return 跳过后的位置
     */
    size_t skipComment(const std::string& sourceCode, size_t pos) const;
    
    /**
     * @brief 跳过字符串
     * @param sourceCode 源代码
     * @param pos 当前位置
     * @param quote 引号字符
     * @return 跳过后的位置
     */
    size_t skipString(const std::string& sourceCode, size_t pos, char quote) const;
    
    /**
     * @brief 查找匹配的右括号
     * @param sourceCode 源代码
     * @param pos 左括号位置
     * @param leftChar 左括号字符
     * @param rightChar 右括号字符
     * @return 右括号位置，未找到返回-1
     */
    int findMatchingBracket(const std::string& sourceCode, size_t pos, 
                           char leftChar, char rightChar) const;
    
    /**
     * @brief 计算行号和列号
     * @param sourceCode 源代码
     * @param pos 位置
     * @return 行号和列号的pair
     */
    std::pair<size_t, size_t> getLineColumn(const std::string& sourceCode, size_t pos) const;

private:
    std::string m_placeholderPrefix;
    bool m_debugMode;
    
    // 关键字映射
    std::unordered_map<std::string, bool> m_chtlKeywords;
    std::unordered_map<std::string, bool> m_chtlJSKeywords;
    
    // 占位符计数器
    std::unordered_map<CodeFragmentType, size_t> m_placeholderCounters;
};

} // namespace CHTL