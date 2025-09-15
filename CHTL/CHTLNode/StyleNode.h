#ifndef CHTL_STYLENODE_H
#define CHTL_STYLENODE_H

#include "CHTLBaseNode.h"

namespace CHTL {

/**
 * 样式节点
 */
class StyleNode : public CHTLBaseNode {
public:
    StyleNode();
    ~StyleNode();
    
    void addStyleProperty(const std::string& property, const std::string& value);
    const std::map<std::string, std::string>& getStyleProperties() const;
    
    void setSelector(const std::string& selector);
    const std::string& getSelector() const;
    bool hasSelector() const;
    
    void setInlineStyle(bool inline_style);
    bool isInlineStyle() const;
    
    void addClassSelector(const std::string& className);
    void addIdSelector(const std::string& id);
    void addPseudoSelector(const std::string& pseudo);
    
    const std::vector<std::string>& getClassSelectors() const;
    const std::vector<std::string>& getIdSelectors() const;
    const std::vector<std::string>& getPseudoSelectors() const;
    
    std::string generateCode() const override;
    std::string toString() const override;

private:
    std::map<std::string, std::string> style_properties_;
    std::string selector_;
    bool has_selector_;
    bool inline_style_;
    std::vector<std::string> class_selectors_;
    std::vector<std::string> id_selectors_;
    std::vector<std::string> pseudo_selectors_;
};

/**
 * 脚本节点
 */
class ScriptNode : public CHTLBaseNode {
public:
    ScriptNode();
    ~ScriptNode();
    
    void setScriptContent(const std::string& content);
    const std::string& getScriptContent() const;
    
    void setLocalScript(bool local);
    bool isLocalScript() const;
    
    void addCHTLJSSyntax(const std::string& syntax);
    const std::vector<std::string>& getCHTLJSSyntaxes() const;
    
    std::string generateCode() const override;
    std::string toString() const override;

private:
    std::string script_content_;
    bool local_script_;
    std::vector<std::string> chtljs_syntaxes_;
};

/**
 * 原始嵌入节点
 */
class OriginNode : public CHTLBaseNode {
public:
    OriginNode(NodeType type, const std::string& name = "");
    ~OriginNode();
    
    void setName(const std::string& name);
    const std::string& getName() const;
    bool hasName() const;
    
    void setContent(const std::string& content);
    const std::string& getContent() const;
    
    std::string generateCode() const override;
    std::string toString() const override;

private:
    std::string name_;
    std::string content_;
    bool has_name_;
};

/**
 * 导入节点
 */
class ImportNode : public CHTLBaseNode {
public:
    ImportNode(NodeType type);
    ~ImportNode();
    
    void setPath(const std::string& path);
    const std::string& getPath() const;
    
    void setAlias(const std::string& alias);
    const std::string& getAlias() const;
    bool hasAlias() const;
    
    void setNamespace(const std::string& namespace_name);
    const std::string& getNamespace() const;
    bool hasNamespace() const;
    
    std::string generateCode() const override;
    std::string toString() const override;

private:
    std::string path_;
    std::string alias_;
    std::string namespace_name_;
    bool has_alias_;
    bool has_namespace_;
};

/**
 * 命名空间节点
 */
class NamespaceNode : public CHTLBaseNode {
public:
    NamespaceNode(const std::string& name);
    ~NamespaceNode();
    
    void setName(const std::string& name);
    const std::string& getName() const;
    
    std::string generateCode() const override;
    std::string toString() const override;

private:
    std::string name_;
};

/**
 * 配置节点
 */
class ConfigurationNode : public CHTLBaseNode {
public:
    ConfigurationNode(const std::string& name = "");
    ~ConfigurationNode();
    
    void setName(const std::string& name);
    const std::string& getName() const;
    bool hasName() const;
    
    void setConfiguration(const std::string& key, const std::string& value);
    const std::map<std::string, std::string>& getConfigurations() const;
    
    std::string generateCode() const override;
    std::string toString() const override;

private:
    std::string name_;
    std::map<std::string, std::string> configurations_;
    bool has_name_;
};

} // namespace CHTL

#endif // CHTL_STYLENODE_H