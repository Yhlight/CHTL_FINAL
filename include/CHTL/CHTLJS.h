#pragma once

#include "CHTLNode.h"
#include "CHTLContext.h"
#include <string>
#include <memory>
#include <unordered_map>
#include <vector>
#include <set>
#include <functional>

namespace CHTL {

/**
 * @brief CHTL JS 文件加载器
 */
class CHTLJSFileLoader {
public:
    explicit CHTLJSFileLoader(std::shared_ptr<CHTLContext> context);
    ~CHTLJSFileLoader() = default;

    // 文件加载
    std::string loadFile(const std::string& path) const;
    bool fileExists(const std::string& path) const;
    
    // 模块解析
    std::vector<std::string> parseDependencies(const std::string& content) const;
    std::string resolveModulePath(const std::string& moduleName, const std::string& basePath) const;
    
    // 依赖管理
    void addDependency(const std::string& moduleName, const std::string& path);
    std::vector<std::string> getDependencies(const std::string& moduleName) const;
    
    // 缓存管理
    void cacheModule(const std::string& moduleName, const std::string& content);
    std::string getCachedModule(const std::string& moduleName) const;
    bool isModuleCached(const std::string& moduleName) const;

private:
    std::shared_ptr<CHTLContext> m_context;
    std::unordered_map<std::string, std::string> m_dependencies;
    std::unordered_map<std::string, std::string> m_moduleCache;
    
    // 辅助方法
    std::string extractModuleName(const std::string& path) const;
    std::vector<std::string> parseAMDDefine(const std::string& content) const;
};

/**
 * @brief CHTL JS 选择器
 */
class CHTLJSSelector {
public:
    explicit CHTLJSSelector(std::shared_ptr<CHTLContext> context);
    ~CHTLJSSelector() = default;

    // 选择器解析
    std::string parseSelector(const std::string& selector) const;
    std::vector<std::string> parseMultipleSelectors(const std::string& selectors) const;
    
    // 选择器验证
    bool isValidSelector(const std::string& selector) const;
    bool isComplexSelector(const std::string& selector) const;
    
    // 选择器生成
    std::string generateSelectorCode(const std::string& selector) const;
    std::string generateMultipleSelectorCode(const std::vector<std::string>& selectors) const;
    
    // 自动化规则
    std::string applyAutomationRules(const std::string& selector) const;
    std::string addClassPrefix(const std::string& selector) const;
    std::string addIdPrefix(const std::string& selector) const;

private:
    std::shared_ptr<CHTLContext> m_context;
    
    // 辅助方法
    bool isElementSelector(const std::string& selector) const;
    bool isClassSelector(const std::string& selector) const;
    bool isIdSelector(const std::string& selector) const;
    bool isAttributeSelector(const std::string& selector) const;
    bool isPseudoSelector(const std::string& selector) const;
    std::string escapeSelector(const std::string& selector) const;
};

/**
 * @brief CHTL JS 事件监听器
 */
class CHTLJSEventListener {
public:
    explicit CHTLJSEventListener(std::shared_ptr<CHTLContext> context);
    ~CHTLJSEventListener() = default;

    // 事件监听器解析
    std::string parseEventListener(const std::string& eventType, const std::string& handler) const;
    std::vector<std::pair<std::string, std::string>> parseMultipleListeners(const std::string& listeners) const;
    
    // 事件监听器生成
    std::string generateEventListenerCode(const std::string& eventType, const std::string& handler) const;
    std::string generateMultipleListenerCode(const std::vector<std::pair<std::string, std::string>>& listeners) const;
    
    // 事件委托
    std::string generateEventDelegationCode(const std::string& target, const std::string& eventType, const std::string& handler) const;
    std::string parseEventDelegation(const std::string& delegation) const;

private:
    std::shared_ptr<CHTLContext> m_context;
    
    // 辅助方法
    bool isValidEventType(const std::string& eventType) const;
    std::string sanitizeEventHandler(const std::string& handler) const;
    std::string generateEventDelegationHandler(const std::string& target, const std::string& eventType, const std::string& handler) const;
};

/**
 * @brief CHTL JS 动画系统
 */
class CHTLJSAnimation {
public:
    explicit CHTLJSAnimation(std::shared_ptr<CHTLContext> context);
    ~CHTLJSAnimation() = default;

    // 动画解析
    std::string parseAnimation(const std::string& animationConfig) const;
    std::unordered_map<std::string, std::string> parseAnimationProperties(const std::string& config) const;
    
    // 动画生成
    std::string generateAnimationCode(const std::string& animationConfig) const;
    std::string generateAnimationFunction(const std::unordered_map<std::string, std::string>& properties) const;
    
    // 缓动函数
    std::string generateEasingFunction(const std::string& easing) const;
    std::string parseEasing(const std::string& easing) const;
    
    // 动画控制
    std::string generateAnimationControl(const std::string& controlType) const;
    std::string generateLoopControl(const std::string& loopConfig) const;

private:
    std::shared_ptr<CHTLContext> m_context;
    
    // 辅助方法
    bool isValidEasing(const std::string& easing) const;
    std::string generateEasingCode(const std::string& easing) const;
    std::string generateTimingFunction(const std::string& timing) const;
};

/**
 * @brief CHTL JS 虚对象
 */
class CHTLJSVirtualObject {
public:
    explicit CHTLJSVirtualObject(std::shared_ptr<CHTLContext> context);
    ~CHTLJSVirtualObject() = default;

    // 虚对象解析
    std::string parseVirtualObject(const std::string& objectName, const std::string& function) const;
    std::unordered_map<std::string, std::string> parseVirtualObjectProperties(const std::string& function) const;
    
