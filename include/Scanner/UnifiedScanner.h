#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <map>

namespace CHTL {

/**
 * 统一扫描器
 * 负责分离CHTL、CHTL JS、CSS、JS代码片段
 */
class UnifiedScanner {
public:
    enum class CodeType {
        CHTL,           // CHTL代码
        CHTL_JS,        // CHTL JS代码
        CSS,            // CSS代码
        JS,             // JavaScript代码
        UNKNOWN         // 未知类型
    };

    struct CodeFragment {
        CodeType type;
        std::string content;
        size_t start_pos;
        size_t end_pos;
        std::string placeholder;  // 占位符
    };

    UnifiedScanner();
    ~UnifiedScanner();

    // 扫描代码，分离不同类型的代码片段
    std::vector<CodeFragment> scan(const std::string& code);
    
    // 恢复占位符
    std::string restorePlaceholders(const std::string& code, 
                                   const std::vector<CodeFragment>& fragments);
    
    // 设置配置
    void setConfiguration(const std::string& key, const std::string& value);
    
    // 启用/禁用调试模式
    void setDebugMode(bool enabled);

private:
    // 扫描CHTL代码
    std::vector<CodeFragment> scanCHTL(const std::string& code, size_t start_pos);
    
    // 扫描CHTL JS代码
    std::vector<CodeFragment> scanCHTLJS(const std::string& code, size_t start_pos);
    
    // 扫描CSS代码
    std::vector<CodeFragment> scanCSS(const std::string& code, size_t start_pos);
    
    // 扫描JS代码
    std::vector<CodeFragment> scanJS(const std::string& code, size_t start_pos);
    
    // 查找语法边界
    size_t findSyntaxBoundary(const std::string& code, size_t start_pos, 
                             const std::string& open, const std::string& close);
    
    // 查找CHTL语法边界
    size_t findCHTLBoundary(const std::string& code, size_t start_pos);
    
    // 查找CHTL JS语法边界
    size_t findCHTLJSBoundary(const std::string& code, size_t start_pos);
    
    // 查找CSS语法边界
    size_t findCSSBoundary(const std::string& code, size_t start_pos);
    
    // 查找JS语法边界
    size_t findJSBoundary(const std::string& code, size_t start_pos);
    
    // 生成占位符
    std::string generatePlaceholder(CodeType type, size_t index);
    
    // 验证代码完整性
    bool validateCodeFragment(const std::string& code, CodeType type);
    
    // 处理嵌套结构
    std::string processNestedStructures(const std::string& code, CodeType type);
    
    // 处理占位符替换
    std::string replaceWithPlaceholders(const std::string& code, 
                                       const std::vector<CodeFragment>& fragments);
    
    // 配置
    std::unordered_map<std::string, std::string> configurations_;
    bool debug_mode_;
    
    // 占位符计数器
    std::map<CodeType, size_t> placeholder_counters_;
    
    // 占位符映射
    std::unordered_map<std::string, CodeFragment> placeholder_map_;
};

} // namespace CHTL