#ifndef CODE_MERGER_H
#define CODE_MERGER_H

#include <string>
#include <map>
#include <vector>

namespace CHTL {

// 编译结果
struct CompileResult {
    std::map<std::string, std::string> outputs; // 类型 -> 输出内容
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    std::map<std::string, std::string> metadata; // 元数据
};

// 最终结果
struct FinalResult {
    std::string html;
    std::string css;
    std::string javascript;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    std::map<std::string, std::string> metadata;
};

class CodeMerger {
private:
    // 配置
    bool debugMode;
    bool generateDefaultStructure;
    std::string outputFormat;
    
    // HTML模板
    std::string htmlTemplate;
    
    // 错误和警告
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    
    // 辅助方法
    void initializeHTMLTemplate();
    std::string generateHTML(const CompileResult& result);
    std::string generateCSS(const CompileResult& result);
    std::string generateJavaScript(const CompileResult& result);
    
    // HTML生成
    std::string generateHTML5Document(const std::string& body, const std::string& css, const std::string& js);
    std::string generateHTML4Document(const std::string& body, const std::string& css, const std::string& js);
    std::string generateXHTMLDocument(const std::string& body, const std::string& css, const std::string& js);
    
    // CSS处理
    std::string processCSS(const std::string& css);
    std::string minifyCSS(const std::string& css);
    std::string formatCSS(const std::string& css);
    
    // JavaScript处理
    std::string processJavaScript(const std::string& js);
    std::string minifyJavaScript(const std::string& js);
    std::string formatJavaScript(const std::string& js);
    
    // 代码优化
    std::string optimizeHTML(const std::string& html);
    std::string removeEmptyElements(const std::string& html);
    std::string compressWhitespace(const std::string& html);
    
    // 错误处理
    void addError(const std::string& error);
    void addWarning(const std::string& warning);
    
    // 验证
    bool validateHTML(const std::string& html);
    bool validateCSS(const std::string& css);
    bool validateJavaScript(const std::string& js);

public:
    CodeMerger();
    ~CodeMerger() = default;
    
    // 配置
    void setDebugMode(bool debug) { debugMode = debug; }
    void setGenerateDefaultStructure(bool generate) { generateDefaultStructure = generate; }
    void setOutputFormat(const std::string& format) { outputFormat = format; }
    
    // 主要合并方法
    FinalResult merge(const CompileResult& result);
    
    // 获取错误和警告
    const std::vector<std::string>& getErrors() const { return errors; }
    const std::vector<std::string>& getWarnings() const { return warnings; }
    
    // 清除错误和警告
    void clearMessages();
    
    // 验证结果
    bool validate(const FinalResult& result) const;
    
    // 调试信息
    std::string getDebugInfo() const;
};

} // namespace CHTL

#endif // CODE_MERGER_H