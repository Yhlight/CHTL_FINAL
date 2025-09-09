#pragma once

namespace CHTL {

// 前向声明
class BaseNode;
class ElementNode;
class TextNode;
class CommentNode;
class TemplateNode;
class CustomNode;
class StyleNode;
class ScriptNode;
class OriginNode;
class ImportNode;
class ConfigNode;
class NamespaceNode;
class OperatorNode;

/**
 * @brief 访问者模式基类
 * 
 * 用于实现访问者模式，支持对 AST 节点的遍历和操作
 */
class Visitor {
public:
    /**
     * @brief 虚析构函数
     */
    virtual ~Visitor() = default;
    
    /**
     * @brief 访问基类节点
     * @param node 基类节点
     */
    virtual void visit(BaseNode& node) = 0;
    
    /**
     * @brief 访问元素节点
     * @param node 元素节点
     */
    virtual void visit(ElementNode& node) = 0;
    
    /**
     * @brief 访问文本节点
     * @param node 文本节点
     */
    virtual void visit(TextNode& node) = 0;
    
    /**
     * @brief 访问注释节点
     * @param node 注释节点
     */
    virtual void visit(CommentNode& node) = 0;
    
    /**
     * @brief 访问模板节点
     * @param node 模板节点
     */
    virtual void visit(TemplateNode& node) = 0;
    
    /**
     * @brief 访问自定义节点
     * @param node 自定义节点
     */
    virtual void visit(CustomNode& node) = 0;
    
    /**
     * @brief 访问样式节点
     * @param node 样式节点
     */
    virtual void visit(StyleNode& node) = 0;
    
    /**
     * @brief 访问脚本节点
     * @param node 脚本节点
     */
    virtual void visit(ScriptNode& node) = 0;
    
    /**
     * @brief 访问原始嵌入节点
     * @param node 原始嵌入节点
     */
    virtual void visit(OriginNode& node) = 0;
    
    /**
     * @brief 访问导入节点
     * @param node 导入节点
     */
    virtual void visit(ImportNode& node) = 0;
    
    /**
     * @brief 访问配置节点
     * @param node 配置节点
     */
    virtual void visit(ConfigNode& node) = 0;
    
    /**
     * @brief 访问命名空间节点
     * @param node 命名空间节点
     */
    virtual void visit(NamespaceNode& node) = 0;
    
    /**
     * @brief 访问操作符节点
     * @param node 操作符节点
     */
    virtual void visit(OperatorNode& node) = 0;
};

} // namespace CHTL