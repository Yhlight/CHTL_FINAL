#pragma once

#include "CHTL/CHTLNode.h"
#include <string>
#include <memory>

namespace CHTL {

/**
 * 具体的基础节点类
 * 提供CHTLNode的默认实现
 */
class ConcreteNode : public CHTLNode {
public:
    ConcreteNode(NodeType type);
    virtual ~ConcreteNode() = default;
    
    // 实现纯虚函数
    std::shared_ptr<CHTLNode> clone() const override;
    bool validate() const override;
    std::string generateHTML() const override;
    
    // 设置内容
    void setContent(const std::string& content);
    std::string getContent() const;
    
    // 添加属性
    void addAttribute(const std::string& key, const std::string& value);
    void removeAttribute(const std::string& key);
    std::string getAttribute(const std::string& key) const;
    bool hasAttribute(const std::string& key) const;
    
    // 生成HTML的辅助方法
    std::string generateAttributes() const;
    std::string generateChildren() const;
};

/**
 * 元素节点
 * 表示HTML元素
 */
class ConcreteElementNode : public ConcreteNode {
public:
    ConcreteElementNode(const std::string& tag_name);
    virtual ~ConcreteElementNode() = default;
    
    std::shared_ptr<CHTLNode> clone() const override;
    std::string generateHTML() const override;
    
    void setTagName(const std::string& tag_name);
    std::string getTagName() const;
    
    void setSelfClosing(bool self_closing);
    bool isSelfClosing() const;

private:
    std::string tag_name_;
    bool self_closing_;
};

/**
 * 文本节点
 * 表示纯文本内容
 */
class ConcreteTextNode : public ConcreteNode {
public:
    ConcreteTextNode(const std::string& text);
    virtual ~ConcreteTextNode() = default;
    
    std::shared_ptr<CHTLNode> clone() const override;
    std::string generateHTML() const override;
    
    void setText(const std::string& text);
    std::string getText() const;
    
    // 转义HTML特殊字符
    std::string escapeHTML(const std::string& text) const;
};

/**
 * 注释节点
 * 表示HTML注释
 */
class ConcreteCommentNode : public ConcreteNode {
public:
    ConcreteCommentNode(const std::string& comment);
    virtual ~ConcreteCommentNode() = default;
    
    std::shared_ptr<CHTLNode> clone() const override;
    std::string generateHTML() const override;
    
    void setComment(const std::string& comment);
    std::string getComment() const;
};

/**
 * 模板节点
 * 表示CHTL模板
 */
class TemplateNode : public ConcreteNode {
public:
    enum class TemplateType {
        STYLE,
        ELEMENT,
        VAR,
        CUSTOM
    };
    
    TemplateNode(const std::string& name, TemplateType type);
    virtual ~TemplateNode() = default;
    
    std::shared_ptr<CHTLNode> clone() const override;
    std::string generateHTML() const override;
    
    void setTemplateName(const std::string& name);
    std::string getTemplateName() const;
    
    void setTemplateType(TemplateType type);
    TemplateType getTemplateType() const;
    
    // 样式模板特有方法
    void addCSSProperty(const std::string& property, const std::string& value);
    std::string getCSSProperty(const std::string& property) const;
    bool hasCSSProperty(const std::string& property) const;
    
    // 变量模板特有方法
    void addVariable(const std::string& name, const std::string& value);
    std::string getVariable(const std::string& name) const;
    bool hasVariable(const std::string& name) const;

private:
    std::string template_name_;
    TemplateType template_type_;
    std::unordered_map<std::string, std::string> css_properties_;
    std::unordered_map<std::string, std::string> variables_;
};

/**
 * 导入节点
 * 表示导入语句
 */
class ImportNode : public ConcreteNode {
public:
    ImportNode(const std::string& import_path);
    virtual ~ImportNode() = default;
    
    std::shared_ptr<CHTLNode> clone() const override;
    std::string generateHTML() const override;
    
    void setImportPath(const std::string& path);
    std::string getImportPath() const;
    
    void setImportType(const std::string& type);
    std::string getImportType() const;

private:
    std::string import_path_;
    std::string import_type_;
};

/**
 * 命名空间节点
 * 表示命名空间声明
 */
class NamespaceNode : public ConcreteNode {
public:
    NamespaceNode(const std::string& namespace_name);
    virtual ~NamespaceNode() = default;
    
    std::shared_ptr<CHTLNode> clone() const override;
    std::string generateHTML() const override;
    
    void setNamespaceName(const std::string& name);
    std::string getNamespaceName() const;

private:
    std::string namespace_name_;
};

/**
 * 配置节点
 * 表示配置声明
 */
class ConfigurationNode : public ConcreteNode {
public:
    ConfigurationNode(const std::string& config_name);
    virtual ~ConfigurationNode() = default;
    
    std::shared_ptr<CHTLNode> clone() const override;
    std::string generateHTML() const override;
    
    void setConfigName(const std::string& name);
    std::string getConfigName() const;
    
    void addConfigValue(const std::string& key, const std::string& value);
    std::string getConfigValue(const std::string& key) const;
    bool hasConfigValue(const std::string& key) const;
    bool validate() const override;

private:
    std::string config_name_;
    std::unordered_map<std::string, std::string> config_values_;
};

/**
 * 原始内容节点
 * 表示原始HTML/JS/CSS内容
 */
class OriginNode : public ConcreteNode {
public:
    enum class OriginType {
        HTML,
        CSS,
        JAVASCRIPT
    };
    
    OriginNode(const std::string& content, OriginType type);
    virtual ~OriginNode() = default;
    
    std::shared_ptr<CHTLNode> clone() const override;
    std::string generateHTML() const override;
    
    void setOriginType(OriginType type);
    OriginType getOriginType() const;
    
    void setOriginContent(const std::string& content);
    std::string getOriginContent() const;

private:
    OriginType origin_type_;
    std::string origin_content_;
};

} // namespace CHTL