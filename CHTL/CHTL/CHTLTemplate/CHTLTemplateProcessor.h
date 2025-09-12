#ifndef CHTL_TEMPLATE_PROCESSOR_H
#define CHTL_TEMPLATE_PROCESSOR_H

#include "CHTLNode/TemplateNode.h"
#include "CHTLNode/ElementNode.h"
#include "CHTLNode/StyleNode.h"
#include "CHTLNode/BaseNode.h"
#include <string>
#include <map>
#include <vector>
#include <memory>
#include <unordered_map>
#include <unordered_set>

namespace CHTL {

class CHTLTemplateProcessor {
private:
    // 模板注册表
    std::unordered_map<std::string, std::shared_ptr<TemplateNode>> templateRegistry;
    std::unordered_map<std::string, std::vector<std::string>> templateDependencies;
    
    // 模板继承图
    std::unordered_map<std::string, std::vector<std::string>> inheritanceGraph;
    std::unordered_map<std::string, std::vector<std::string>> reverseInheritanceGraph;
    
    // 模板实例缓存
    std::unordered_map<std::string, std::shared_ptr<BaseNode>> instanceCache;
    
    // 配置
    bool debugMode;
    bool strictMode;
    bool enableCaching;
    bool enableInheritance;
    
    // 错误和警告
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    
    // 辅助方法
    void addError(const std::string& error);
    void addWarning(const std::string& warning);
    
    // 模板注册
    bool registerTemplate(std::shared_ptr<TemplateNode> templateNode);
    bool unregisterTemplate(const std::string& templateName);
    std::shared_ptr<TemplateNode> getTemplate(const std::string& templateName) const;
    
    // 模板继承处理
    void buildInheritanceGraph();
    void processInheritance(std::shared_ptr<TemplateNode> templateNode);
    void mergeParentTemplates(std::shared_ptr<TemplateNode> templateNode);
    void resolveInheritanceConflicts(std::shared_ptr<TemplateNode> templateNode);
    
    // 模板组合
    std::shared_ptr<TemplateNode> combineTemplates(const std::vector<std::string>& templateNames) const;
    std::shared_ptr<TemplateNode> mergeTemplates(const std::vector<std::string>& templateNames) const;
    
    // 模板实例化
    std::shared_ptr<BaseNode> instantiateTemplate(const std::string& templateName, 
                                                  const std::map<std::string, std::string>& args = {}) const;
    std::shared_ptr<BaseNode> instantiateStyleTemplate(std::shared_ptr<TemplateNode> templateNode,
                                                       const std::map<std::string, std::string>& args = {}) const;
    std::shared_ptr<BaseNode> instantiateElementTemplate(std::shared_ptr<TemplateNode> templateNode,
                                                         const std::map<std::string, std::string>& args = {}) const;
    std::shared_ptr<BaseNode> instantiateVariableTemplate(std::shared_ptr<TemplateNode> templateNode,
                                                          const std::map<std::string, std::string>& args = {}) const;
    
    // 模板验证
    bool validateTemplate(std::shared_ptr<TemplateNode> templateNode) const;
    bool validateTemplateDependencies(const std::string& templateName) const;
    bool validateInheritanceChain(const std::string& templateName) const;
    
    // 模板解析
    std::string parseTemplateString(const std::string& templateStr, 
                                   const std::map<std::string, std::string>& args = {}) const;
    std::vector<std::string> extractPlaceholders(const std::string& templateStr) const;
    std::string replacePlaceholders(const std::string& templateStr, 
                                   const std::map<std::string, std::string>& replacements) const;
    
    // 模板优化
    void optimizeTemplate(std::shared_ptr<TemplateNode> templateNode);
    void removeUnusedParameters(std::shared_ptr<TemplateNode> templateNode);
    void mergeDuplicateContent(std::shared_ptr<TemplateNode> templateNode);
    
    // 缓存管理
    void clearCache();
    void clearTemplateCache(const std::string& templateName);
    bool isCached(const std::string& templateName, const std::map<std::string, std::string>& args) const;
    std::string getCacheKey(const std::string& templateName, const std::map<std::string, std::string>& args) const;
    
