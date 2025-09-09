#pragma once

#include "BaseNode.h"

namespace CHTL {

enum class ScriptType {
    INLINE,         // 内联脚本
    GLOBAL,         // 全局脚本
    CHTL_JS,        // CHTL JS脚本
    FILELOADER,     // 文件加载器
    LISTEN,         // 增强监听器
    DELEGATE,       // 事件委托
    ANIMATE,        // 动画
    VIR,            // 虚对象
    ROUTER,         // 路由
    UTIL            // util表达式
};

class ScriptNode : public BaseNode {
private:
    ScriptType scriptType;
    std::string scriptContent;
    std::map<std::string, std::string> parameters;
    std::vector<std::shared_ptr<BaseNode>> children;
    
    // CHTL JS特性
    std::vector<std::string> fileLoaderPaths;
    std::map<std::string, std::string> eventListeners;
    std::map<std::string, std::string> delegates;
    std::map<std::string, std::string> animations;
    std::map<std::string, std::string> virtualObjects;
    std::map<std::string, std::string> routes;
    
    // 增强选择器
    std::vector<std::string> selectors;
    std::string contextSelector;
    
    // 自动化规则
    bool autoAddClass;
    bool autoAddId;
    
public:
    ScriptNode(ScriptType type = ScriptType::INLINE, const std::string& content = "");
    virtual ~ScriptNode() = default;
    
    // 脚本类型
    ScriptType getScriptType() const { return scriptType; }
    void setScriptType(ScriptType type) { scriptType = type; }
    
    // 脚本内容
    const std::string& getScriptContent() const { return scriptContent; }
    void setScriptContent(const std::string& content) { scriptContent = content; }
    
    // 参数管理
    void setParameter(const std::string& key, const std::string& value);
    std::string getParameter(const std::string& key) const;
    bool hasParameter(const std::string& key) const;
    void removeParameter(const std::string& key);
    const std::map<std::string, std::string>& getParameters() const { return parameters; }
    
    // 子节点管理
    void addChild(std::shared_ptr<BaseNode> child);
    void removeChild(std::shared_ptr<BaseNode> child);
    std::vector<std::shared_ptr<BaseNode>>& getChildren() { return children; }
    const std::vector<std::shared_ptr<BaseNode>>& getChildren() const { return children; }
    
    // 文件加载器
    void addFileLoaderPath(const std::string& path);
    const std::vector<std::string>& getFileLoaderPaths() const { return fileLoaderPaths; }
    
    // 事件监听器
    void addEventListener(const std::string& event, const std::string& handler);
    const std::map<std::string, std::string>& getEventListeners() const { return eventListeners; }
    
    // 事件委托
    void addDelegate(const std::string& target, const std::string& config);
    const std::map<std::string, std::string>& getDelegates() const { return delegates; }
    
    // 动画
    void addAnimation(const std::string& name, const std::string& config);
    const std::map<std::string, std::string>& getAnimations() const { return animations; }
    
    // 虚对象
    void addVirtualObject(const std::string& name, const std::string& config);
    const std::map<std::string, std::string>& getVirtualObjects() const { return virtualObjects; }
    
    // 路由
    void addRoute(const std::string& path, const std::string& config);
    const std::map<std::string, std::string>& getRoutes() const { return routes; }
    
    // 增强选择器
    void addSelector(const std::string& selector);
    const std::vector<std::string>& getSelectors() const { return selectors; }
    
    void setContextSelector(const std::string& selector) { contextSelector = selector; }
    const std::string& getContextSelector() const { return contextSelector; }
    
    // 自动化规则
    void setAutoAddClass(bool autoAdd) { autoAddClass = autoAdd; }
    bool isAutoAddClass() const { return autoAddClass; }
    
    void setAutoAddId(bool autoAdd) { autoAddId = autoAdd; }
    bool isAutoAddId() const { return autoAddId; }
    
    // 克隆
    virtual std::shared_ptr<BaseNode> clone() const override;
    
    // 序列化
    virtual std::string toString() const override;
    virtual std::string toHTML() const override;
    virtual std::string toCSS() const override;
    virtual std::string toJS() const override;
    
    // 验证
    virtual bool isValid() const override;
    
    // 调试
    virtual std::string debugString() const override;
    
    // 静态方法
    static std::string generateFileLoaderJS(const std::vector<std::string>& paths);
    static std::string generateEventListenerJS(const std::map<std::string, std::string>& listeners);
    static std::string generateDelegateJS(const std::map<std::string, std::string>& delegates);
    static std::string generateAnimationJS(const std::map<std::string, std::string>& animations);
    static std::string generateRouterJS(const std::map<std::string, std::string>& routes);
    
private:
    std::string processCHTLJSSyntax(const std::string& content);
    std::string processSelectors(const std::string& content);
    std::string processArrows(const std::string& content);
};

} // namespace CHTL