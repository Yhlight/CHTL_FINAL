#pragma once

#include "CHTLCommon.h"
#include <memory>
#include <vector>

namespace CHTL {

// 基础节点类
class CHTLNode {
public:
    CHTLNode(NodeType type, const Position& pos = Position());
    virtual ~CHTLNode() = default;
    
    // 获取节点类型
    NodeType getType() const { return type_; }
    
    // 获取位置信息
    const Position& getPosition() const { return position_; }
    void setPosition(const Position& pos) { position_ = pos; }
    
    // 获取/设置父节点
    std::shared_ptr<CHTLNode> getParent() const { return parent_.lock(); }
    void setParent(std::shared_ptr<CHTLNode> parent) { parent_ = parent; }
    
    // 子节点管理
    const std::vector<std::shared_ptr<CHTLNode>>& getChildren() const { return children_; }
    void addChild(std::shared_ptr<CHTLNode> child);
    void removeChild(std::shared_ptr<CHTLNode> child);
    void clearChildren();
    
    // 属性管理
    const AttributeList& getAttributes() const { return attributes_; }
    void addAttribute(const Attribute& attr);
    void setAttribute(const String& name, const String& value, bool quoted = false);
    std::optional<String> getAttribute(const String& name) const;
    void removeAttribute(const String& name);
    
    // 文本内容
    const String& getText() const { return text_; }
    void setText(const String& text) { text_ = text; }
    
    // 标签名（用于元素节点）
    const String& getTagName() const { return tagName_; }
    void setTagName(const String& name) { tagName_ = name; }
    
    // 是否为自闭合标签
    bool isSelfClosing() const { return selfClosing_; }
    void setSelfClosing(bool closing) { selfClosing_ = closing; }
    
    // 克隆节点
    virtual std::shared_ptr<CHTLNode> clone() const;
    
    // 访问者模式
    virtual void accept(class CHTLVisitor& visitor) = 0;

protected:
    NodeType type_;
    Position position_;
    std::weak_ptr<CHTLNode> parent_;
    std::vector<std::shared_ptr<CHTLNode>> children_;
    AttributeList attributes_;
    String text_;
    String tagName_;
    bool selfClosing_ = false;
};

// 文本节点
class TextNode : public CHTLNode {
public:
    TextNode(const String& text = "", const Position& pos = Position());
    
    void accept(class CHTLVisitor& visitor) override;
    std::shared_ptr<CHTLNode> clone() const override;
};

// 元素节点
class ElementNode : public CHTLNode {
public:
    ElementNode(const String& tagName = "", const Position& pos = Position());
    
    void accept(class CHTLVisitor& visitor) override;
    std::shared_ptr<CHTLNode> clone() const override;
    
    // 查找子元素
    std::vector<std::shared_ptr<ElementNode>> findElements(const String& tagName) const;
    std::shared_ptr<ElementNode> findElement(const String& tagName) const;
    std::vector<std::shared_ptr<ElementNode>> findElementsByClass(const String& className) const;
    std::shared_ptr<ElementNode> findElementById(const String& id) const;
};

// 注释节点
class CommentNode : public CHTLNode {
public:
    CommentNode(const String& content = "", const Position& pos = Position());
    
    void accept(class CHTLVisitor& visitor) override;
    std::shared_ptr<CHTLNode> clone() const override;
    
    // 注释类型
    enum class CommentType {
        SingleLine,    // //
        MultiLine,     // /* */
        Generator      // --
    };
    
    CommentType getCommentType() const { return commentType_; }
    void setCommentType(CommentType type) { commentType_ = type; }

private:
    CommentType commentType_ = CommentType::SingleLine;
};

// 样式节点
class StyleNode : public CHTLNode {
public:
    StyleNode(const Position& pos = Position());
    
    void accept(class CHTLVisitor& visitor) override;
    std::shared_ptr<CHTLNode> clone() const override;
    
    // 样式内容
    const String& getStyleContent() const { return styleContent_; }
    void setStyleContent(const String& content) { styleContent_ = content; }
    
    // 是否为局部样式
    bool isLocal() const { return isLocal_; }
    void setLocal(bool local) { isLocal_ = local; }

private:
    String styleContent_;
    bool isLocal_ = false;
};

// 脚本节点
class ScriptNode : public CHTLNode {
public:
    ScriptNode(const Position& pos = Position());
    
    void accept(class CHTLVisitor& visitor) override;
    std::shared_ptr<CHTLNode> clone() const override;
    
    // 脚本内容
    const String& getScriptContent() const { return scriptContent_; }
    void setScriptContent(const String& content) { scriptContent_ = content; }
    
    // 是否为局部脚本
    bool isLocal() const { return isLocal_; }
    void setLocal(bool local) { isLocal_ = local; }

private:
    String scriptContent_;
    bool isLocal_ = false;
};

// 模板节点基类
class TemplateNode : public CHTLNode {
public:
    TemplateNode(NodeType type, const String& name, const Position& pos = Position());
    
    const String& getTemplateName() const { return templateName_; }
    void setTemplateName(const String& name) { templateName_ = name; }
    
