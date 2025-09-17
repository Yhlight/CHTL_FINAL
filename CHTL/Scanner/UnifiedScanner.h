#ifndef UNIFIED_SCANNER_H
#define UNIFIED_SCANNER_H

#include <string>
#include <vector>
#include <memory>
#include <map>

namespace CHTL {

/**
 * 统一扫描器 - CHTL项目的核心组件
 * 负责分离CHTL、CHTL JS、CSS、JS代码
 */
class UnifiedScanner {
public:
    /**
     * 代码片段类型
     */
    enum class CodeType {
        CHTL,           // CHTL代码
        CHTL_JS,        // CHTL JS代码
        CSS,            // CSS代码
        JS,             // JavaScript代码
        UNKNOWN         // 未知类型
    };

    /**
     * 代码片段结构
     */
    struct CodeFragment {
        CodeType type;
        std::string content;
        size_t startPos;
        size_t endPos;
        std::string placeholder;  // 占位符（用于替换）
        
        CodeFragment(CodeType t, const std::string& c, size_t start, size_t end)
            : type(t), content(c), startPos(start), endPos(end) {}
    };

    /**
     * 扫描结果
     */
    struct ScanResult {
        std::vector<CodeFragment> fragments;
        std::map<std::string, std::string> placeholderMap;  // 占位符映射
        std::string processedCode;  // 处理后的代码
    };

public:
    UnifiedScanner();
    ~UnifiedScanner();

    /**
     * 扫描代码，分离不同类型的代码片段
     * @param code 输入代码
     * @return 扫描结果
     */
    ScanResult scan(const std::string& code);

    /**
     * 恢复占位符为原始代码
     * @param processedCode 处理后的代码
     * @param placeholderMap 占位符映射
     * @return 恢复后的代码
     */
    std::string restorePlaceholders(const std::string& processedCode, 
                                   const std::map<std::string, std::string>& placeholderMap);

private:
    /**
     * 双指针扫描算法
     * @param code 输入代码
     * @param start 开始位置
     * @param end 结束位置
     * @return 扫描到的代码片段
     */
    std::vector<CodeFragment> dualPointerScan(const std::string& code, size_t start, size_t end);

    /**
     * 前置截取算法
     * @param code 输入代码
     * @param keyword 关键字
     * @param pos 位置
     * @return 截取结果
     */
    std::string preExtract(const std::string& code, const std::string& keyword, size_t pos);

    /**
     * 识别代码类型
     * @param code 代码片段
     * @return 代码类型
     */
    CodeType identifyCodeType(const std::string& code);

    /**
     * 检查是否为CHTL语法边界
     * @param code 代码
     * @param pos 位置
     * @return 是否为边界
     */
    bool isCHTLSyntaxBoundary(const std::string& code, size_t pos);

    /**
     * 检查是否为CHTL JS语法边界
     * @param code 代码
     * @param pos 位置
     * @return 是否为边界
     */
    bool isCHTLJSSyntaxBoundary(const std::string& code, size_t pos);

    /**
     * 生成占位符
     * @param type 代码类型
     * @param index 索引
     * @return 占位符
     */
    std::string generatePlaceholder(CodeType type, size_t index);

    /**
     * 处理局部样式块中的CHTL语法
     * @param code 代码
     * @return 处理后的代码
     */
    std::string processLocalStyleBlock(const std::string& code);

    /**
     * 处理局部脚本块中的混合语法
     * @param code 代码
     * @return 处理后的代码
     */
    std::string processLocalScriptBlock(const std::string& code);

    /**
     * 分离CHTL JS和JS代码
     * @param code 代码
     * @return 分离结果
     */
    std::vector<CodeFragment> separateCHTLJSAndJS(const std::string& code);
    
    /**
     * 查找CHTL块结束位置
     * @param code 代码
     * @param start 开始位置
     * @return 结束位置
     */
    size_t findCHTLBlockEnd(const std::string& code, size_t start);
    
    /**
     * 查找CHTL JS块结束位置
     * @param code 代码
     * @param start 开始位置
     * @return 结束位置
     */
    size_t findCHTLJSBlockEnd(const std::string& code, size_t start);
    
    /**
     * 处理全局样式块
     * @param code 代码
     * @return 处理后的代码
     */
    std::string processGlobalStyleBlocks(const std::string& code);
    
    /**
     * 处理局部脚本块
     * @param code 代码
     * @return 处理后的代码
     */
    std::string processLocalScriptBlocks(const std::string& code);

private:
    size_t m_placeholderCounter;
    std::map<std::string, std::string> m_placeholderMap;
};

} // namespace CHTL

#endif // UNIFIED_SCANNER_H