#ifndef CHTLJSADVANCEDPROCESSOR_H
#define CHTLJSADVANCEDPROCESSOR_H

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>

namespace CHTL {
namespace CHTLJS {

// 事件监听器类型
enum class EventListenerType {
    CLICK,
    MOUSE_OVER,
    MOUSE_OUT,
    KEY_DOWN,
    KEY_UP,
    SUBMIT,
    CHANGE,
    FOCUS,
    BLUR,
    CUSTOM
};

// 动画类型
enum class AnimationType {
    FADE_IN,
    FADE_OUT,
    SLIDE_IN,
    SLIDE_OUT,
    SCALE_IN,
    SCALE_OUT,
    ROTATE,
    CUSTOM
};

// 路由类型
enum class RouteType {
    HASH,
    HISTORY,
    CUSTOM
};

// 虚对象类型
enum class VirtualObjectType {
    COMPONENT,
    WIDGET,
    MODAL,
    TOOLTIP,
    CUSTOM
};

// 事件监听器配置
struct EventListenerConfig {
    std::string eventName;
    std::string selector;
    std::function<void()> handler;
    bool useCapture = false;
    bool once = false;
    bool passive = false;
    std::string namespaceName;
};

// 动画配置
struct AnimationConfig {
    AnimationType type;
    int duration = 300;
    std::string easing = "ease";
    int delay = 0;
    int iterationCount = 1;
    std::string direction = "normal";
    bool fillMode = true;
    std::map<std::string, std::string> keyframes;
};

// 路由配置
struct RouteConfig {
    RouteType type;
    std::string path;
    std::function<void()> handler;
    std::map<std::string, std::string> params;
    bool exact = false;
    std::string component;
};

// 虚对象配置
struct VirtualObjectConfig {
    VirtualObjectType type;
    std::string name;
    std::map<std::string, std::string> props;
    std::map<std::string, std::function<void()>> methods;
    std::string templateContent;
    std::map<std::string, std::string> styles;
};

// 响应式值配置
struct ResponsiveValueConfig {
    std::string variableName;
    std::string expression;
    std::function<void()> updateHandler;
    bool immediate = true;
    std::vector<std::string> dependencies;
};

// CHTL JS高级功能处理器
class CHTLJSAdvancedProcessor {
public:
    CHTLJSAdvancedProcessor();
    ~CHTLJSAdvancedProcessor();

    // 增强监听器
    void addEventListener(const std::string& selector, const std::string& eventName, 
                         std::function<void()> handler, const EventListenerConfig& config = EventListenerConfig());
    void removeEventListener(const std::string& selector, const std::string& eventName);
    void clearEventListeners(const std::string& selector);
    void clearAllEventListeners();

    // 事件委托
    void delegate(const std::string& parentSelector, const std::string& childSelector, 
                 const std::string& eventName, std::function<void()> handler);
    void undelegate(const std::string& parentSelector, const std::string& childSelector, 
                   const std::string& eventName);
    void clearDelegates(const std::string& parentSelector);

    // 动画
    void animate(const std::string& selector, const AnimationConfig& config);
    void stopAnimation(const std::string& selector);
    void pauseAnimation(const std::string& selector);
    void resumeAnimation(const std::string& selector);
    void clearAnimations(const std::string& selector);

    // 虚对象
    void createVirtualObject(const std::string& name, const VirtualObjectConfig& config);
    void destroyVirtualObject(const std::string& name);
    void updateVirtualObject(const std::string& name, const std::map<std::string, std::string>& props);
    void renderVirtualObject(const std::string& name, const std::string& container);

    // 路由
    void addRoute(const std::string& path, const RouteConfig& config);
    void removeRoute(const std::string& path);
    void navigateTo(const std::string& path);
    void navigateBack();
    void navigateForward();
    void clearRoutes();

    // 响应式值
    void addResponsiveValue(const std::string& variableName, const ResponsiveValueConfig& config);
    void removeResponsiveValue(const std::string& variableName);
    void updateResponsiveValue(const std::string& variableName, const std::string& newValue);
    void clearResponsiveValues();

    // 动态属性条件表达式
    void addDynamicAttribute(const std::string& selector, const std::string& attributeName, 
                            const std::string& expression);
    void removeDynamicAttribute(const std::string& selector, const std::string& attributeName);
    void updateDynamicAttribute(const std::string& selector, const std::string& attributeName, 
                               const std::string& newExpression);
    void clearDynamicAttributes(const std::string& selector);

    // 工具方法
    std::string generateJavaScript() const;
    std::string generateCSS() const;
    std::string generateHTML() const;
    
    // 状态管理
    void reset();
    void clear();
    
    // 错误处理
    std::vector<std::string> getErrors() const;
    std::vector<std::string> getWarnings() const;
    void addError(const std::string& error);
    void addWarning(const std::string& warning);
    void clearErrors();
    void clearWarnings();

private:
    // 内部状态
    std::vector<EventListenerConfig> eventListeners;
    std::vector<std::pair<std::string, std::string>> delegates; // parentSelector, childSelector
    std::map<std::string, AnimationConfig> animations;
    std::map<std::string, VirtualObjectConfig> virtualObjects;
    std::map<std::string, RouteConfig> routes;
    std::map<std::string, ResponsiveValueConfig> responsiveValues;
    std::map<std::string, std::map<std::string, std::string>> dynamicAttributes;
    
    // 错误和警告
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    
    // 内部方法
    std::string generateEventListenerCode(const EventListenerConfig& config) const;
    std::string generateDelegateCode(const std::string& parentSelector, const std::string& childSelector, 
                                   const std::string& eventName) const;
    std::string generateAnimationCode(const std::string& selector, const AnimationConfig& config) const;
    std::string generateVirtualObjectCode(const std::string& name, const VirtualObjectConfig& config) const;
    std::string generateRouteCode(const std::string& path, const RouteConfig& config) const;
    std::string generateResponsiveValueCode(const std::string& variableName, const ResponsiveValueConfig& config) const;
    std::string generateDynamicAttributeCode(const std::string& selector, const std::string& attributeName, 
                                           const std::string& expression) const;
    
    // 验证方法
    bool validateSelector(const std::string& selector) const;
    bool validateEventName(const std::string& eventName) const;
    bool validateAnimationConfig(const AnimationConfig& config) const;
    bool validateRouteConfig(const RouteConfig& config) const;
    bool validateVirtualObjectConfig(const VirtualObjectConfig& config) const;
    bool validateResponsiveValueConfig(const ResponsiveValueConfig& config) const;
};

} // namespace CHTLJS
} // namespace CHTL

#endif // CHTLJSADVANCEDPROCESSOR_H