#ifndef CHTL_JS_NODE_H
#define CHTL_JS_NODE_H

#include <string>
#include <vector>
#include <map>
#include <memory>

namespace CHTL {

/**
 * CHTL JS节点类型枚举
 */
enum class CHTLJSNodeType {
    ELEMENT,        // 元素节点
    TEXT,           // 文本节点
    COMMENT,        // 注释节点
    TEMPLATE,       // 模板节点
    CUSTOM,         // 自定义节点
    STYLE,          // 样式节点
    SCRIPT,         // 脚本节点
    ORIGIN,         // 原始节点
    IMPORT,         // 导入节点
    CONFIG,         // 配置节点
    NAMESPACE,      // 命名空间节点
    OPERATOR        // 操作符节点
};

/**
 * CHTL JS基础节点类
 */
class CHTLJSBaseNode {
public:
    CHTLJSBaseNode(CHTLJSNodeType type);
    virtual ~CHTLJSBaseNode();
    
    /**
     * 获取节点类型
     * @return 节点类型
     */
    CHTLJSNodeType getType() const;
    
    /**
     * 设置节点名称
     * @param name 名称
     */
    void setName(const std::string& name);
    
    /**
     * 获取节点名称
     * @return 名称
     */
    std::string getName() const;
    
    /**
     * 转换为字符串表示
     * @return 字符串表示
     */
    virtual std::string toString() const;
    
    /**
     * 设置节点值
     * @param value 值
     */
    void setValue(const std::string& value);
    
    /**
     * 获取节点值
     * @return 值
     */
    std::string getValue() const;
    
    /**
     * 添加子节点
     * @param child 子节点
     */
    void addChild(std::unique_ptr<CHTLJSBaseNode> child);
    
    /**
     * 获取子节点列表
     * @return 子节点列表
     */
    const std::vector<std::unique_ptr<CHTLJSBaseNode>>& getChildren() const;
    
    /**
     * 设置属性
     * @param key 属性名
     * @param value 属性值
     */
    void setAttribute(const std::string& key, const std::string& value);
    
    /**
     * 获取属性
     * @param key 属性名
     * @return 属性值
     */
    std::string getAttribute(const std::string& key) const;
    
    /**
     * 获取所有属性
     * @return 属性映射
     */
    const std::map<std::string, std::string>& getAttributes() const;
    
    /**
     * 检查是否有属性
     * @param key 属性名
     * @return 是否有属性
     */
    bool hasAttribute(const std::string& key) const;
    
    /**
     * 清空子节点
     */
    void clearChildren();
    
    /**
     * 清空属性
     */
    void clearAttributes();

protected:
    CHTLJSNodeType type;
    std::string name;
    std::string value;
    std::vector<std::unique_ptr<CHTLJSBaseNode>> children;
    std::map<std::string, std::string> attributes;
};

/**
 * CHTL JS元素节点
 */
class CHTLJSElementNode : public CHTLJSBaseNode {
public:
    CHTLJSElementNode(const std::string& tagName);
    virtual ~CHTLJSElementNode();
    
    /**
     * 获取标签名
     * @return 标签名
     */
    std::string getTagName() const;
    
    /**
     * 设置标签名
     * @param tagName 标签名
     */
    void setTagName(const std::string& tagName);
    
    /**
     * 检查是否是自闭合标签
     * @return 是否自闭合
     */
    bool isSelfClosing() const;
    
    /**
     * 设置自闭合
     * @param selfClosing 是否自闭合
     */
    void setSelfClosing(bool selfClosing);

private:
    std::string tagName;
    bool selfClosing;
};

/**
 * CHTL JS文本节点
 */
class CHTLJSTextNode : public CHTLJSBaseNode {
public:
    CHTLJSTextNode(const std::string& text);
    virtual ~CHTLJSTextNode();
    
    /**
     * 获取文本内容
     * @return 文本内容
     */
    std::string getText() const;
    
    /**
     * 设置文本内容
     * @param text 文本内容
     */
    void setText(const std::string& text);
};

/**
 * CHTL JS注释节点
 */
class CHTLJSCommentNode : public CHTLJSBaseNode {
public:
    CHTLJSCommentNode(const std::string& comment);
    virtual ~CHTLJSCommentNode();
    
    /**
     * 获取注释内容
     * @return 注释内容
     */
    std::string getComment() const;
    
    /**
     * 设置注释内容
     * @param comment 注释内容
     */
    void setComment(const std::string& comment);
};

/**
 * CHTL JS模板节点
 */
class CHTLJSTemplateNode : public CHTLJSBaseNode {
public:
    CHTLJSTemplateNode(const std::string& templateName);
    virtual ~CHTLJSTemplateNode();
    
    /**
     * 获取模板名称
     * @return 模板名称
     */
    std::string getTemplateName() const;
    
    /**
     * 设置模板名称
     * @param templateName 模板名称
     */
    void setTemplateName(const std::string& templateName);
    
    /**
     * 获取模板类型
     * @return 模板类型
     */
    std::string getTemplateType() const;
    
