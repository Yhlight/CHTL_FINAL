#ifndef UNIFIED_SCANNER_H
#define UNIFIED_SCANNER_H

#include <string>
#include <vector>
#include <memory>
#include <map>

namespace CHTL {

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
    std::string placeholder;  // 占位符，用于替换原始代码
    
    CodeFragment(CodeType t, const std::string& c, size_t start, size_t end)
        : type(t), content(c), startPos(start), endPos(end) {}
};

/**
 * 统一扫描器 - CHTL项目核心组件
 * 
 * 负责：
 * 1. 分离全局style块和局部script内部允许的部分CHTL语法
 * 2. 完全分离JS和CHTL JS代码
 * 3. 使用占位符机制保持代码边界完整性
 */
class UnifiedScanner {
public:
    UnifiedScanner();
    ~UnifiedScanner();

    /**
     * 扫描并分离代码
     * @param sourceCode 源代码
     * @return 分离后的代码片段列表
     */
    std::vector<CodeFragment> scan(const std::string& sourceCode);

    /**
     * 恢复占位符为原始代码
     * @param code 包含占位符的代码
     * @return 恢复后的代码
     */
    std::string restorePlaceholders(const std::string& code);

    /**
     * 获取占位符映射表
     * @return 占位符到原始代码的映射
     */
    const std::map<std::string, std::string>& getPlaceholderMap() const;

private:
    // 占位符映射表
    std::map<std::string, std::string> placeholderMap;
    
    // 占位符计数器
    size_t placeholderCounter;
    
    /**
     * 生成唯一占位符
     * @param type 代码类型
     * @return 占位符字符串
     */
    std::string generatePlaceholder(CodeType type);
    
    /**
     * 检测代码类型
     * @param code 代码片段
     * @return 代码类型
     */
    CodeType detectCodeType(const std::string& code);
    
    /**
     * 检测CHTL语法边界
     * @param code 代码
     * @param pos 当前位置
     * @return 是否为CHTL语法边界
     */
    bool isCHTLBoundary(const std::string& code, size_t pos);
    
    /**
     * 检测CHTL JS语法边界
     * @param code 代码
     * @param pos 当前位置
     * @return 是否为CHTL JS语法边界
     */
    bool isCHTLJSBoundary(const std::string& code, size_t pos);
    
    /**
     * 检测CSS语法边界
     * @param code 代码
     * @param pos 当前位置
     * @return 是否为CSS语法边界
     */
    bool isCSSBoundary(const std::string& code, size_t pos);
    
    /**
     * 检测JS语法边界
     * @param code 代码
     * @param pos 当前位置
     * @return 是否为JS语法边界
     */
    bool isJSBoundary(const std::string& code, size_t pos);
    
    /**
     * 使用双指针扫描算法分离代码
     * @param sourceCode 源代码
     * @return 代码片段列表
     */
    std::vector<CodeFragment> dualPointerScan(const std::string& sourceCode);
    
    /**
     * 前置截取处理
     * @param code 代码片段
     * @param keyword 关键字
     * @return 处理后的代码
     */
    std::string preprocessCode(const std::string& code, const std::string& keyword);
    
    /**
     * 处理嵌套代码块
     * @param code 代码
     * @param startPos 开始位置
     * @param endPos 结束位置
     * @return 处理后的代码片段
     */
    CodeFragment processNestedBlock(const std::string& code, size_t startPos, size_t endPos);
    
    /**
     * 宽判处理 - 处理大块CHTL代码
     * @param code 代码
     * @param startPos 开始位置
     * @return 代码片段
     */
    CodeFragment wideJudge(const std::string& code, size_t startPos);
    
    /**
     * 严判处理 - 处理CHTL JS和JS混合代码
     * @param code 代码
     * @param startPos 开始位置
     * @return 代码片段
     */
    CodeFragment strictJudge(const std::string& code, size_t startPos);
    
    /**
     * 处理局部样式块中的CHTL语法
     * @param code 代码
     * @param startPos 开始位置
     * @return 代码片段
     */
    CodeFragment processLocalStyleCHTL(const std::string& code, size_t startPos);
    
    /**
     * 处理局部脚本块中的CHTL语法
     * @param code 代码
     * @param startPos 开始位置
     * @return 代码片段
     */
    CodeFragment processLocalScriptCHTL(const std::string& code, size_t startPos);
};

} // namespace CHTL

#endif // UNIFIED_SCANNER_H