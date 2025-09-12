#include "CHTLJSAdvancedProcessor.h"
#include <iostream>
#include <sstream>
#include <algorithm>

namespace CHTL {
namespace CHTLJS {

CHTLJSAdvancedProcessor::CHTLJSAdvancedProcessor() {
    // 初始化处理器
}

CHTLJSAdvancedProcessor::~CHTLJSAdvancedProcessor() {
    // 清理资源
}

// 增强监听器实现
void CHTLJSAdvancedProcessor::addEventListener(const std::string& selector, const std::string& eventName, 
                                               std::function<void()> handler, const EventListenerConfig& config) {
    if (!validateSelector(selector)) {
        addError("无效的选择器: " + selector);
        return;
    }
    
    if (!validateEventName(eventName)) {
        addError("无效的事件名称: " + eventName);
        return;
    }
    
    EventListenerConfig listenerConfig = config;
    listenerConfig.selector = selector;
    listenerConfig.eventName = eventName;
    listenerConfig.handler = handler;
    
    eventListeners.push_back(listenerConfig);
}

void CHTLJSAdvancedProcessor::removeEventListener(const std::string& selector, const std::string& eventName) {
    eventListeners.erase(
        std::remove_if(eventListeners.begin(), eventListeners.end(),
            [&](const EventListenerConfig& config) {
                return config.selector == selector && config.eventName == eventName;
            }),
        eventListeners.end()
    );
}

void CHTLJSAdvancedProcessor::clearEventListeners(const std::string& selector) {
    eventListeners.erase(
        std::remove_if(eventListeners.begin(), eventListeners.end(),
            [&](const EventListenerConfig& config) {
                return config.selector == selector;
            }),
        eventListeners.end()
    );
}

void CHTLJSAdvancedProcessor::clearAllEventListeners() {
    eventListeners.clear();
}

// 事件委托实现
void CHTLJSAdvancedProcessor::delegate(const std::string& parentSelector, const std::string& childSelector, 
                                       const std::string& eventName, std::function<void()> handler) {
    if (!validateSelector(parentSelector) || !validateSelector(childSelector)) {
        addError("无效的选择器: " + parentSelector + " 或 " + childSelector);
        return;
    }
    
    if (!validateEventName(eventName)) {
        addError("无效的事件名称: " + eventName);
        return;
    }
    
    delegates.push_back({parentSelector, childSelector});
}

void CHTLJSAdvancedProcessor::undelegate(const std::string& parentSelector, const std::string& childSelector, 
                                         const std::string& eventName) {
    delegates.erase(
        std::remove_if(delegates.begin(), delegates.end(),
            [&](const std::pair<std::string, std::string>& delegate) {
                return delegate.first == parentSelector && delegate.second == childSelector;
            }),
        delegates.end()
    );
}

void CHTLJSAdvancedProcessor::clearDelegates(const std::string& parentSelector) {
    delegates.erase(
        std::remove_if(delegates.begin(), delegates.end(),
            [&](const std::pair<std::string, std::string>& delegate) {
                return delegate.first == parentSelector;
            }),
        delegates.end()
    );
}

// 动画实现
void CHTLJSAdvancedProcessor::animate(const std::string& selector, const AnimationConfig& config) {
    if (!validateSelector(selector)) {
        addError("无效的选择器: " + selector);
        return;
    }
    
    if (!validateAnimationConfig(config)) {
        addError("无效的动画配置");
        return;
    }
    
    animations[selector] = config;
}

void CHTLJSAdvancedProcessor::stopAnimation(const std::string& selector) {
    auto it = animations.find(selector);
    if (it != animations.end()) {
        animations.erase(it);
    }
}

void CHTLJSAdvancedProcessor::pauseAnimation(const std::string& selector) {
    // 实现动画暂停逻辑
}

void CHTLJSAdvancedProcessor::resumeAnimation(const std::string& selector) {
    // 实现动画恢复逻辑
}

void CHTLJSAdvancedProcessor::clearAnimations(const std::string& selector) {
    animations.erase(selector);
}

// 虚对象实现
void CHTLJSAdvancedProcessor::createVirtualObject(const std::string& name, const VirtualObjectConfig& config) {
    if (!validateVirtualObjectConfig(config)) {
        addError("无效的虚对象配置");
        return;
    }
    
    virtualObjects[name] = config;
}

void CHTLJSAdvancedProcessor::destroyVirtualObject(const std::string& name) {
    virtualObjects.erase(name);
}

void CHTLJSAdvancedProcessor::updateVirtualObject(const std::string& name, const std::map<std::string, std::string>& props) {
    auto it = virtualObjects.find(name);
    if (it != virtualObjects.end()) {
        for (const auto& prop : props) {
            it->second.props[prop.first] = prop.second;
        }
    }
}

void CHTLJSAdvancedProcessor::renderVirtualObject(const std::string& name, const std::string& container) {
    auto it = virtualObjects.find(name);
    if (it != virtualObjects.end()) {
        // 实现虚对象渲染逻辑
    }
}

// 路由实现
void CHTLJSAdvancedProcessor::addRoute(const std::string& path, const RouteConfig& config) {
    if (!validateRouteConfig(config)) {
        addError("无效的路由配置");
        return;
    }
    
    routes[path] = config;
}

void CHTLJSAdvancedProcessor::removeRoute(const std::string& path) {
    routes.erase(path);
}

void CHTLJSAdvancedProcessor::navigateTo(const std::string& path) {
    auto it = routes.find(path);
    if (it != routes.end()) {
        if (it->second.handler) {
            it->second.handler();
        }
    }
}

void CHTLJSAdvancedProcessor::navigateBack() {
    // 实现后退导航逻辑
}

void CHTLJSAdvancedProcessor::navigateForward() {
    // 实现前进导航逻辑
}

void CHTLJSAdvancedProcessor::clearRoutes() {
    routes.clear();
}

// 响应式值实现
void CHTLJSAdvancedProcessor::addResponsiveValue(const std::string& variableName, const ResponsiveValueConfig& config) {
    if (!validateResponsiveValueConfig(config)) {
        addError("无效的响应式值配置");
        return;
    }
    
    responsiveValues[variableName] = config;
}

void CHTLJSAdvancedProcessor::removeResponsiveValue(const std::string& variableName) {
    responsiveValues.erase(variableName);
}

void CHTLJSAdvancedProcessor::updateResponsiveValue(const std::string& variableName, const std::string& newValue) {
    auto it = responsiveValues.find(variableName);
    if (it != responsiveValues.end()) {
        it->second.expression = newValue;
        if (it->second.updateHandler) {
            it->second.updateHandler();
        }
    }
}

void CHTLJSAdvancedProcessor::clearResponsiveValues() {
    responsiveValues.clear();
}

// 动态属性条件表达式实现
void CHTLJSAdvancedProcessor::addDynamicAttribute(const std::string& selector, const std::string& attributeName, 
                                                  const std::string& expression) {
    if (!validateSelector(selector)) {
        addError("无效的选择器: " + selector);
        return;
    }
    
    dynamicAttributes[selector][attributeName] = expression;
}

void CHTLJSAdvancedProcessor::removeDynamicAttribute(const std::string& selector, const std::string& attributeName) {
    auto it = dynamicAttributes.find(selector);
    if (it != dynamicAttributes.end()) {
        it->second.erase(attributeName);
        if (it->second.empty()) {
            dynamicAttributes.erase(it);
        }
    }
}

void CHTLJSAdvancedProcessor::updateDynamicAttribute(const std::string& selector, const std::string& attributeName, 
                                                     const std::string& newExpression) {
    auto it = dynamicAttributes.find(selector);
    if (it != dynamicAttributes.end()) {
        it->second[attributeName] = newExpression;
    }
}

void CHTLJSAdvancedProcessor::clearDynamicAttributes(const std::string& selector) {
    dynamicAttributes.erase(selector);
}

// 工具方法实现
std::string CHTLJSAdvancedProcessor::generateJavaScript() const {
    std::ostringstream oss;
    
    // 生成事件监听器代码
    for (const auto& listener : eventListeners) {
        oss << generateEventListenerCode(listener) << std::endl;
    }
    
    // 生成事件委托代码
    for (const auto& delegate : delegates) {
        oss << generateDelegateCode(delegate.first, delegate.second, "click") << std::endl;
    }
    
    // 生成动画代码
    for (const auto& animation : animations) {
        oss << generateAnimationCode(animation.first, animation.second) << std::endl;
    }
    
    // 生成虚对象代码
    for (const auto& virtualObject : virtualObjects) {
        oss << generateVirtualObjectCode(virtualObject.first, virtualObject.second) << std::endl;
    }
    
    // 生成路由代码
    for (const auto& route : routes) {
        oss << generateRouteCode(route.first, route.second) << std::endl;
    }
    
    // 生成响应式值代码
    for (const auto& responsiveValue : responsiveValues) {
        oss << generateResponsiveValueCode(responsiveValue.first, responsiveValue.second) << std::endl;
    }
    
    // 生成动态属性代码
    for (const auto& dynamicAttribute : dynamicAttributes) {
        for (const auto& attr : dynamicAttribute.second) {
            oss << generateDynamicAttributeCode(dynamicAttribute.first, attr.first, attr.second) << std::endl;
        }
    }
    
    return oss.str();
}

std::string CHTLJSAdvancedProcessor::generateCSS() const {
    std::ostringstream oss;
    
    // 生成动画CSS
    for (const auto& animation : animations) {
        oss << "@keyframes " << animation.first << "_animation {" << std::endl;
        for (const auto& keyframe : animation.second.keyframes) {
            oss << "  " << keyframe.first << " { " << keyframe.second << " }" << std::endl;
        }
        oss << "}" << std::endl;
    }
    
    return oss.str();
}

std::string CHTLJSAdvancedProcessor::generateHTML() const {
    std::ostringstream oss;
    
    // 生成虚对象HTML
    for (const auto& virtualObject : virtualObjects) {
        oss << "<!-- 虚对象: " << virtualObject.first << " -->" << std::endl;
        oss << "<div id=\"" << virtualObject.first << "\">" << std::endl;
        oss << virtualObject.second.templateContent << std::endl;
        oss << "</div>" << std::endl;
    }
    
    return oss.str();
}

// 状态管理实现
void CHTLJSAdvancedProcessor::reset() {
    clear();
}

void CHTLJSAdvancedProcessor::clear() {
    eventListeners.clear();
    delegates.clear();
    animations.clear();
    virtualObjects.clear();
    routes.clear();
    responsiveValues.clear();
    dynamicAttributes.clear();
    errors.clear();
    warnings.clear();
}

// 错误处理实现
std::vector<std::string> CHTLJSAdvancedProcessor::getErrors() const {
    return errors;
}

std::vector<std::string> CHTLJSAdvancedProcessor::getWarnings() const {
    return warnings;
}

void CHTLJSAdvancedProcessor::addError(const std::string& error) {
    errors.push_back(error);
}

void CHTLJSAdvancedProcessor::addWarning(const std::string& warning) {
    warnings.push_back(warning);
}

void CHTLJSAdvancedProcessor::clearErrors() {
    errors.clear();
}

void CHTLJSAdvancedProcessor::clearWarnings() {
    warnings.clear();
}

// 内部方法实现
std::string CHTLJSAdvancedProcessor::generateEventListenerCode(const EventListenerConfig& config) const {
    std::ostringstream oss;
    oss << "document.querySelectorAll('" << config.selector << "').forEach(function(element) {" << std::endl;
    oss << "  element.addEventListener('" << config.eventName << "', function(event) {" << std::endl;
    oss << "    // 事件处理逻辑" << std::endl;
    oss << "  }, " << (config.useCapture ? "true" : "false") << ");" << std::endl;
    oss << "});" << std::endl;
    return oss.str();
}

std::string CHTLJSAdvancedProcessor::generateDelegateCode(const std::string& parentSelector, const std::string& childSelector, 
                                                         const std::string& eventName) const {
    std::ostringstream oss;
    oss << "document.querySelector('" << parentSelector << "').addEventListener('" << eventName << "', function(event) {" << std::endl;
    oss << "  if (event.target.matches('" << childSelector << "')) {" << std::endl;
    oss << "    // 委托事件处理逻辑" << std::endl;
    oss << "  }" << std::endl;
    oss << "});" << std::endl;
    return oss.str();
}

std::string CHTLJSAdvancedProcessor::generateAnimationCode(const std::string& selector, const AnimationConfig& config) const {
    std::ostringstream oss;
    oss << "document.querySelectorAll('" << selector << "').forEach(function(element) {" << std::endl;
    oss << "  element.style.animation = 'animation_" << static_cast<int>(config.type) << " " << config.duration << "ms " << config.easing << "';" << std::endl;
    oss << "});" << std::endl;
    return oss.str();
}

std::string CHTLJSAdvancedProcessor::generateVirtualObjectCode(const std::string& name, const VirtualObjectConfig& config) const {
    std::ostringstream oss;
    oss << "var " << name << " = {" << std::endl;
    oss << "  type: '" << static_cast<int>(config.type) << "'," << std::endl;
    oss << "  props: " << "{";
    for (const auto& prop : config.props) {
        oss << "'" << prop.first << "': '" << prop.second << "', ";
    }
    oss << "}," << std::endl;
    oss << "  template: '" << config.templateContent << "'," << std::endl;
    oss << "  render: function() { /* 渲染逻辑 */ }" << std::endl;
    oss << "};" << std::endl;
    return oss.str();
}

std::string CHTLJSAdvancedProcessor::generateRouteCode(const std::string& path, const RouteConfig& config) const {
    std::ostringstream oss;
    oss << "router.addRoute('" << path << "', function() {" << std::endl;
    oss << "  // 路由处理逻辑" << std::endl;
    oss << "});" << std::endl;
    return oss.str();
}

std::string CHTLJSAdvancedProcessor::generateResponsiveValueCode(const std::string& variableName, const ResponsiveValueConfig& config) const {
    std::ostringstream oss;
    oss << "var " << variableName << " = " << config.expression << ";" << std::endl;
    oss << "// 响应式更新逻辑" << std::endl;
    return oss.str();
}

std::string CHTLJSAdvancedProcessor::generateDynamicAttributeCode(const std::string& selector, const std::string& attributeName, 
                                                                 const std::string& expression) const {
    std::ostringstream oss;
    oss << "document.querySelectorAll('" << selector << "').forEach(function(element) {" << std::endl;
    oss << "  element.setAttribute('" << attributeName << "', " << expression << ");" << std::endl;
    oss << "});" << std::endl;
    return oss.str();
}

// 验证方法实现
bool CHTLJSAdvancedProcessor::validateSelector(const std::string& selector) const {
    return !selector.empty() && selector.length() > 0;
}

bool CHTLJSAdvancedProcessor::validateEventName(const std::string& eventName) const {
    return !eventName.empty() && eventName.length() > 0;
}

bool CHTLJSAdvancedProcessor::validateAnimationConfig(const AnimationConfig& config) const {
    return config.duration > 0 && !config.easing.empty();
}

bool CHTLJSAdvancedProcessor::validateRouteConfig(const RouteConfig& config) const {
    return !config.path.empty();
}

bool CHTLJSAdvancedProcessor::validateVirtualObjectConfig(const VirtualObjectConfig& config) const {
    return !config.name.empty();
}

bool CHTLJSAdvancedProcessor::validateResponsiveValueConfig(const ResponsiveValueConfig& config) const {
    return !config.variableName.empty() && !config.expression.empty();
}

} // namespace CHTLJS
} // namespace CHTL