#pragma once

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace CHTL {

/**
 * @brief 全局映射表
 * 
 * 管理 CHTL 编译过程中的全局状态和映射关系
 */
class GlobalMap {
public:
    // 单例模式
    static GlobalMap& getInstance();
    
    // 禁止拷贝和赋值
    GlobalMap(const GlobalMap&) = delete;
    GlobalMap& operator=(const GlobalMap&) = delete;
    
    // 模板管理
    void addTemplate(const std::string& name, const std::string& type, const std::string& content);
    bool hasTemplate(const std::string& name) const;
    std::string getTemplate(const std::string& name) const;
    std::vector<std::string> getAllTemplates() const;
    
    // 自定义管理
    void addCustom(const std::string& name, const std::string& type, const std::string& content);
    bool hasCustom(const std::string& name) const;
    std::string getCustom(const std::string& name) const;
    std::vector<std::string> getAllCustoms() const;
    
    // 变量管理
    void addVariable(const std::string& name, const std::string& value);
    bool hasVariable(const std::string& name) const;
    std::string getVariable(const std::string& name) const;
    std::vector<std::string> getAllVariables() const;
    
    // 命名空间管理
    void addNamespace(const std::string& name, const std::string& parent = "");
    bool hasNamespace(const std::string& name) const;
    std::string getNamespaceParent(const std::string& name) const;
    std::vector<std::string> getNamespaceChildren(const std::string& name) const;
    
    // 配置管理
    void setConfig(const std::string& key, const std::string& value);
    std::string getConfig(const std::string& key, const std::string& defaultValue = "") const;
    bool hasConfig(const std::string& key) const;
    
    // 导入管理
    void addImport(const std::string& name, const std::string& path, const std::string& type);
    bool hasImport(const std::string& name) const;
    std::string getImportPath(const std::string& name) const;
    std::string getImportType(const std::string& name) const;
    
    // 约束管理
    void addConstraint(const std::string& scope, const std::string& constraint);
    std::vector<std::string> getConstraints(const std::string& scope) const;
    bool hasConstraint(const std::string& scope, const std::string& constraint) const;
    
    // 清理所有数据
    void clear();
    
    // 重置到初始状态
    void reset();
    
private:
    GlobalMap() = default;
    ~GlobalMap() = default;
    
    // 模板存储：name -> {type, content}
    std::unordered_map<std::string, std::pair<std::string, std::string>> templates_;
    
    // 自定义存储：name -> {type, content}
    std::unordered_map<std::string, std::pair<std::string, std::string>> customs_;
    
    // 变量存储：name -> value
    std::unordered_map<std::string, std::string> variables_;
    
    // 命名空间存储：name -> parent
    std::unordered_map<std::string, std::string> namespaces_;
    
    // 配置存储：key -> value
    std::unordered_map<std::string, std::string> configs_;
    
    // 导入存储：name -> {path, type}
    std::unordered_map<std::string, std::pair<std::string, std::string>> imports_;
    
    // 约束存储：scope -> constraints
    std::unordered_map<std::string, std::vector<std::string>> constraints_;
};

} // namespace CHTL