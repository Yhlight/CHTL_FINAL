#ifndef CHTL_JS_CONTEXT_H
#define CHTL_JS_CONTEXT_H

#include <string>
#include <unordered_map>
#include <vector>
#include <memory>
#include <functional>

namespace CHTL {

class CHTLJSContext {
public:
    CHTLJSContext();
    ~CHTLJSContext() = default;
    
    // 虚对象管理
    void registerVirtualObject(const std::string& name, const std::string& content);
    std::string getVirtualObject(const std::string& name) const;
    bool hasVirtualObject(const std::string& name) const;
    
    // 函数管理
    void registerFunction(const std::string& name, const std::string& content);
    std::string getFunction(const std::string& name) const;
    bool hasFunction(const std::string& name) const;
    
    // 事件委托管理
    void registerEventDelegate(const std::string& parent, const std::string& target, const std::string& event, const std::string& handler);
    std::vector<std::string> getEventDelegates(const std::string& parent) const;
    
    // 路由管理
    void registerRoute(const std::string& url, const std::string& page);
    std::string getRoute(const std::string& url) const;
    bool hasRoute(const std::string& url) const;
    
    // 动画管理
    void registerAnimation(const std::string& name, const std::string& content);
    std::string getAnimation(const std::string& name) const;
    bool hasAnimation(const std::string& name) const;
    
    // 错误处理
    void addError(const std::string& error);
    void addWarning(const std::string& warning);
    const std::vector<std::string>& getErrors() const { return errors_; }
    const std::vector<std::string>& getWarnings() const { return warnings_; }
    bool hasErrors() const { return !errors_.empty(); }
    bool hasWarnings() const { return !warnings_.empty(); }
    
    // 清理
    void clear();
    
private:
    std::unordered_map<std::string, std::string> virtualObjects_;
    std::unordered_map<std::string, std::string> functions_;
    std::unordered_map<std::string, std::vector<std::string>> eventDelegates_;
    std::unordered_map<std::string, std::string> routes_;
    std::unordered_map<std::string, std::string> animations_;
    std::vector<std::string> errors_;
    std::vector<std::string> warnings_;
};

} // namespace CHTL

#endif // CHTL_JS_CONTEXT_H