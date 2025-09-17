#ifndef CHTLJSCONTEXT_H
#define CHTLJSCONTEXT_H

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>

namespace CHTLJS {

/**
 * CHTL JS上下文管理
 * 管理CHTL JS编译时的全局状态和配置
 */
class CHTLJSContext {
public:
    CHTLJSContext();
    ~CHTLJSContext();

    // 变量管理
    void setVariable(const std::string& name, const std::string& value);
    std::string getVariable(const std::string& name) const;
    bool hasVariable(const std::string& name) const;
    void removeVariable(const std::string& name);
    
    // 响应式变量管理
    void setReactiveVariable(const std::string& name, const std::string& value);
    std::string getReactiveVariable(const std::string& name) const;
    bool hasReactiveVariable(const std::string& name) const;
    
    // 虚拟对象管理
    void registerVirtualObject(const std::string& name, std::function<std::string()> generator);
    std::string generateVirtualObject(const std::string& name) const;
    bool hasVirtualObject(const std::string& name) const;
    
    // 事件监听器管理
    void addEventListener(const std::string& selector, const std::string& event, const std::string& handler);
    std::vector<std::string> getEventListeners(const std::string& selector) const;
    
    // 动画管理
    void addAnimation(const std::string& name, const std::string& animation);
    std::string getAnimation(const std::string& name) const;
    bool hasAnimation(const std::string& name) const;
    
    // 路由管理
    void addRoute(const std::string& path, const std::string& handler);
    std::string getRouteHandler(const std::string& path) const;
    bool hasRoute(const std::string& path) const;
    
    // 模块管理
    void addModule(const std::string& name, const std::string& content);
    std::string getModule(const std::string& name) const;
    bool hasModule(const std::string& name) const;
    
    // 配置管理
    void setConfiguration(const std::string& key, const std::string& value);
    std::string getConfiguration(const std::string& key) const;
    bool hasConfiguration(const std::string& key) const;
    
    // 编译选项
    void setCompileOption(const std::string& option, bool value);
    bool getCompileOption(const std::string& option) const;
    
    // 清理
    void clear();

private:
    // 变量存储
    std::map<std::string, std::string> variables_;
    std::map<std::string, std::string> reactive_variables_;
    
    // 虚拟对象存储
    std::map<std::string, std::function<std::string()>> virtual_objects_;
    
    // 事件系统
    std::map<std::string, std::vector<std::string>> event_listeners_;
    
    // 动画系统
    std::map<std::string, std::string> animations_;
    
    // 路由系统
    std::map<std::string, std::string> routes_;
    
    // 模块系统
    std::map<std::string, std::string> modules_;
    
    // 配置
    std::map<std::string, std::string> configurations_;
    std::map<std::string, bool> compile_options_;
};

} // namespace CHTLJS

#endif // CHTLJSCONTEXT_H