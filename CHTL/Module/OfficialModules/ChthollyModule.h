#ifndef CHTHOLLYMODULE_H
#define CHTHOLLYMODULE_H

#include "../CMODModule.h"
#include "../CJMODModule.h"
#include <string>
#include <vector>
#include <map>
#include <memory>

namespace CHTL {
namespace Module {

// Chtholly模块 - 官方核心模块
class ChthollyModule {
public:
    ChthollyModule();
    ~ChthollyModule();

    // 模块信息
    static const std::string MODULE_NAME;
    static const std::string MODULE_VERSION;
    static const std::string MODULE_DESCRIPTION;
    static const std::string MODULE_AUTHOR;
    static const std::string MODULE_LICENSE;
    
    // 核心功能
    std::string getName() const;
    std::string getVersion() const;
    std::string getDescription() const;
    std::string getAuthor() const;
    std::string getLicense() const;
    
    // 模块管理
    bool initialize();
    bool isInitialized() const;
    void cleanup();
    
    // 核心API
    std::string processCHTL(const std::string& chtlCode) const;
    std::string processCHTLJS(const std::string& chtljsCode) const;
    std::string processCSS(const std::string& cssCode) const;
    std::string processJavaScript(const std::string& jsCode) const;
    
    // 模板功能
    std::string createTemplate(const std::string& name, const std::string& content);
    std::string applyTemplate(const std::string& templateName, const std::map<std::string, std::string>& variables) const;
    bool hasTemplate(const std::string& name) const;
    std::vector<std::string> getTemplateNames() const;
    
    // 样式功能
    std::string createStyle(const std::string& selector, const std::map<std::string, std::string>& properties) const;
    std::string applyStyle(const std::string& element, const std::string& style) const;
    std::string generateCSS(const std::map<std::string, std::map<std::string, std::string>>& styles) const;
    
    // 脚本功能
    std::string createScript(const std::string& name, const std::string& code);
    std::string executeScript(const std::string& scriptName, const std::map<std::string, std::string>& parameters) const;
    bool hasScript(const std::string& name) const;
    std::vector<std::string> getScriptNames() const;
    
    // 组件功能
    std::string createComponent(const std::string& name, const std::string& templateContent, 
                               const std::string& style, const std::string& script);
    std::string renderComponent(const std::string& componentName, const std::map<std::string, std::string>& props) const;
    bool hasComponent(const std::string& name) const;
    std::vector<std::string> getComponentNames() const;
    
    // 工具功能
    std::string minifyCode(const std::string& code) const;
    std::string beautifyCode(const std::string& code) const;
    std::string validateCode(const std::string& code) const;
    std::map<std::string, std::string> analyzeCode(const std::string& code) const;
    
    // 配置功能
    void setConfiguration(const std::string& key, const std::string& value);
    std::string getConfiguration(const std::string& key) const;
    bool hasConfiguration(const std::string& key) const;
    std::map<std::string, std::string> getAllConfigurations() const;
    
    // 错误处理
    std::vector<std::string> getErrors() const;
    std::vector<std::string> getWarnings() const;
    void addError(const std::string& error) const;
    void addWarning(const std::string& warning) const;
    void clearErrors();
    void clearWarnings();
    
    // 统计信息
    size_t getTemplateCount() const;
    size_t getStyleCount() const;
    size_t getScriptCount() const;
    size_t getComponentCount() const;
    std::map<std::string, size_t> getStatistics() const;

private:
    bool initialized;
    std::map<std::string, std::string> templates;
    std::map<std::string, std::string> scripts;
    std::map<std::string, std::string> components;
    std::map<std::string, std::string> configurations;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    
    // 内部方法
    std::string processTemplateVariables(const std::string& templateContent, const std::map<std::string, std::string>& variables) const;
    std::string processStyleProperties(const std::map<std::string, std::string>& properties) const;
    std::string processScriptParameters(const std::string& script, const std::map<std::string, std::string>& parameters) const;
    std::string processComponentProps(const std::string& component, const std::map<std::string, std::string>& props) const;
    
    // 验证方法
    bool validateTemplate(const std::string& templateContent) const;
    bool validateStyle(const std::map<std::string, std::string>& properties) const;
    bool validateScript(const std::string& script) const;
    bool validateComponent(const std::string& component) const;
    
    // 工具方法
    std::string escapeString(const std::string& str) const;
    std::string unescapeString(const std::string& str) const;
    std::vector<std::string> splitString(const std::string& str, char delimiter) const;
    std::string joinString(const std::vector<std::string>& parts, const std::string& delimiter) const;
    std::string trimString(const std::string& str) const;
    bool startsWith(const std::string& str, const std::string& prefix) const;
    bool endsWith(const std::string& str, const std::string& suffix) const;
};

} // namespace Module
} // namespace CHTL

#endif // CHTHOLLYMODULE_H