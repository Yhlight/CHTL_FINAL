#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <map>
#include <functional>

namespace CHTL {

/**
 * CHTL JS编译器上下文
 * 管理CHTL JS编译过程中的状态和配置
 */
class CHTLJSContext {
public:
    // 虚对象类型
    struct VirtualObject {
        std::string name;
        std::unordered_map<std::string, std::string> properties;
        std::unordered_map<std::string, std::function<std::string(const std::string&)>> functions;
    };
    
    // 增强选择器类型
    struct EnhancedSelector {
        std::string selector;
        std::string dom_reference;
        bool is_auto_generated;
    };
    
    // 事件委托类型
    struct EventDelegate {
        std::string parent_selector;
        std::vector<std::string> target_selectors;
        std::unordered_map<std::string, std::string> event_handlers;
    };
    
    // 动画类型
    struct Animation {
        std::string target;
        int duration;
        std::string easing;
        std::unordered_map<std::string, std::string> begin_styles;
        std::vector<std::map<std::string, std::string>> when_styles;
        std::unordered_map<std::string, std::string> end_styles;
        int loop_count;
        std::string direction;
        int delay;
        std::string callback;
    };
    
    // 路由类型
    struct Route {
        std::vector<std::string> urls;
        std::vector<std::string> pages;
        std::string root;
        std::string mode;
    };

    CHTLJSContext();
    ~CHTLJSContext();

    // 虚对象管理
    void addVirtualObject(const std::string& name, const VirtualObject& obj);
    VirtualObject* getVirtualObject(const std::string& name);
    bool hasVirtualObject(const std::string& name) const;
    
    // 增强选择器管理
    void addEnhancedSelector(const std::string& name, const EnhancedSelector& selector);
    EnhancedSelector* getEnhancedSelector(const std::string& name);
    bool hasEnhancedSelector(const std::string& name) const;
    
    // 事件委托管理
    void addEventDelegate(const std::string& name, const EventDelegate& delegate);
    EventDelegate* getEventDelegate(const std::string& name);
    bool hasEventDelegate(const std::string& name) const;
    
    // 动画管理
    void addAnimation(const std::string& name, const Animation& animation);
    Animation* getAnimation(const std::string& name);
    bool hasAnimation(const std::string& name) const;
    
    // 路由管理
    void addRoute(const Route& route);
    std::vector<Route> getRoutes() const;
    
    // 响应式值管理
    void addReactiveValue(const std::string& name, const std::string& value);
    std::string getReactiveValue(const std::string& name) const;
    bool hasReactiveValue(const std::string& name) const;
    
    // 文件加载器管理
    void addFileLoader(const std::string& name, const std::vector<std::string>& files);
    std::vector<std::string> getFileLoader(const std::string& name) const;
    bool hasFileLoader(const std::string& name) const;
    
    // 配置管理
    void setConfiguration(const std::string& key, const std::string& value);
    std::string getConfiguration(const std::string& key) const;
    
    // 调试模式
    void setDebugMode(bool enabled);
    bool isDebugMode() const;
    
    // 清理
    void clear();

private:
    // 虚对象存储
    std::unordered_map<std::string, VirtualObject> virtual_objects_;
    
    // 增强选择器存储
    std::unordered_map<std::string, EnhancedSelector> enhanced_selectors_;
    
    // 事件委托存储
    std::unordered_map<std::string, EventDelegate> event_delegates_;
    
    // 动画存储
    std::unordered_map<std::string, Animation> animations_;
    
    // 路由存储
    std::vector<Route> routes_;
    
    // 响应式值存储
    std::unordered_map<std::string, std::string> reactive_values_;
    
    // 文件加载器存储
    std::unordered_map<std::string, std::vector<std::string>> file_loaders_;
    
    // 配置存储
    std::unordered_map<std::string, std::string> configurations_;
    
    // 调试模式
    bool debug_mode_;
};

} // namespace CHTL