    // 模板继承
    const StringList& getInherits() const { return inherits_; }
    void addInherit(const String& name) { inherits_.push_back(name); }

protected:
    String templateName_;
    StringList inherits_;
};

// 样式组模板
class TemplateStyleNode : public TemplateNode {
public:
    TemplateStyleNode(const String& name, const Position& pos = Position());
    
    void accept(class CHTLVisitor& visitor) override;
    std::shared_ptr<CHTLNode> clone() const override;
};

// 元素模板
class TemplateElementNode : public TemplateNode {
public:
    TemplateElementNode(const String& name, const Position& pos = Position());
    
    void accept(class CHTLVisitor& visitor) override;
    std::shared_ptr<CHTLNode> clone() const override;
};

// 变量组模板
class TemplateVarNode : public TemplateNode {
public:
    TemplateVarNode(const String& name, const Position& pos = Position());
    
    void accept(class CHTLVisitor& visitor) override;
    std::shared_ptr<CHTLNode> clone() const override;
};

// 自定义节点基类
class CustomNode : public CHTLNode {
public:
    CustomNode(NodeType type, const String& name, const Position& pos = Position());
    
    const String& getCustomName() const { return customName_; }
    void setCustomName(const String& name) { customName_ = name; }
    
    // 特例化操作
    const StringList& getDeletes() const { return deletes_; }
    void addDelete(const String& name) { deletes_.push_back(name); }
    
    const StringList& getInserts() const { return inserts_; }
    void addInsert(const String& name) { inserts_.push_back(name); }

protected:
    String customName_;
    StringList deletes_;
    StringList inserts_;
};

// 自定义样式组
class CustomStyleNode : public CustomNode {
public:
    CustomStyleNode(const String& name, const Position& pos = Position());
    
    void accept(class CHTLVisitor& visitor) override;
    std::shared_ptr<CHTLNode> clone() const override;
};

// 自定义元素
class CustomElementNode : public CustomNode {
public:
    CustomElementNode(const String& name, const Position& pos = Position());
    
    void accept(class CHTLVisitor& visitor) override;
    std::shared_ptr<CHTLNode> clone() const override;
};

// 自定义变量组
class CustomVarNode : public CustomNode {
public:
    CustomVarNode(const String& name, const Position& pos = Position());
    
    void accept(class CHTLVisitor& visitor) override;
    std::shared_ptr<CHTLNode> clone() const override;
};

// 原始嵌入节点
class OriginNode : public CHTLNode {
public:
    OriginNode(const String& type, const String& name = "", const Position& pos = Position());
    
    void accept(class CHTLVisitor& visitor) override;
    std::shared_ptr<CHTLNode> clone() const override;
    
    const String& getOriginType() const { return originType_; }
    const String& getOriginName() const { return originName_; }
    const String& getOriginContent() const { return originContent_; }
    void setOriginContent(const String& content) { originContent_ = content; }

private:
    String originType_;
    String originName_;
    String originContent_;
};

// 导入节点
class ImportNode : public CHTLNode {
public:
    ImportNode(const String& type, const String& path, const String& alias = "", const Position& pos = Position());
    
    void accept(class CHTLVisitor& visitor) override;
    std::shared_ptr<CHTLNode> clone() const override;
    
    const String& getImportType() const { return importType_; }
    const String& getImportPath() const { return importPath_; }
    const String& getImportAlias() const { return importAlias_; }

private:
    String importType_;
    String importPath_;
    String importAlias_;
};

// 配置节点
class ConfigNode : public CHTLNode {
public:
    ConfigNode(const String& name = "", const Position& pos = Position());
    
    void accept(class CHTLVisitor& visitor) override;
    std::shared_ptr<CHTLNode> clone() const override;
    
    const String& getConfigName() const { return configName_; }
    const ConfigOptions& getConfigOptions() const { return configOptions_; }
    void setConfigOptions(const ConfigOptions& options) { configOptions_ = options; }

private:
    String configName_;
    ConfigOptions configOptions_;
};

// 命名空间节点
class NamespaceNode : public CHTLNode {
public:
    NamespaceNode(const String& name, const Position& pos = Position());
    
    void accept(class CHTLVisitor& visitor) override;
    std::shared_ptr<CHTLNode> clone() const override;
    
    const String& getNamespaceName() const { return namespaceName_; }
    const String& getParentNamespace() const { return parentNamespace_; }
    void setParentNamespace(const String& parent) { parentNamespace_ = parent; }

private:
    String namespaceName_;
    String parentNamespace_;
};

// 操作符节点
class OperatorNode : public CHTLNode {
public:
    enum class OperatorType {
        Delete,
        Insert,
        Use,
        Inherit
    };
    
    OperatorNode(OperatorType type, const Position& pos = Position());
    
    void accept(class CHTLVisitor& visitor) override;
    std::shared_ptr<CHTLNode> clone() const override;
    
    OperatorType getOperatorType() const { return operatorType_; }

private:
    OperatorType operatorType_;
};

} // namespace CHTL