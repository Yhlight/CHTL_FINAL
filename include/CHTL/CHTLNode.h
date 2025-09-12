#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <map>

namespace CHTL {

/**
 * CHTL节点基类
 * 所有CHTL语法元素的基类
 */
class CHTLNode {
public:
    enum class NodeType {
        ELEMENT,        // HTML元素
        TEXT,           // 文本节点
        COMMENT,        // 注释节点
        TEMPLATE,       // 模板节点
        CUSTOM,         // 自定义节点
        STYLE,          // 样式节点
        SCRIPT,         // 脚本节点
        ORIGIN,         // 原始嵌入节点
        IMPORT,         // 导入节点
        CONFIG,         // 配置节点
        NAMESPACE,      // 命名空间节点
        OPERATOR        // 操作符节点
    };

    CHTLNode(NodeType type);
    virtual ~CHTLNode() = default;

    // 节点类型
    NodeType getType() const;
    
    // 属性管理
    void setAttribute(const std::string& key, const std::string& value);
    std::string getAttribute(const std::string& key) const;
    bool hasAttribute(const std::string& key) const;
    void removeAttribute(const std::string& key);
    
    // 子节点管理
    void addChild(std::shared_ptr<CHTLNode> child);
    void removeChild(std::shared_ptr<CHTLNode> child);
    std::vector<std::shared_ptr<CHTLNode>> getChildren() const;
    std::shared_ptr<CHTLNode> getChild(size_t index) const;
    size_t getChildCount() const;
    
    // 父节点管理
    void setParent(std::shared_ptr<CHTLNode> parent);
    std::shared_ptr<CHTLNode> getParent() const;
    
    // 内容管理
    void setContent(const std::string& content);
    std::string getContent() const;
    
    // 位置信息
    void setPosition(size_t line, size_t column);
    size_t getLine() const;
    size_t getColumn() const;
    
    // 克隆
    virtual std::shared_ptr<CHTLNode> clone() const = 0;
    
    // 验证
    virtual bool validate() const = 0;
    
    // 生成HTML
    virtual std::string generateHTML() const = 0;

protected:
    NodeType type_;
    std::unordered_map<std::string, std::string> attributes_;
    std::vector<std::shared_ptr<CHTLNode>> children_;
    std::weak_ptr<CHTLNode> parent_;
    std::string content_;
    size_t line_;
    size_t column_;
};

/**
 * 元素节点
 * 表示HTML元素
 */
class ElementNode : public CHTLNode {
public:
    ElementNode(const std::string& tagName);
    
    std::string getTagName() const;
    void setTagName(const std::string& tagName);
    
    bool isSelfClosing() const;
    void setSelfClosing(bool selfClosing);
    
    std::shared_ptr<CHTLNode> clone() const override;
    bool validate() const override;
    std::string generateHTML() const override;

private:
    std::string tag_name_;
    bool self_closing_;
};

/**
 * 文本节点
 * 表示文本内容
 */
class TextNode : public CHTLNode {
public:
    TextNode(const std::string& text);
    
    std::shared_ptr<CHTLNode> clone() const override;
    bool validate() const override;
    std::string generateHTML() const override;
};

/**
 * 注释节点
 * 表示注释内容
 */
class CommentNode : public CHTLNode {
public:
    enum class CommentType {
        SINGLE_LINE,    // //
        MULTI_LINE,     // /**/
        GENERATOR       // --
    };
    
    CommentNode(const std::string& content, CommentType type);
    
    CommentType getCommentType() const;
    void setCommentType(CommentType type);
    
    std::shared_ptr<CHTLNode> clone() const override;
    bool validate() const override;
    std::string generateHTML() const override;

private:
    CommentType comment_type_;
};

/**
 * 样式节点
 * 表示样式块
 */
class StyleNode : public CHTLNode {
public:
    enum class StyleType {
        LOCAL,          // 局部样式
        GLOBAL,         // 全局样式
        TEMPLATE        // 模板样式
    };
    
    StyleNode(StyleType type);
    
    StyleType getStyleType() const;
    void setStyleType(StyleType type);
    
    // CSS属性管理
    void addCSSProperty(const std::string& property, const std::string& value);
    std::string getCSSProperty(const std::string& property) const;
    bool hasCSSProperty(const std::string& property) const;
    void removeCSSProperty(const std::string& property);
    
    // 选择器管理
    void addSelector(const std::string& selector);
    std::vector<std::string> getSelectors() const;
    
    std::shared_ptr<CHTLNode> clone() const override;
    bool validate() const override;
    std::string generateHTML() const override;

private:
    StyleType style_type_;
    std::unordered_map<std::string, std::string> css_properties_;
    std::vector<std::string> selectors_;
};

/**
 * 脚本节点
 * 表示脚本块
 */
class ScriptNode : public CHTLNode {
public:
    enum class ScriptType {
        LOCAL,          // 局部脚本
        GLOBAL,         // 全局脚本
        CHTL_JS         // CHTL JS脚本
    };
    
    ScriptNode(ScriptType type);
    
    ScriptType getScriptType() const;
    void setScriptType(ScriptType type);
    
    std::shared_ptr<CHTLNode> clone() const override;
    bool validate() const override;
    std::string generateHTML() const override;

private:
    ScriptType script_type_;
};

} // namespace CHTL