    // 虚对象生成
    std::string generateVirtualObjectCode(const std::string& objectName, const std::string& function) const;
    std::string generateVirtualObjectFunction(const std::unordered_map<std::string, std::string>& properties) const;
    
    // 元信息访问
    std::string generateMetaAccessCode(const std::string& objectName) const;
    std::string generateFunctionMetaCode(const std::string& functionName) const;

private:
    std::shared_ptr<CHTLContext> m_context;
    
    // 辅助方法
    bool isValidVirtualObjectName(const std::string& name) const;
    std::string generateMetaPropertyAccess(const std::string& property) const;
};

/**
 * @brief CHTL JS 路由系统
 */
class CHTLJSRouter {
public:
    explicit CHTLJSRouter(std::shared_ptr<CHTLContext> context);
    ~CHTLJSRouter() = default;

    // 路由解析
    std::string parseRoute(const std::string& routeConfig) const;
    std::unordered_map<std::string, std::string> parseRouteProperties(const std::string& config) const;
    
    // 路由生成
    std::string generateRouterCode(const std::string& routeConfig) const;
    std::string generateRouteHandler(const std::unordered_map<std::string, std::string>& properties) const;
    
    // 路由管理
    std::string generateRouteRegistration(const std::string& url, const std::string& page) const;
    std::string generateRouteNavigation(const std::string& url) const;
    
    // 历史管理
    std::string generateHistoryManagement(const std::string& mode) const;
    std::string generateHashMode() const;
    std::string generateHistoryMode() const;

private:
    std::shared_ptr<CHTLContext> m_context;
    
    // 辅助方法
    bool isValidRouteUrl(const std::string& url) const;
    std::string generateRouteMatcher(const std::string& url) const;
    std::string generateRouteParams(const std::string& url) const;
};

/**
 * @brief CHTL JS 编译器
 */
class CHTLJSCompiler {
public:
    explicit CHTLJSCompiler(std::shared_ptr<CHTLContext> context);
    ~CHTLJSCompiler() = default;

    // 编译 CHTL JS
    std::string compileCHTLJS(const std::string& source) const;
    std::string compileFile(const std::string& filePath) const;
    
    // 语法解析
    std::shared_ptr<BaseNode> parseCHTLJS(const std::string& source) const;
    std::vector<std::shared_ptr<BaseNode>> parseCHTLJSFile(const std::string& filePath) const;
    
    // 代码生成
    std::string generateJavaScript(const std::shared_ptr<BaseNode>& ast) const;
    std::string generateModuleCode(const std::vector<std::shared_ptr<BaseNode>>& nodes) const;
    
    // 优化
    std::string optimizeCode(const std::string& code) const;
    std::string minifyCode(const std::string& code) const;

private:
    std::shared_ptr<CHTLContext> m_context;
    std::unique_ptr<CHTLJSFileLoader> m_fileLoader;
    std::unique_ptr<CHTLJSSelector> m_selector;
    std::unique_ptr<CHTLJSEventListener> m_eventListener;
    std::unique_ptr<CHTLJSAnimation> m_animation;
    std::unique_ptr<CHTLJSVirtualObject> m_virtualObject;
    std::unique_ptr<CHTLJSRouter> m_router;
    
    // 辅助方法
    std::string processFileLoader(const std::string& content) const;
    std::string processSelectors(const std::string& content) const;
    std::string processEventListeners(const std::string& content) const;
    std::string processAnimations(const std::string& content) const;
    std::string processVirtualObjects(const std::string& content) const;
    std::string processRouter(const std::string& content) const;
};

/**
 * @brief CHTL JS 处理器
 */
class CHTLJSProcessor {
public:
    explicit CHTLJSProcessor(std::shared_ptr<CHTLContext> context);
    ~CHTLJSProcessor() = default;

    // 处理 CHTL JS
    void processCHTLJS(std::shared_ptr<BaseNode> root);
    void processCHTLJSFile(const std::string& filePath);
    
    // 处理局部脚本
    void processLocalScripts(std::shared_ptr<BaseNode> root);
    void processScriptNode(std::shared_ptr<BaseNode> scriptNode);
    
    // 处理增强选择器
    void processEnhancedSelectors(std::shared_ptr<BaseNode> root);
    void processSelectorNode(std::shared_ptr<BaseNode> selectorNode);
    
    // 处理事件监听器
    void processEventListeners(std::shared_ptr<BaseNode> root);
    void processEventListenerNode(std::shared_ptr<BaseNode> listenerNode);
    
    // 处理动画
    void processAnimations(std::shared_ptr<BaseNode> root);
    void processAnimationNode(std::shared_ptr<BaseNode> animationNode);
    
    // 处理虚对象
    void processVirtualObjects(std::shared_ptr<BaseNode> root);
    void processVirtualObjectNode(std::shared_ptr<BaseNode> virtualObjectNode);
    
    // 处理路由
    void processRouter(std::shared_ptr<BaseNode> root);
    void processRouterNode(std::shared_ptr<BaseNode> routerNode);

private:
    std::shared_ptr<CHTLContext> m_context;
    std::unique_ptr<CHTLJSCompiler> m_compiler;
    
    // 辅助方法
    void processNode(std::shared_ptr<BaseNode> node);
    void processElementNode(std::shared_ptr<ElementNode> elementNode);
    void processScriptNode(std::shared_ptr<ElementNode> scriptNode);
    void processStyleNode(std::shared_ptr<ElementNode> styleNode);
};

} // namespace CHTL