#ifndef CHTL_TEMPLATENODE_H
#define CHTL_TEMPLATENODE_H

#include "CHTLBaseNode.h"

namespace CHTL {

/**
 * 模板节点基类
 */
class TemplateNode : public CHTLBaseNode {
public:
    TemplateNode(NodeType type, const std::string& name);
    ~TemplateNode();
    
    void setName(const std::string& name);
    const std::string& getName() const;
    
    std::string generateCode() const override;
    std::string toString() const override;

protected:
    std::string name_;
};

/**
 * 样式组模板节点
 */
class TemplateStyleNode : public TemplateNode {
public:
    TemplateStyleNode(const std::string& name);
    ~TemplateStyleNode();
    
    void addStyleProperty(const std::string& property, const std::string& value);
    const std::map<std::string, std::string>& getStyleProperties() const;
    
    std::string generateCode() const override;

private:
    std::map<std::string, std::string> style_properties_;
};

/**
 * 元素模板节点
 */
class TemplateElementNode : public TemplateNode {
public:
    TemplateElementNode(const std::string& name);
    ~TemplateElementNode();
    
    std::string generateCode() const override;
};

/**
 * 变量组模板节点
 */
class TemplateVarNode : public TemplateNode {
public:
    TemplateVarNode(const std::string& name);
    ~TemplateVarNode();
    
    void addVariable(const std::string& name, const std::string& value);
    const std::map<std::string, std::string>& getVariables() const;
    
    std::string generateCode() const override;

private:
    std::map<std::string, std::string> variables_;
};

/**
 * 自定义节点基类
 */
class CustomNode : public CHTLBaseNode {
public:
    CustomNode(NodeType type, const std::string& name);
    ~CustomNode();
    
    void setName(const std::string& name);
    const std::string& getName() const;
    
    std::string generateCode() const override;
    std::string toString() const override;

protected:
    std::string name_;
};

/**
 * 自定义样式组节点
 */
class CustomStyleNode : public CustomNode {
public:
    CustomStyleNode(const std::string& name);
    ~CustomStyleNode();
    
    void addStyleProperty(const std::string& property, const std::string& value);
    const std::map<std::string, std::string>& getStyleProperties() const;
    
    // 特例化操作
    void deleteProperty(const std::string& property);
    void deleteInheritance(const std::string& inheritance);
    
    std::string generateCode() const override;

private:
    std::map<std::string, std::string> style_properties_;
    std::vector<std::string> deleted_properties_;
    std::vector<std::string> deleted_inheritances_;
};

/**
 * 自定义元素节点
 */
class CustomElementNode : public CustomNode {
public:
    CustomElementNode(const std::string& name);
    ~CustomElementNode();
    
    // 特例化操作
    void insertElement(const std::string& position, std::shared_ptr<CHTLBaseNode> element);
    void deleteElement(const std::string& selector);
    void deleteElementByIndex(int index);
    
    std::string generateCode() const override;

private:
    std::vector<std::pair<std::string, std::shared_ptr<CHTLBaseNode>>> insertions_;
    std::vector<std::string> deletions_;
};

/**
 * 自定义变量组节点
 */
class CustomVarNode : public CustomNode {
public:
    CustomVarNode(const std::string& name);
    ~CustomVarNode();
    
    void addVariable(const std::string& name, const std::string& value);
    const std::map<std::string, std::string>& getVariables() const;
    
    // 特例化操作
    void setVariableValue(const std::string& name, const std::string& value);
    
    std::string generateCode() const override;

private:
    std::map<std::string, std::string> variables_;
};

} // namespace CHTL

#endif // CHTL_TEMPLATENODE_H