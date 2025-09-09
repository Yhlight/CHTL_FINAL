#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <variant>

namespace CHTL_JS {

// 前向声明
class CHTLJSNode;

/**
 * @brief CHTL JS节点类型枚举
 */
enum class CHTLJSNodeType {
    // 基础节点
    ROOT,           // 根节点
    SCRIPT,         // 脚本节点
    
    // 函数相关
    FUNCTION,       // 函数节点
    FUNCTION_CALL,  // 函数调用节点
    PARAMETER,      // 参数节点
    
    // 对象相关
    OBJECT,         // 对象节点
    PROPERTY,       // 属性节点
    METHOD,         // 方法节点
    
    // 虚对象相关
    VIRTUAL_OBJECT, // 虚对象节点
    VIRTUAL_PROPERTY, // 虚对象属性节点
    
    // 事件相关
    EVENT_LISTENER, // 事件监听器节点
    EVENT_DELEGATE, // 事件委托节点
    
    // 动画相关
    ANIMATION,      // 动画节点
    ANIMATION_KEYFRAME, // 动画关键帧节点
    
    // 路由相关
    ROUTER,         // 路由节点
    ROUTE,          // 路由规则节点
    
    // 文件加载相关
    FILE_LOADER,    // 文件加载器节点
    
    // 工具相关
    UTIL_EXPRESSION, // util表达式节点
    CHANGE_EXPRESSION, // change表达式节点
    THEN_EXPRESSION, // then表达式节点
    
    // 选择器相关
    SELECTOR,       // 选择器节点
    ENHANCED_SELECTOR, // 增强选择器节点
    
    // 表达式相关
    EXPRESSION,     // 表达式节点
    CONDITIONAL,    // 条件表达式节点
    LOGICAL,        // 逻辑表达式节点
    COMPARISON,     // 比较表达式节点
    ARITHMETIC,     // 算术表达式节点
};

/**
 * @brief 属性值类型
 */
using CHTLJSAttributeValue = std::variant<std::string, int, double, bool, std::vector<std::string>>;

/**
 * @brief 属性映射表
 */
using CHTLJSAttributeMap = std::unordered_map<std::string, CHTLJSAttributeValue>;

/**
 * @brief CHTL JS AST节点基类
 */
class CHTLJSNode {
public:
    CHTLJSNode(CHTLJSNodeType type, const std::string& name = "");
    virtual ~CHTLJSNode() = default;

    // 基本属性
    CHTLJSNodeType getType() const { return m_type; }
    const std::string& getName() const { return m_name; }
    void setName(const std::string& name) { m_name = name; }
    
    // 子节点管理
    void addChild(std::shared_ptr<CHTLJSNode> child);
    void removeChild(std::shared_ptr<CHTLJSNode> child);
    const std::vector<std::shared_ptr<CHTLJSNode>>& getChildren() const;
    std::vector<std::shared_ptr<CHTLJSNode>>& getChildren();
    
    // 属性管理
    void setAttribute(const std::string& key, const CHTLJSAttributeValue& value);
    CHTLJSAttributeValue getAttribute(const std::string& key) const;
    bool hasAttribute(const std::string& key) const;
    void removeAttribute(const std::string& key);
    const CHTLJSAttributeMap& getAttributes() const;
    
    // 文本内容
    void setTextContent(const std::string& text);
    const std::string& getTextContent() const;
    
    // 位置信息
    void setPosition(size_t line, size_t column);
    size_t getLine() const { return m_line; }
    size_t getColumn() const { return m_column; }
    
    // 虚函数 - 子类可重写
    virtual std::string toString() const;
    virtual std::string toJavaScript() const;
    
