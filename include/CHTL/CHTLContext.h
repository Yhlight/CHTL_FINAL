#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <map>

namespace CHTL {

// 前向声明
class CHTLNode;
class CHTLLexer;
class CHTLParser;
class CHTLGenerator;

/**
 * CHTL编译器上下文
 * 管理编译过程中的全局状态和配置
 */
class CHTLContext {
public:
    CHTLContext();
    ~CHTLContext();

    // 配置管理
    void setConfiguration(const std::string& key, const std::string& value);
    std::string getConfiguration(const std::string& key) const;
    
    // 命名空间管理
    void addNamespace(const std::string& name, const std::string& parent = "");
    bool hasNamespace(const std::string& name) const;
    std::string getFullNamespace(const std::string& name) const;
    
    // 模板管理
    void addTemplate(const std::string& type, const std::string& name, std::shared_ptr<CHTLNode> node);
    std::shared_ptr<CHTLNode> getTemplate(const std::string& type, const std::string& name) const;
    
    // 自定义管理
    void addCustom(const std::string& type, const std::string& name, std::shared_ptr<CHTLNode> node);
    std::shared_ptr<CHTLNode> getCustom(const std::string& type, const std::string& name) const;
    
    // 导入管理
    void addImport(const std::string& path, const std::string& alias = "");
    std::string getImportPath(const std::string& alias) const;
    
    // 约束管理
    void addConstraint(const std::string& element, const std::vector<std::string>& constraints);
    bool isConstraintViolated(const std::string& element, const std::string& constraint) const;
    
    // 调试模式
    void setDebugMode(bool enabled);
    bool isDebugMode() const;
    
    // 清理
    void clear();

private:
    // 配置存储
    std::unordered_map<std::string, std::string> configurations_;
    
    // 命名空间树
    std::map<std::string, std::string> namespaces_;
    
    // 模板存储
    std::unordered_map<std::string, std::unordered_map<std::string, std::shared_ptr<CHTLNode>>> templates_;
    
    // 自定义存储
    std::unordered_map<std::string, std::unordered_map<std::string, std::shared_ptr<CHTLNode>>> customs_;
    
    // 导入映射
    std::unordered_map<std::string, std::string> imports_;
    
    // 约束存储
    std::unordered_map<std::string, std::vector<std::string>> constraints_;
    
    // 调试模式
    bool debug_mode_;
};

} // namespace CHTL