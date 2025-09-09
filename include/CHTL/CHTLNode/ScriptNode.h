#pragma once

#include "BaseNode.h"

namespace CHTL {

/**
 * @brief 脚本类型枚举
 */
enum class ScriptType {
    LOCAL,      // 局部脚本块
    GLOBAL,     // 全局脚本块
    CHTL_JS     // CHTL JS脚本
};

/**
 * @brief 脚本节点
 * 表示JavaScript或CHTL JS脚本定义
 */
class ScriptNode : public BaseNode {
public:
    ScriptNode(ScriptType type = ScriptType::LOCAL);
    virtual ~ScriptNode();
    
    // 脚本类型
    ScriptType getScriptType() const;
    
    // 脚本内容
    void setScriptContent(const std::string& content);
    std::string getScriptContent() const;
    
    // CHTL JS函数
    void addCHTLJSFunction(const std::string& name, const std::string& content);
    std::string getCHTLJSFunction(const std::string& name) const;
    bool hasCHTLJSFunction(const std::string& name) const;
    const std::unordered_map<std::string, std::string>& getCHTLJSFunctions() const;
    
    // 增强选择器
    void addSelector(const std::string& selector);
    std::vector<std::string> getSelectors() const;
    bool hasSelector(const std::string& selector) const;
    
    // 增强监听器
    void addListener(const std::string& event, const std::string& handler);
    std::unordered_map<std::string, std::string> getListeners() const;
    
    // 事件委托
    void addDelegate(const std::string& parentSelector, const std::string& targetSelector, 
                   const std::string& event, const std::string& handler);
    std::vector<std::tuple<std::string, std::string, std::string, std::string>> getDelegates() const;
    
    // 动画定义
    void addAnimation(const std::string& name, const std::string& animationDef);
    std::string getAnimation(const std::string& name) const;
    bool hasAnimation(const std::string& name) const;
    const std::unordered_map<std::string, std::string>& getAnimations() const;
    
    // 虚对象定义
    void addVirtualObject(const std::string& name, const std::string& definition);
    std::string getVirtualObject(const std::string& name) const;
    bool hasVirtualObject(const std::string& name) const;
    const std::unordered_map<std::string, std::string>& getVirtualObjects() const;
    
    // 路由定义
    void addRoute(const std::string& url, const std::string& page);
    std::string getRoute(const std::string& url) const;
    bool hasRoute(const std::string& url) const;
    const std::unordered_map<std::string, std::string>& getRoutes() const;
    
    // 文件加载器
    void addFileLoader(const std::string& filePath);
    std::vector<std::string> getFileLoaders() const;
    
    // 动态属性条件表达式
    void addDynamicCondition(const std::string& property, const std::string& condition);
    std::vector<std::pair<std::string, std::string>> getDynamicConditions() const;
    
    // 访问者模式
    virtual void accept(NodeVisitor& visitor) override;
    
    // 克隆节点
    virtual std::shared_ptr<BaseNode> clone() const override;
    
    // 节点验证
    virtual bool validate() const override;
    
    // 调试信息
    virtual std::string toString() const override;

private:
    ScriptType m_scriptType;
    std::string m_scriptContent;
    std::unordered_map<std::string, std::string> m_chtljsFunctions;
    std::vector<std::string> m_selectors;
    std::unordered_map<std::string, std::string> m_listeners;
    std::vector<std::tuple<std::string, std::string, std::string, std::string>> m_delegates;
    std::unordered_map<std::string, std::string> m_animations;
    std::unordered_map<std::string, std::string> m_virtualObjects;
    std::unordered_map<std::string, std::string> m_routes;
    std::vector<std::string> m_fileLoaders;
    std::vector<std::pair<std::string, std::string>> m_dynamicConditions;
};

} // namespace CHTL