    // 查找功能
    std::shared_ptr<CHTLJSNode> findChild(const std::string& name) const;
    std::vector<std::shared_ptr<CHTLJSNode>> findChildren(const std::string& name) const;
    std::shared_ptr<CHTLJSNode> findChildByType(CHTLJSNodeType type) const;
    std::vector<std::shared_ptr<CHTLJSNode>> findChildrenByType(CHTLJSNodeType type) const;

protected:
    CHTLJSNodeType m_type;
    std::string m_name;
    std::vector<std::shared_ptr<CHTLJSNode>> m_children;
    CHTLJSAttributeMap m_attributes;
    std::string m_textContent;
    size_t m_line;
    size_t m_column;
};

/**
 * @brief 函数节点
 */
class CHTLJSFunctionNode : public CHTLJSNode {
public:
    CHTLJSFunctionNode(const std::string& name);
    virtual ~CHTLJSFunctionNode() = default;
    
    void addParameter(const std::string& param);
    const std::vector<std::string>& getParameters() const;
    
    void setReturnType(const std::string& type);
    const std::string& getReturnType() const;
    
    virtual std::string toJavaScript() const override;
    
private:
    std::vector<std::string> m_parameters;
    std::string m_returnType;
};

/**
 * @brief 虚对象节点
 */
class CHTLJSVirtualObjectNode : public CHTLJSNode {
public:
    CHTLJSVirtualObjectNode(const std::string& name);
    virtual ~CHTLJSVirtualObjectNode() = default;
    
    void addProperty(const std::string& name, const std::string& value);
    const std::unordered_map<std::string, std::string>& getProperties() const;
    
    void addMethod(const std::string& name, const std::string& definition);
    const std::unordered_map<std::string, std::string>& getMethods() const;
    
    virtual std::string toJavaScript() const override;
    
private:
    std::unordered_map<std::string, std::string> m_properties;
    std::unordered_map<std::string, std::string> m_methods;
};

/**
 * @brief 事件监听器节点
 */
class CHTLJSEventListenerNode : public CHTLJSNode {
public:
    CHTLJSEventListenerNode();
    virtual ~CHTLJSEventListenerNode() = default;
    
    void setEventType(const std::string& eventType);
    const std::string& getEventType() const;
    
    void setHandler(const std::string& handler);
    const std::string& getHandler() const;
    
    void setTarget(const std::string& target);
    const std::string& getTarget() const;
    
    virtual std::string toJavaScript() const override;
    
private:
    std::string m_eventType;
    std::string m_handler;
    std::string m_target;
};

/**
 * @brief 动画节点
 */
class CHTLJSAnimationNode : public CHTLJSNode {
public:
    CHTLJSAnimationNode(const std::string& name);
    virtual ~CHTLJSAnimationNode() = default;
    
    void setTarget(const std::string& target);
    const std::string& getTarget() const;
    
    void setDuration(int duration);
    int getDuration() const;
    
    void setEasing(const std::string& easing);
    const std::string& getEasing() const;
    
    void addKeyframe(float time, const std::unordered_map<std::string, std::string>& properties);
    const std::vector<std::pair<float, std::unordered_map<std::string, std::string>>>& getKeyframes() const;
    
    virtual std::string toJavaScript() const override;
    
private:
    std::string m_target;
    int m_duration;
    std::string m_easing;
    std::vector<std::pair<float, std::unordered_map<std::string, std::string>>> m_keyframes;
};

/**
 * @brief 路由节点
 */
class CHTLJSRouterNode : public CHTLJSNode {
public:
    CHTLJSRouterNode();
    virtual ~CHTLJSRouterNode() = default;
    
    void addRoute(const std::string& url, const std::string& page);
    const std::unordered_map<std::string, std::string>& getRoutes() const;
    
    void setMode(const std::string& mode);
    const std::string& getMode() const;
    
    void setRoot(const std::string& root);
    const std::string& getRoot() const;
    
    virtual std::string toJavaScript() const override;
    
private:
    std::unordered_map<std::string, std::string> m_routes;
    std::string m_mode;
    std::string m_root;
};

/**
 * @brief 选择器节点
 */
class CHTLJSSelectorNode : public CHTLJSNode {
public:
    CHTLJSSelectorNode(const std::string& selector);
    virtual ~CHTLJSSelectorNode() = default;
    
    const std::string& getSelector() const;
    void setSelector(const std::string& selector);
    
    virtual std::string toJavaScript() const override;
    
private:
    std::string m_selector;
};

} // namespace CHTL_JS