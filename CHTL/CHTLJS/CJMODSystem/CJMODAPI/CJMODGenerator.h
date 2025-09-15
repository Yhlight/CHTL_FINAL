#ifndef CJMOD_GENERATOR_H
#define CJMOD_GENERATOR_H

#include "Arg.h"
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <functional>

namespace CHTL {
namespace CJMOD {

enum class GenerationMode {
    JAVASCRIPT,     // 生成JavaScript代码
    CHTLJS,         // 生成CHTL JS代码
    HTML,           // 生成HTML代码
    CSS,            // 生成CSS代码
    MIXED           // 生成混合代码
};

struct GenerationOptions {
    GenerationMode mode;
    bool minify;
    bool includeComments;
    bool preserveWhitespace;
    std::string indentString;
    int indentLevel;
    std::unordered_map<std::string, std::string> customReplacements;
    
    GenerationOptions() : mode(GenerationMode::JAVASCRIPT), minify(false), 
                         includeComments(true), preserveWhitespace(true),
                         indentString("  "), indentLevel(0) {}
};

class CJMODGenerator {
private:
    GenerationOptions options;
    std::unordered_map<std::string, std::string> generatedCode;
    std::vector<std::string> generationHistory;
    
    // 代码生成方法
    std::string generateJavaScript(const Arg& args);
    std::string generateCHTLJS(const Arg& args);
    std::string generateHTML(const Arg& args);
    std::string generateCSS(const Arg& args);
    std::string generateMixed(const Arg& args);
    
    // 模板处理
    std::string processTemplate(const std::string& template, const Arg& args);
    std::string replacePlaceholders(const std::string& template, const Arg& args);
    std::string applyCustomReplacements(const std::string& code);
    
    // 代码格式化
    std::string formatCode(const std::string& code);
    std::string minifyCode(const std::string& code);
    std::string beautifyCode(const std::string& code);
    
    // 代码验证
    bool validateGeneratedCode(const std::string& code);
    std::vector<std::string> getValidationErrors(const std::string& code);
    
    // 辅助方法
    std::string indent(const std::string& code, int level);
    std::string addComments(const std::string& code);
    std::string removeComments(const std::string& code);
    
public:
    CJMODGenerator();
    CJMODGenerator(const GenerationOptions& options);
    ~CJMODGenerator() = default;
    
    // 主要生成方法
    static std::string exportResult(const Arg& args);
    static std::string exportResult(const Arg& args, const GenerationOptions& options);
    std::string generate(const Arg& args);
    std::string generate(const Arg& args, const GenerationOptions& options);
    
    // 批量生成
    std::vector<std::string> generateBatch(const std::vector<Arg>& argsList);
    std::unordered_map<std::string, std::string> generateMap(const std::unordered_map<std::string, Arg>& argsMap);
    
    // 代码导出
    bool exportToFile(const std::string& filename, const Arg& args);
    bool exportToFile(const std::string& filename, const std::string& code);
    std::string exportToString(const Arg& args);
    
    // 配置管理
    void setOptions(const GenerationOptions& options);
    GenerationOptions getOptions() const;
    void setMode(GenerationMode mode);
    void setMinify(bool minify);
    void setIncludeComments(bool include);
    void setPreserveWhitespace(bool preserve);
    void setIndentString(const std::string& indent);
    void setIndentLevel(int level);
    
    // 自定义替换
    void addCustomReplacement(const std::string& placeholder, const std::string& replacement);
    void removeCustomReplacement(const std::string& placeholder);
    void clearCustomReplacements();
    std::unordered_map<std::string, std::string> getCustomReplacements() const;
    
    // 生成历史
    std::vector<std::string> getGenerationHistory() const;
    void clearHistory();
    std::string getLastGenerated() const;
    
    // 代码缓存
    void cacheGeneratedCode(const std::string& key, const std::string& code);
    std::string getCachedCode(const std::string& key) const;
    bool hasCachedCode(const std::string& key) const;
    void clearCache();
    std::unordered_map<std::string, std::string> getAllCachedCode() const;
    
    // 模板管理
    void registerTemplate(const std::string& name, const std::string& template);
    void unregisterTemplate(const std::string& name);
    bool hasTemplate(const std::string& name) const;
    std::string getTemplate(const std::string& name) const;
    std::unordered_map<std::string, std::string> getAllTemplates() const;
    
    // 验证和调试
    bool validateArg(const Arg& args) const;
    std::vector<std::string> getValidationErrors(const Arg& args) const;
    void printGenerationStatistics() const;
    void printCacheStatistics() const;
    void printTemplateStatistics() const;
    
    // 工具方法
    void reset();
    void clear();
    
private:
    std::unordered_map<std::string, std::string> templates;
};

} // namespace CJMOD
} // namespace CHTL

#endif // CJMOD_GENERATOR_H