    /**
     * 设置模板类型
     * @param templateType 模板类型
     */
    void setTemplateType(const std::string& templateType);
};

/**
 * CHTL JS自定义节点
 */
class CHTLJSCustomNode : public CHTLJSBaseNode {
public:
    CHTLJSCustomNode(const std::string& customName);
    virtual ~CHTLJSCustomNode();
    
    /**
     * 获取自定义名称
     * @return 自定义名称
     */
    std::string getCustomName() const;
    
    /**
     * 设置自定义名称
     * @param customName 自定义名称
     */
    void setCustomName(const std::string& customName);
};

/**
 * CHTL JS样式节点
 */
class CHTLJSStyleNode : public CHTLJSBaseNode {
public:
    CHTLJSStyleNode();
    virtual ~CHTLJSStyleNode();
    
    /**
     * 添加样式属性
     * @param property 属性名
     * @param value 属性值
     */
    void addProperty(const std::string& property, const std::string& value);
    
    /**
     * 获取样式属性
     * @param property 属性名
     * @return 属性值
     */
    std::string getProperty(const std::string& property) const;
    
    /**
     * 获取所有样式属性
     * @return 属性映射
     */
    const std::map<std::string, std::string>& getProperties() const;
    
    /**
     * 检查是否有样式属性
     * @param property 属性名
     * @return 是否有属性
     */
    bool hasProperty(const std::string& property) const;
    
    /**
     * 清空样式属性
     */
    void clearProperties();

private:
    std::map<std::string, std::string> properties;
};

/**
 * CHTL JS脚本节点
 */
class CHTLJSScriptNode : public CHTLJSBaseNode {
public:
    CHTLJSScriptNode();
    virtual ~CHTLJSScriptNode();
    
    /**
     * 获取脚本内容
     * @return 脚本内容
     */
    std::string getScript() const;
    
    /**
     * 设置脚本内容
     * @param script 脚本内容
     */
    void setScript(const std::string& script);
};

/**
 * CHTL JS原始节点
 */
class CHTLJSOriginNode : public CHTLJSBaseNode {
public:
    CHTLJSOriginNode(const std::string& originType);
    virtual ~CHTLJSOriginNode();
    
    /**
     * 获取原始类型
     * @return 原始类型
     */
    std::string getOriginType() const;
    
    /**
     * 设置原始类型
     * @param originType 原始类型
     */
    void setOriginType(const std::string& originType);
    
    /**
     * 获取原始内容
     * @return 原始内容
     */
    std::string getOriginContent() const;
    
    /**
     * 设置原始内容
     * @param content 原始内容
     */
    void setOriginContent(const std::string& content);

private:
    std::string originType;
    std::string originContent;
};

/**
 * CHTL JS导入节点
 */
class CHTLJSImportNode : public CHTLJSBaseNode {
public:
    CHTLJSImportNode(const std::string& importPath);
    virtual ~CHTLJSImportNode();
    
    /**
     * 获取导入路径
     * @return 导入路径
     */
    std::string getPath() const;
    
    /**
     * 设置导入路径
     * @param path 导入路径
     */
    void setPath(const std::string& path);
    
    /**
     * 获取导入类型
     * @return 导入类型
     */
    std::string getImportType() const;
    
    /**
     * 设置导入类型
     * @param importType 导入类型
     */
    void setImportType(const std::string& importType);
};

/**
 * CHTL JS配置节点
 */
class CHTLJSConfigNode : public CHTLJSBaseNode {
public:
    CHTLJSConfigNode();
    virtual ~CHTLJSConfigNode();
    
    /**
     * 添加配置项
     * @param key 配置键
     * @param value 配置值
     */
    void addConfig(const std::string& key, const std::string& value);
    
    /**
     * 获取配置项
     * @param key 配置键
     * @return 配置值
     */
    std::string getConfig(const std::string& key) const;
    
    /**
     * 获取所有配置
     * @return 配置映射
     */
    const std::map<std::string, std::string>& getConfigs() const;
    
    /**
     * 检查是否有配置
     * @param key 配置键
     * @return 是否有配置
     */
    bool hasConfig(const std::string& key) const;

private:
    std::map<std::string, std::string> configs;
};

/**
 * CHTL JS命名空间节点
 */
class CHTLJSNamespaceNode : public CHTLJSBaseNode {
public:
    CHTLJSNamespaceNode(const std::string& namespaceName);
    virtual ~CHTLJSNamespaceNode();
    
    /**
     * 获取命名空间名称
     * @return 命名空间名称
     */
    std::string getNamespaceName() const;
    
    /**
     * 设置命名空间名称
     * @param namespaceName 命名空间名称
     */
    void setNamespaceName(const std::string& namespaceName);
};

/**
 * CHTL JS操作符节点
 */
class CHTLJSOperatorNode : public CHTLJSBaseNode {
public:
    CHTLJSOperatorNode(const std::string& operatorType);
    virtual ~CHTLJSOperatorNode();
    
    /**
     * 获取操作符类型
     * @return 操作符类型
     */
    std::string getOperatorType() const;
    
    /**
     * 设置操作符类型
     * @param operatorType 操作符类型
     */
    void setOperatorType(const std::string& operatorType);
};

} // namespace CHTL

#endif // CHTL_JS_NODE_H