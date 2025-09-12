#ifndef CHTL_NODE_H
#define CHTL_NODE_H

#include <string>
#include <vector>
#include <map>
#include <memory>

namespace CHTL {

/**
 * 节点类型枚举
 */
enum class NodeType {
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
 * 基础节点类
 */
class BaseNode {
public:
    BaseNode(NodeType type);
    virtual ~BaseNode();
    
    /**
     * 获取节点类型
     * @return 节点类型
     */
    NodeType getType() const;
    
    /**
     * 转换为字符串表示
     * @param indent 缩进级别
     * @return 字符串表示
     */
    virtual std::string toString(int indent = 0) const;
    
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
    void addChild(std::unique_ptr<BaseNode> child);
    
    /**
     * 获取子节点列表
     * @return 子节点列表
     */
    const std::vector<std::unique_ptr<BaseNode>>& getChildren() const;
    
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
    NodeType type;
    std::string name;
    std::string value;
    std::vector<std::unique_ptr<BaseNode>> children;
    std::map<std::string, std::string> attributes;
};

/**
 * 元素节点
 */
class ElementNode : public BaseNode {
public:
    ElementNode(const std::string& tagName);
    virtual ~ElementNode();
    
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
    
    /**
     * 获取子节点
     * @return 子节点列表
     */
    std::vector<std::unique_ptr<BaseNode>>& getChildren();
    const std::vector<std::unique_ptr<BaseNode>>& getChildren() const;
    
    /**
     * 获取属性
     * @return 属性映射
     */
    std::map<std::string, std::string>& getAttributes();
    const std::map<std::string, std::string>& getAttributes() const;

private:
    std::string tagName;
    bool selfClosing;
};

/**
 * 文本节点
 */
class TextNode : public BaseNode {
public:
    TextNode(const std::string& text);
    virtual ~TextNode();
    
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
 * 注释节点
 */
class CommentNode : public BaseNode {
public:
    CommentNode(const std::string& comment);
    virtual ~CommentNode();
    
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
 * 模板节点
 */
class TemplateNode : public BaseNode {
public:
    TemplateNode(const std::string& templateName);
    virtual ~TemplateNode();
    
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
    
    /**
     * 获取属性
     * @return 属性映射
     */
    std::map<std::string, std::string>& getProperties();
    const std::map<std::string, std::string>& getProperties() const;

private:
    std::string templateName;
    std::string templateType;
    std::map<std::string, std::string> properties;
};

/**
 * 自定义节点
 */
class CustomNode : public BaseNode {
public:
    CustomNode(const std::string& customName);
    virtual ~CustomNode();
    
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
    
    /**
     * 获取属性
     * @return 属性映射
     */
    std::map<std::string, std::string>& getProperties();
    const std::map<std::string, std::string>& getProperties() const;

private:
    std::string customName;
    std::string customType;
    std::map<std::string, std::string> properties;
};

/**
 * 样式节点
 */
class StyleNode : public BaseNode {
public:
    StyleNode();
    virtual ~StyleNode();
    
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
 * 脚本节点
 */
class ScriptNode : public BaseNode {
public:
    ScriptNode();
    virtual ~ScriptNode();
    
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
 * 原始节点
 */
class OriginNode : public BaseNode {
public:
    OriginNode(const std::string& originType);
    virtual ~OriginNode();
    
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
 * 导入节点
 */
class ImportNode : public BaseNode {
public:
    ImportNode(const std::string& importPath);
    virtual ~ImportNode();
    
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
 * 配置节点
 */
class ConfigNode : public BaseNode {
public:
    ConfigNode();
    virtual ~ConfigNode();
    
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
 * 命名空间节点
 */
class NamespaceNode : public BaseNode {
public:
    NamespaceNode(const std::string& namespaceName);
    virtual ~NamespaceNode();
    
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
 * 操作符节点
 */
class OperatorNode : public BaseNode {
public:
    OperatorNode(const std::string& operatorType);
    virtual ~OperatorNode();
    
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

#endif // CHTL_NODE_H