    // 依赖分析
    std::vector<std::string> getTemplateDependencies(const std::string& templateName) const;
    std::vector<std::string> getDependentTemplates(const std::string& templateName) const;
    bool hasCircularDependency(const std::string& templateName) const;
    
    // 模板搜索
    std::vector<std::string> searchTemplates(const std::string& pattern) const;
    std::vector<std::string> searchTemplatesByType(TemplateNode::TemplateType type) const;
    std::vector<std::string> searchTemplatesByTag(const std::string& tag) const;

public:
    CHTLTemplateProcessor();
    ~CHTLTemplateProcessor() = default;
    
    // 配置
    void setDebugMode(bool debug) { debugMode = debug; }
    void setStrictMode(bool strict) { strictMode = strict; }
    void setEnableCaching(bool enable) { enableCaching = enable; }
    void setEnableInheritance(bool enable) { enableInheritance = enable; }
    
    // 模板管理
    bool addTemplate(std::shared_ptr<TemplateNode> templateNode);
    bool removeTemplate(const std::string& templateName);
    bool updateTemplate(std::shared_ptr<TemplateNode> templateNode);
    std::shared_ptr<TemplateNode> getTemplate(const std::string& templateName) const;
    
    // 模板实例化
    std::shared_ptr<BaseNode> instantiate(const std::string& templateName, 
                                          const std::map<std::string, std::string>& args = {});
    std::shared_ptr<BaseNode> instantiateStyleTemplate(const std::string& templateName, 
                                                       const std::map<std::string, std::string>& args = {});
    std::shared_ptr<BaseNode> instantiateElementTemplate(const std::string& templateName, 
                                                         const std::map<std::string, std::string>& args = {});
    std::shared_ptr<BaseNode> instantiateVariableTemplate(const std::string& templateName, 
                                                          const std::map<std::string, std::string>& args = {});
    
    // 模板组合
    std::shared_ptr<TemplateNode> combine(const std::vector<std::string>& templateNames) const;
    std::shared_ptr<TemplateNode> merge(const std::vector<std::string>& templateNames) const;
    
    // 模板继承
    bool addInheritance(const std::string& childTemplate, const std::string& parentTemplate);
    bool removeInheritance(const std::string& childTemplate, const std::string& parentTemplate);
    std::vector<std::string> getParentTemplates(const std::string& templateName) const;
    std::vector<std::string> getChildTemplates(const std::string& templateName) const;
    
    // 模板验证
    bool validateAllTemplates() const;
    bool validateTemplate(const std::string& templateName) const;
    bool validateInheritance() const;
    
    // 模板搜索
    std::vector<std::string> searchTemplates(const std::string& pattern) const;
    std::vector<std::string> searchTemplatesByType(TemplateNode::TemplateType type) const;
    std::vector<std::string> searchTemplatesByTag(const std::string& tag) const;
    
    // 模板优化
    void optimizeAllTemplates();
    void optimizeTemplate(const std::string& templateName);
    
    // 模板导出
    std::string exportTemplate(const std::string& templateName) const;
    std::string exportAllTemplates() const;
    bool importTemplate(const std::string& templateData);
    bool importTemplates(const std::string& templatesData);
    
    // 获取信息
    std::vector<std::string> getAllTemplateNames() const;
    std::vector<std::string> getTemplateNamesByType(TemplateNode::TemplateType type) const;
    size_t getTemplateCount() const;
    size_t getTemplateCountByType(TemplateNode::TemplateType type) const;
    
    // 获取错误和警告
    const std::vector<std::string>& getErrors() const { return errors; }
    const std::vector<std::string>& getWarnings() const { return warnings; }
    
    // 清除错误和警告
    void clearMessages();
    
    // 重置处理器
    void reset();
    
    // 调试信息
    std::string getDebugInfo() const;
    std::string getTemplateInfo(const std::string& templateName) const;
    std::string getInheritanceGraph() const;
    std::string getDependencyGraph() const;
};

} // namespace CHTL

#endif // CHTL_TEMPLATE_PROCESSOR_H