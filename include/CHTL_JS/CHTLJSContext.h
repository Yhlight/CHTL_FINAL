#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <functional>

namespace CHTL_JS {

/**
 * @brief CHTL JS编译上下文
 */
class CHTLJSContext {
public:
    CHTLJSContext();
    ~CHTLJSContext() = default;

    // 配置管理
    void setDebugMode(bool debug);
    bool isDebugMode() const;
    
    void setMinifyOutput(bool minify);
    bool isMinifyOutput() const;
    
    void setIncludeSourceMaps(bool include);
    bool isIncludeSourceMaps() const;

    // 文件管理
    void addSourceFile(const std::string& filePath);
    const std::vector<std::string>& getSourceFiles() const;
    
    void setOutputDirectory(const std::string& dir);
    const std::string& getOutputDirectory() const;

    // 模块管理
    void addModulePath(const std::string& path);
    const std::vector<std::string>& getModulePaths() const;
    
    void setOfficialModulePath(const std::string& path);
    const std::string& getOfficialModulePath() const;

    // 虚对象管理
    void registerVirtualObject(const std::string& name, const std::string& definition);
    const std::unordered_map<std::string, std::string>& getVirtualObjects() const;
    
    void addVirtualObjectBinding(const std::string& name, const std::string& binding);
    const std::unordered_map<std::string, std::string>& getVirtualObjectBindings() const;

    // 函数管理
    void registerCHTLJSFunction(const std::string& name, const std::string& definition);
    const std::unordered_map<std::string, std::string>& getCHTLJSFunctions() const;
    
    void addFunctionBinding(const std::string& name, const std::string& binding);
    const std::unordered_map<std::string, std::string>& getFunctionBindings() const;

    // 事件委托管理
    void registerEventDelegate(const std::string& parent, const std::string& target, 
                              const std::string& event, const std::string& handler);
    const std::unordered_map<std::string, std::vector<std::string>>& getEventDelegates() const;

    // 路由管理
    void addRoute(const std::string& url, const std::string& page);
    const std::unordered_map<std::string, std::string>& getRoutes() const;
    
    void setRouterMode(const std::string& mode);
    const std::string& getRouterMode() const;
    
    void setRouterRoot(const std::string& root);
    const std::string& getRouterRoot() const;

    // 动画管理
    void registerAnimation(const std::string& name, const std::string& definition);
    const std::unordered_map<std::string, std::string>& getAnimations() const;

    // 错误处理
    void addError(const std::string& error);
    void addWarning(const std::string& warning);
    const std::vector<std::string>& getErrors() const;
    const std::vector<std::string>& getWarnings() const;
    
    bool hasErrors() const;
    void clearErrors();

    // 编译状态
    void setCompilationState(const std::string& state);
    const std::string& getCompilationState() const;

private:
    // 配置选项
    bool m_debugMode;
    bool m_minifyOutput;
    bool m_includeSourceMaps;
    
    // 文件管理
    std::vector<std::string> m_sourceFiles;
    std::string m_outputDirectory;
    
    // 模块管理
    std::vector<std::string> m_modulePaths;
    std::string m_officialModulePath;
    
    // 虚对象管理
    std::unordered_map<std::string, std::string> m_virtualObjects;
    std::unordered_map<std::string, std::string> m_virtualObjectBindings;
    
    // 函数管理
    std::unordered_map<std::string, std::string> m_chtljsFunctions;
    std::unordered_map<std::string, std::string> m_functionBindings;
    
    // 事件委托管理
    std::unordered_map<std::string, std::vector<std::string>> m_eventDelegates;
    
    // 路由管理
    std::unordered_map<std::string, std::string> m_routes;
    std::string m_routerMode;
    std::string m_routerRoot;
    
    // 动画管理
    std::unordered_map<std::string, std::string> m_animations;
    
    // 错误处理
    std::vector<std::string> m_errors;
    std::vector<std::string> m_warnings;
    
    // 编译状态
    std::string m_compilationState;
};

} // namespace CHTL_JS