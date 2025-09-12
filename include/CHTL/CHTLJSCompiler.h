#pragma once

#include "CHTL/CHTLNode.h"
#include "CHTL/ConfigurationSystem.h"
#include "Scanner/UnifiedScanner.h"
#include <string>
#include <vector>
#include <memory>
#include <map>
#include <functional>

namespace CHTL {

/**
 * CHTL JS文件载入器
 */
class CHTLJSFileLoader {
public:
    CHTLJSFileLoader();
    ~CHTLJSFileLoader();
    
    // 文件载入
    void addFile(const std::string& filePath);
    void addFiles(const std::vector<std::string>& filePaths);
    void removeFile(const std::string& filePath);
    void clearFiles();
    
    // 文件查询
    std::vector<std::string> getFiles() const;
    bool hasFile(const std::string& filePath) const;
    
    // 文件处理
    std::string processFiles() const;
    std::vector<std::string> getFileContents() const;
    
    // 验证
    bool validateFiles() const;
    std::vector<std::string> getValidationErrors() const;
    
private:
    std::vector<std::string> files_;
    
    // 文件处理辅助函数
    std::string readFile(const std::string& filePath) const;
    bool fileExists(const std::string& filePath) const;
    std::string getFileExtension(const std::string& filePath) const;
};

/**
 * CHTL JS增强选择器
 */
class CHTLJSEnhancedSelector {
public:
    CHTLJSEnhancedSelector();
    ~CHTLJSEnhancedSelector();
    
    // 选择器类型
    enum class SelectorType {
        TAG,        // {{button}}
        CLASS,      // {{.box}}
        ID,         // {{#box}}
        DESCENDANT, // {{.box button}}
        INDEXED     // {{button[0]}}
    };
    
    // 选择器解析
    SelectorType parseSelector(const std::string& selector) const;
    std::string generateJavaScript(const std::string& selector) const;
    
    // 选择器验证
    bool validateSelector(const std::string& selector) const;
    std::vector<std::string> getValidationErrors(const std::string& selector) const;
    
private:
    // 解析辅助函数
    std::string extractTagName(const std::string& selector) const;
    std::string extractClassName(const std::string& selector) const;
    std::string extractIdName(const std::string& selector) const;
    std::string extractDescendantSelector(const std::string& selector) const;
    int extractIndex(const std::string& selector) const;
    
    // 生成辅助函数
    std::string generateTagSelector(const std::string& tagName) const;
    std::string generateClassSelector(const std::string& className) const;
    std::string generateIdSelector(const std::string& idName) const;
    std::string generateDescendantSelector(const std::string& descendantSelector) const;
    std::string generateIndexedSelector(const std::string& baseSelector, int index) const;
};

/**
 * CHTL JS动画系统
 */
class CHTLJSAnimationSystem {
public:
    CHTLJSAnimationSystem();
    ~CHTLJSAnimationSystem();
    
    // 动画配置
    struct AnimationConfig {
        std::string target;
        int duration;
        std::string easing;
        std::map<std::string, std::string> begin;
        std::vector<std::map<std::string, std::string>> when;
        std::map<std::string, std::string> end;
        int loop;
        std::string direction;
        int delay;
        std::string callback;
    };
    
    // 动画处理
    std::string generateAnimation(const AnimationConfig& config) const;
    std::string generateAnimationFunction(const AnimationConfig& config) const;
    
    // 动画验证
    bool validateAnimation(const AnimationConfig& config) const;
    std::vector<std::string> getValidationErrors(const AnimationConfig& config) const;
    
private:
    // 生成辅助函数
    std::string generateTargetSelector(const std::string& target) const;
    std::string generateEasingFunction(const std::string& easing) const;
    std::string generateKeyframes(const std::vector<std::map<std::string, std::string>>& when) const;
    std::string generateCallback(const std::string& callback) const;
};

/**
 * CHTL JS路由系统
 */
class CHTLJSRoutingSystem {
public:
    CHTLJSRoutingSystem();
    ~CHTLJSRoutingSystem();
    
    // 路由配置
    struct RouteConfig {
        std::string path;
        std::string component;
        std::map<std::string, std::string> params;
        std::vector<std::string> children;
    };
    
    // 路由处理
    void addRoute(const RouteConfig& route);
    void removeRoute(const std::string& path);
    std::string generateRouter() const;
    std::string generateRouteHandler(const RouteConfig& route) const;
    
