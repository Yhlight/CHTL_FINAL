#ifndef CHTL_CHTHOLLY_MODULE_H
#define CHTL_CHTHOLLY_MODULE_H

#include <string>
#include <vector>
#include <map>
#include <memory>
#include "../CHTL/CMODSystem/CMODModule.h"
#include "../CHTL_JS/CJMODSystem/CJMODModule.h"

namespace CHTL {
namespace Official {

// Chtholly模块 - 官方核心模块
class ChthollyModule : public CMODModule {
public:
    ChthollyModule();
    ~ChthollyModule() = default;
    
    // 模块信息
    std::string getModuleName() const override;
    std::string getVersion() const override;
    std::string getDescription() const override;
    std::string getAuthor() const override;
    
    // 模块功能
    bool initialize() override;
    bool isInitialized() const override;
    void cleanup() override;
    
    // Chtholly特有功能
    std::string generateHTML(const std::string& content) const;
    std::string generateCSS(const std::string& content) const;
    std::string generateJS(const std::string& content) const;
    
    // 子模块管理
    void addSubModule(std::shared_ptr<CMODModule> subModule);
    std::vector<std::shared_ptr<CMODModule>> getSubModules() const;
    std::shared_ptr<CMODModule> getSubModule(const std::string& name) const;
    
    // 配置管理
    void setConfiguration(const std::string& key, const std::string& value);
    std::string getConfiguration(const std::string& key) const;
    std::map<std::string, std::string> getAllConfigurations() const;
    
private:
    bool initialized;
    std::map<std::string, std::string> configurations;
    std::vector<std::shared_ptr<CMODModule>> subModules;
    
    void initializeDefaultConfigurations();
    void initializeSubModules();
};

// Chtholly核心子模块
class ChthollyCoreModule : public CMODModule {
public:
    ChthollyCoreModule();
    ~ChthollyCoreModule() = default;
    
    std::string getModuleName() const override;
    std::string getVersion() const override;
    std::string getDescription() const override;
    std::string getAuthor() const override;
    
    bool initialize() override;
    bool isInitialized() const override;
    void cleanup() override;
    
    // 核心功能
    std::string processElement(const std::string& element) const;
    std::string processAttribute(const std::string& attribute) const;
    std::string processText(const std::string& text) const;
    
private:
    bool initialized;
};

// Chtholly样式子模块
class ChthollyStyleModule : public CMODModule {
public:
    ChthollyStyleModule();
    ~ChthollyStyleModule() = default;
    
    std::string getModuleName() const override;
    std::string getVersion() const override;
    std::string getDescription() const override;
    std::string getAuthor() const override;
    
    bool initialize() override;
    bool isInitialized() const override;
    void cleanup() override;
    
    // 样式功能
    std::string processStyle(const std::string& style) const;
    std::string generateClass(const std::string& className) const;
    std::string generateId(const std::string& idName) const;
    
private:
    bool initialized;
};

// Chtholly脚本子模块
class ChthollyScriptModule : public CMODModule {
public:
    ChthollyScriptModule();
    ~ChthollyScriptModule() = default;
    
    std::string getModuleName() const override;
    std::string getVersion() const override;
    std::string getDescription() const override;
    std::string getAuthor() const override;
    
    bool initialize() override;
    bool isInitialized() const override;
    void cleanup() override;
    
    // 脚本功能
    std::string processScript(const std::string& script) const;
    std::string generateEvent(const std::string& eventName) const;
    std::string generateFunction(const std::string& functionName) const;
    
private:
    bool initialized;
};

// Chtholly模板子模块
class ChthollyTemplateModule : public CMODModule {
public:
    ChthollyTemplateModule();
    ~ChthollyTemplateModule() = default;
    
    std::string getModuleName() const override;
    std::string getVersion() const override;
    std::string getDescription() const override;
    std::string getAuthor() const override;
    
    bool initialize() override;
    bool isInitialized() const override;
    void cleanup() override;
    
    // 模板功能
    std::string processTemplate(const std::string& template) const;
    std::string generateTemplate(const std::string& templateName) const;
    std::string processVariable(const std::string& variable) const;
    
private:
    bool initialized;
};

} // namespace Official
} // namespace CHTL

#endif // CHTL_CHTHOLLY_MODULE_H