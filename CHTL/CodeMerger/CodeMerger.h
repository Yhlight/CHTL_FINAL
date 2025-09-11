#ifndef CODE_MERGER_H
#define CODE_MERGER_H

#include <string>
#include <map>
#include <vector>
#include "Common/CommonTypes.h"

namespace CHTL {

class CodeMerger {
private:
    // 配置
    bool debugMode;
    bool generateDefaultStructure;
    std::string outputFormat;
    bool minifyOutput;
    bool beautifyOutput;
    bool optimizeOutput;
    std::string title;
    std::string language;
    std::map<std::string, std::string> metaTags;
    std::vector<std::string> externalCSS;
    std::vector<std::string> externalJS;
    
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
    std::string minifyHTML(const std::string& html);
    std::string beautifyHTML(const std::string& html);
    
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
    void setMinifyOutput(bool minify) { minifyOutput = minify; }
    void setBeautifyOutput(bool beautify) { beautifyOutput = beautify; }
    void setOptimizeOutput(bool optimize) { optimizeOutput = optimize; }
    void setTitle(const std::string& docTitle) { title = docTitle; }
    void setLanguage(const std::string& lang) { language = lang; }
    void addMetaTag(const std::string& name, const std::string& content);
    void addExternalCSS(const std::string& cssPath);
    void addExternalJS(const std::string& jsPath);
    
    // 获取配置
    bool getDebugMode() const { return debugMode; }
    bool getGenerateDefaultStructure() const { return generateDefaultStructure; }
    std::string getOutputFormat() const { return outputFormat; }
    bool getMinifyOutput() const { return minifyOutput; }
    bool getBeautifyOutput() const { return beautifyOutput; }
    bool getOptimizeOutput() const { return optimizeOutput; }
    std::string getTitle() const { return title; }
    std::string getLanguage() const { return language; }
    
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
    
    // 高级功能
    std::string generateInlineCSS(const std::string& css);
    std::string generateInlineJS(const std::string& js);
    std::string generateExternalCSSLinks();
    std::string generateExternalJSLinks();
    std::string generateMetaTags();
    
    // 代码分析
    std::map<std::string, int> analyzeHTML(const std::string& html);
    std::map<std::string, int> analyzeCSS(const std::string& css);
    std::map<std::string, int> analyzeJavaScript(const std::string& js);
    
    // 代码转换
    std::string convertToInline(const FinalResult& result);
    std::string convertToExternal(const FinalResult& result);
    std::string convertToMinified(const FinalResult& result);
    std::string convertToBeautified(const FinalResult& result);
    
    // 模板管理
    void setHTMLTemplate(const std::string& templateStr);
    std::string getHTMLTemplate() const;
    void resetHTMLTemplate();
    
    // 清理功能
    void clearMetaTags();
    void clearExternalCSS();
    void clearExternalJS();
    void clearAll();
};

} // namespace CHTL

#endif // CODE_MERGER_H