    // 路由查询
    std::vector<RouteConfig> getRoutes() const;
    RouteConfig getRoute(const std::string& path) const;
    bool hasRoute(const std::string& path) const;
    
    // 路由验证
    bool validateRoute(const RouteConfig& route) const;
    std::vector<std::string> getValidationErrors(const RouteConfig& route) const;
    
private:
    std::vector<RouteConfig> routes_;
    
    // 生成辅助函数
    std::string generateRouteMatcher(const std::string& path) const;
    std::string generateParamExtractor(const std::map<std::string, std::string>& params) const;
    std::string generateComponentLoader(const std::string& component) const;
};

/**
 * CHTL JS响应式系统
 */
class CHTLJSReactiveSystem {
public:
    CHTLJSReactiveSystem();
    ~CHTLJSReactiveSystem();
    
    // 响应式值
    struct ReactiveValue {
        std::string name;
        std::string initialValue;
        std::string type;
        std::vector<std::string> dependencies;
    };
    
    // 响应式处理
    void addReactiveValue(const ReactiveValue& value);
    void removeReactiveValue(const std::string& name);
    std::string generateReactiveSystem() const;
    std::string generateReactiveValue(const ReactiveValue& value) const;
    
    // 响应式查询
    std::vector<ReactiveValue> getReactiveValues() const;
    ReactiveValue getReactiveValue(const std::string& name) const;
    bool hasReactiveValue(const std::string& name) const;
    
    // 响应式验证
    bool validateReactiveValue(const ReactiveValue& value) const;
    std::vector<std::string> getValidationErrors(const ReactiveValue& value) const;
    
private:
    std::vector<ReactiveValue> reactive_values_;
    
    // 生成辅助函数
    std::string generateReactiveProxy(const ReactiveValue& value) const;
    std::string generateDependencyTracker(const std::vector<std::string>& dependencies) const;
    std::string generateUpdateHandler(const ReactiveValue& value) const;
};

/**
 * CHTL JS编译器
 */
class CHTLJSCompiler {
public:
    CHTLJSCompiler();
    ~CHTLJSCompiler();
    
    // 编译器配置
    void setConfigurationManager(std::shared_ptr<ConfigurationManager> configManager);
    std::shared_ptr<ConfigurationManager> getConfigurationManager() const;
    
    // 编译处理
    std::string compile(const std::string& content);
    std::string compileFile(const std::string& filePath);
    std::string compileFiles(const std::vector<std::string>& filePaths);
    
    // 组件管理
    void addComponent(const std::string& name, const std::string& content);
    void removeComponent(const std::string& name);
    std::string getComponent(const std::string& name) const;
    bool hasComponent(const std::string& name) const;
    
    // 模块管理
    void addModule(const std::string& name, const std::string& content);
    void removeModule(const std::string& name);
    std::string getModule(const std::string& name) const;
    bool hasModule(const std::string& name) const;
    
    // 编译验证
    bool validate(const std::string& content) const;
    std::vector<std::string> getValidationErrors(const std::string& content) const;
    
    // 清理
    void clear();
    void clearComponents();
    void clearModules();
    
private:
    std::shared_ptr<ConfigurationManager> config_manager_;
    std::map<std::string, std::string> components_;
    std::map<std::string, std::string> modules_;
    
    CHTLJSFileLoader file_loader_;
    CHTLJSEnhancedSelector enhanced_selector_;
    CHTLJSAnimationSystem animation_system_;
    CHTLJSRoutingSystem routing_system_;
    CHTLJSReactiveSystem reactive_system_;
    
    // 编译辅助函数
    std::string processFileLoader(const std::string& content) const;
    std::string processEnhancedSelectors(const std::string& content) const;
    std::string processAnimations(const std::string& content) const;
    std::string processRouting(const std::string& content) const;
    std::string processReactiveValues(const std::string& content) const;
    
    // 解析辅助函数
    std::string parseFileLoaderBlock(const std::string& content) const;
    std::string parseEnhancedSelector(const std::string& content) const;
    std::string parseAnimationBlock(const std::string& content) const;
    std::string parseRouteBlock(const std::string& content) const;
    std::string parseReactiveValue(const std::string& content) const;
    
    // 生成辅助函数
    std::string generateFileLoaderCode(const std::vector<std::string>& files) const;
    std::string generateEnhancedSelectorCode(const std::string& selector) const;
    std::string generateAnimationCode(const std::string& animation) const;
    std::string generateRouteCode(const std::string& route) const;
    std::string generateReactiveCode(const std::string& reactive) const;
};

} // namespace CHTL