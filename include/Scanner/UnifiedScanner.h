#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

namespace CHTL {

/**
 * @brief 统一扫描器 - CHTL项目的核心组件
 * 负责分离CHTL、CHTL JS、CSS、JS代码片段
 */
class UnifiedScanner {
public:
    /**
     * @brief 代码片段类型
     */
    enum class CodeType {
        CHTL,      // CHTL代码
        CHTL_JS,   // CHTL JS代码
        CSS,       // CSS代码
        JS,        // JavaScript代码
        UNKNOWN    // 未知类型
    };

    /**
     * @brief 代码片段结构
     */
    struct CodeFragment {
        CodeType type;
        std::string content;
        size_t start_pos;
        size_t end_pos;
        std::string placeholder; // 占位符（如果使用）
    };

    UnifiedScanner();
    ~UnifiedScanner() = default;

    /**
     * @brief 扫描并分离代码
     * @param source_code 源代码
     * @return 分离后的代码片段列表
     */
    std::vector<CodeFragment> scan(const std::string& source_code);

    /**
     * @brief 设置占位符前缀
     * @param prefix 占位符前缀
     */
    void setPlaceholderPrefix(const std::string& prefix);

    /**
     * @brief 获取占位符映射
     * @return 占位符到原始代码的映射
     */
    const std::unordered_map<std::string, std::string>& getPlaceholderMap() const;

    /**
     * @brief 清空占位符映射
     */
    void clearPlaceholderMap();

private:
    /**
     * @brief 识别代码类型
     * @param content 代码内容
     * @return 代码类型
     */
    CodeType identifyCodeType(const std::string& content);

    /**
     * @brief 处理CHTL代码片段
     * @param content 代码内容
     * @param start_pos 开始位置
     * @return 代码片段
     */
    CodeFragment processCHTLFragment(const std::string& content, size_t start_pos);

    /**
     * @brief 处理CHTL JS代码片段
     * @param content 代码内容
     * @param start_pos 开始位置
     * @return 代码片段
     */
    CodeFragment processCHTLJSFragment(const std::string& content, size_t start_pos);

    /**
     * @brief 处理CSS代码片段
     * @param content 代码内容
     * @param start_pos 开始位置
     * @return 代码片段
     */
    CodeFragment processCSSFragment(const std::string& content, size_t start_pos);

    /**
     * @brief 处理JS代码片段
     * @param content 代码内容
     * @param start_pos 开始位置
     * @return 代码片段
     */
    CodeFragment processJSFragment(const std::string& content, size_t start_pos);

    /**
     * @brief 创建占位符
     * @param content 原始内容
     * @return 占位符
     */
    std::string createPlaceholder(const std::string& content);

    /**
     * @brief 双指针扫描算法
     * @param content 代码内容
     * @param keyword 关键字
     * @return 扫描结果
     */
    std::vector<std::pair<size_t, size_t>> doublePointerScan(
        const std::string& content, 
        const std::string& keyword
    );

    /**
     * @brief 前置截取算法
     * @param content 代码内容
     * @param keyword 关键字
     * @return 截取结果
     */
    std::string preemptiveCapture(const std::string& content, const std::string& keyword);

    /**
     * @brief 检查语法边界
     * @param content 代码内容
     * @param pos 位置
     * @return 是否为语法边界
     */
    bool isSyntaxBoundary(const std::string& content, size_t pos);

    /**
     * @brief 宽判处理
     * @param content 代码内容
     * @return 处理结果
     */
    std::vector<CodeFragment> wideJudgment(const std::string& content);

    /**
     * @brief 严判处理
     * @param content 代码内容
     * @return 处理结果
     */
    std::vector<CodeFragment> strictJudgment(const std::string& content);

private:
    std::string placeholder_prefix_;
    std::unordered_map<std::string, std::string> placeholder_map_;
    size_t placeholder_counter_;
    
    // 语法边界标识符
    static const std::vector<std::string> CHTL_BOUNDARIES;
    static const std::vector<std::string> CHTL_JS_BOUNDARIES;
    static const std::vector<std::string> CSS_BOUNDARIES;
    static const std::vector<std::string> JS_BOUNDARIES;
};

} // namespace CHTL