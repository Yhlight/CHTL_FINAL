#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>

namespace CHTL {

/**
 * 默认HTML结构类型
 */
enum class DefaultStructType {
    MINIMAL,        // 最小结构
    STANDARD,       // 标准HTML5结构
    SPA,           // SPA应用结构
    DOCUMENT,      // 文档结构
    APPLICATION    // 应用结构
};

/**
 * 默认结构配置
 */
struct DefaultStructConfig {
    DefaultStructType type;
    std::string title;
    std::string charset;
    std::string viewport;
    std::vector<std::string> meta_tags;
    std::vector<std::string> css_links;
    std::vector<std::string> js_scripts;
    std::string body_class;
    std::string html_lang;
    bool include_doctype;
    bool include_html_tag;
    bool include_head_tag;
    bool include_body_tag;
    
    DefaultStructConfig() 
        : type(DefaultStructType::STANDARD)
        , title("CHTL Document")
        , charset("UTF-8")
        , viewport("width=device-width, initial-scale=1.0")
        , body_class("")
        , html_lang("en")
        , include_doctype(true)
        , include_html_tag(true)
        , include_head_tag(true)
        , include_body_tag(true) {
    }
};

/**
 * 默认结构生成器
 */
class DefaultStructGenerator {
public:
    DefaultStructGenerator();
    ~DefaultStructGenerator();
    
    // 生成默认结构
    std::string generateDefaultStruct(const std::string& content, const DefaultStructConfig& config) const;
    std::string generateMinimalStruct(const std::string& content) const;
    std::string generateStandardStruct(const std::string& content) const;
    std::string generateSPAStruct(const std::string& content) const;
    std::string generateDocumentStruct(const std::string& content) const;
    std::string generateApplicationStruct(const std::string& content) const;
    
    // 结构检测
    bool hasHTMLStructure(const std::string& content) const;
    bool hasDoctype(const std::string& content) const;
    bool hasHTMLTag(const std::string& content) const;
    bool hasHeadTag(const std::string& content) const;
    bool hasBodyTag(const std::string& content) const;
    
    // 内容提取
    std::string extractBodyContent(const std::string& content) const;
    std::string extractHeadContent(const std::string& content) const;
    std::string extractTitle(const std::string& content) const;
    std::vector<std::string> extractMetaTags(const std::string& content) const;
    std::vector<std::string> extractCSSLinks(const std::string& content) const;
    std::vector<std::string> extractJSScripts(const std::string& content) const;
    
    // 结构合并
    std::string mergeWithDefaultStruct(const std::string& content, const DefaultStructConfig& config) const;
    std::string wrapInDefaultStruct(const std::string& content, const DefaultStructConfig& config) const;
    
    // 配置管理
    void setDefaultConfig(const DefaultStructConfig& config);
    DefaultStructConfig getDefaultConfig() const;
    
    // 预设配置
    DefaultStructConfig getMinimalConfig() const;
    DefaultStructConfig getStandardConfig() const;
    DefaultStructConfig getSPAConfig() const;
    DefaultStructConfig getDocumentConfig() const;
    DefaultStructConfig getApplicationConfig() const;
    
private:
    DefaultStructConfig default_config_;
    
    // 辅助函数
    std::string generateDoctype() const;
    std::string generateHTMLTag(const DefaultStructConfig& config) const;
    std::string generateHeadTag(const DefaultStructConfig& config) const;
    std::string generateBodyTag(const DefaultStructConfig& config) const;
    std::string generateMetaTags(const DefaultStructConfig& config) const;
    std::string generateCSSLinks(const DefaultStructConfig& config) const;
    std::string generateJSScripts(const DefaultStructConfig& config) const;
    
    // 内容分析
    std::string findTagContent(const std::string& content, const std::string& tagName) const;
    std::vector<std::string> findAllTagContent(const std::string& content, const std::string& tagName) const;
    bool containsTag(const std::string& content, const std::string& tagName) const;
    std::string cleanContent(const std::string& content) const;
    
    // 模板处理
    std::string processTemplate(const std::string& template_str, const std::map<std::string, std::string>& variables) const;
    std::string replacePlaceholders(const std::string& str, const std::map<std::string, std::string>& replacements) const;
};

/**
 * 默认结构管理器
 */
class DefaultStructManager {
public:
    DefaultStructManager();
    ~DefaultStructManager();
    
    // 结构管理
    void enableDefaultStruct(bool enabled);
    bool isDefaultStructEnabled() const;
    
    void setStructType(DefaultStructType type);
    DefaultStructType getStructType() const;
    
    void setConfig(const DefaultStructConfig& config);
    DefaultStructConfig getConfig() const;
    
    // 内容处理
    std::string processContent(const std::string& content) const;
    std::string processContentWithConfig(const std::string& content, const DefaultStructConfig& config) const;
    
    // 预设管理
    void loadPreset(const std::string& presetName);
    std::vector<std::string> getAvailablePresets() const;
    
    // 验证
    bool validateConfig(const DefaultStructConfig& config) const;
    std::vector<std::string> getConfigValidationErrors(const DefaultStructConfig& config) const;
    
private:
    bool default_struct_enabled_;
    DefaultStructType struct_type_;
    DefaultStructConfig config_;
    DefaultStructGenerator generator_;
    
    // 预设配置
    std::map<std::string, DefaultStructConfig> presets_;
    
    // 辅助函数
    void initializePresets();
    DefaultStructConfig createPresetConfig(const std::string& presetName) const;
};

/**
 * 编译器选项处理器
 */
class CompilerOptionProcessor {
public:
    CompilerOptionProcessor();
    ~CompilerOptionProcessor();
    
    // 选项解析
    bool parseDefaultStructOption(const std::string& option) const;
    DefaultStructConfig parseDefaultStructConfig(const std::string& option) const;
    
    // 选项验证
    bool isValidDefaultStructOption(const std::string& option) const;
    std::vector<std::string> getOptionValidationErrors(const std::string& option) const;
    
    // 选项处理
    std::string processDefaultStructOption(const std::string& content, const std::string& option) const;
    std::string processDefaultStructOptionWithConfig(const std::string& content, const DefaultStructConfig& config) const;
    
    // 选项信息
    std::string getDefaultStructOptionHelp() const;
    std::vector<std::string> getDefaultStructOptionExamples() const;
    
private:
    // 辅助函数
    DefaultStructType parseStructType(const std::string& typeStr) const;
    std::map<std::string, std::string> parseOptionParameters(const std::string& option) const;
    bool isStructTypeValid(const std::string& typeStr) const;
    std::vector<std::string> getValidStructTypes() const;
};

} // namespace CHTL