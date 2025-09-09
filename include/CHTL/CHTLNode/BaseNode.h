#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <any>

namespace CHTL {

// Forward declarations
class Token;

/**
 * @brief 节点类型枚举
 * 
 * 定义了AST中所有可能的节点类型
 */
enum class NodeType {
    // 基础节点
    ROOT,               // 根节点
    ELEMENT,            // 元素节点
    TEXT,               // 文本节点
    COMMENT,            // 注释节点
    
    // 样式和脚本
    STYLE,              // 样式节点
    SCRIPT,             // 脚本节点
    
    // 模板系统
    TEMPLATE,           // 模板节点
    CUSTOM,             // 自定义节点
    
    // 原始嵌入
    ORIGIN,             // 原始嵌入节点
    
    // 导入和配置
    IMPORT,             // 导入节点
    CONFIGURATION,      // 配置节点
    NAMESPACE,          // 命名空间节点
    
    // 操作符
    DELETE,             // 删除操作
    INSERT,             // 插入操作
    INHERIT,            // 继承操作
    
    // 属性
    ATTRIBUTE,          // 属性节点
    STYLE_PROPERTY,     // 样式属性节点
    
    // 条件表达式
    CONDITIONAL,        // 条件表达式节点
    LOGICAL_AND,        // 逻辑与节点
    LOGICAL_OR,         // 逻辑或节点
    COMPARISON,         // 比较节点
    
    // CHTL JS 节点
    VIR_OBJECT,         // 虚对象节点
    LISTEN,             // 监听器节点
    ANIMATE,            // 动画节点
    ROUTER,             // 路由节点
    FILELOADER,         // 文件加载器节点
    DELEGATE,           // 委托节点
    INEVERAWAY,         // iNeverAway节点
    UTIL_EXPR,          // util表达式节点
    PRINTMYLOVE,        // printMylove节点
};

/**
 * @brief 基础AST节点类
 * 
 * 所有AST节点的基类，提供通用的节点功能
 */
class BaseNode {
public:
    BaseNode(NodeType type);
    virtual ~BaseNode() = default;

    /**
     * @brief 获取节点类型
     * @return 节点类型
     */
    NodeType getType() const { return m_type; }

    /**
     * @brief 设置节点类型
     * @param type 节点类型
     */
    void setType(NodeType type) { m_type = type; }

    /**
     * @brief 添加子节点
     * @param child 子节点
     */
    void addChild(std::unique_ptr<BaseNode> child);

    /**
     * @brief 获取子节点列表
     * @return 子节点列表
     */
    const std::vector<std::unique_ptr<BaseNode>>& getChildren() const { return m_children; }

    /**
     * @brief 获取子节点数量
     * @return 子节点数量
     */
    size_t getChildCount() const { return m_children.size(); }

    /**
     * @brief 获取指定索引的子节点
     * @param index 索引
     * @return 子节点指针
     */
    BaseNode* getChild(size_t index) const;

    /**
     * @brief 移除子节点
     * @param index 索引
     * @return 被移除的节点
     */
    std::unique_ptr<BaseNode> removeChild(size_t index);

    /**
     * @brief 清空所有子节点
     */
    void clearChildren();

    /**
     * @brief 设置属性
     * @param key 属性键
     * @param value 属性值
     */
    void setAttribute(const std::string& key, const std::any& value);

    /**
     * @brief 获取属性
     * @param key 属性键
     * @return 属性值
     */
    std::any getAttribute(const std::string& key) const;

    /**
     * @brief 检查是否有属性
     * @param key 属性键
     * @return 是否有该属性
     */
    bool hasAttribute(const std::string& key) const;

    /**
     * @brief 移除属性
     * @param key 属性键
     * @return 是否成功移除
     */
    bool removeAttribute(const std::string& key);

    /**
     * @brief 获取所有属性
     * @return 属性映射
     */
    const std::unordered_map<std::string, std::any>& getAttributes() const { return m_attributes; }

    /**
     * @brief 设置令牌
     * @param token 令牌
     */
    void setToken(std::unique_ptr<Token> token);

    /**
     * @brief 获取令牌
     * @return 令牌指针
     */
    Token* getToken() const { return m_token.get(); }

    /**
     * @brief 设置行号
     * @param line 行号
     */
    void setLine(size_t line) { m_line = line; }

    /**
     * @brief 获取行号
     * @return 行号
     */
    size_t getLine() const { return m_line; }

    /**
     * @brief 设置列号
     * @param column 列号
     */
    void setColumn(size_t column) { m_column = column; }

    /**
     * @brief 获取列号
     * @return 列号
     */
    size_t getColumn() const { return m_column; }

    /**
     * @brief 设置父节点
     * @param parent 父节点
     */
    void setParent(BaseNode* parent) { m_parent = parent; }

    /**
     * @brief 获取父节点
     * @return 父节点指针
     */
    BaseNode* getParent() const { return m_parent; }

    /**
     * @brief 检查是否为叶子节点
     * @return 是否为叶子节点
     */
    bool isLeaf() const { return m_children.empty(); }

    /**
     * @brief 检查是否为根节点
     * @return 是否为根节点
     */
    bool isRoot() const { return m_parent == nullptr; }

    /**
     * @brief 获取节点深度
     * @return 节点深度
     */
    size_t getDepth() const;

    /**
     * @brief 查找子节点
     * @param type 节点类型
     * @return 找到的节点指针
     */
    BaseNode* findChild(NodeType type) const;

    /**
     * @brief 查找所有匹配的子节点
     * @param type 节点类型
     * @return 匹配的节点列表
     */
    std::vector<BaseNode*> findAllChildren(NodeType type) const;

    /**
     * @brief 获取节点路径
     * @return 节点路径字符串
     */
    std::string getPath() const;

    /**
     * @brief 克隆节点
     * @return 克隆的节点
     */
    virtual std::unique_ptr<BaseNode> clone() const;

    /**
     * @brief 接受访问者
     * @param visitor 访问者
     */
    virtual void accept(class NodeVisitor* visitor);

    /**
     * @brief 获取节点字符串表示
     * @return 字符串表示
     */
    virtual std::string toString() const;

    /**
     * @brief 获取节点类型字符串
     * @return 类型字符串
     */
    static std::string typeToString(NodeType type);

protected:
    NodeType m_type;
    std::vector<std::unique_ptr<BaseNode>> m_children;
    std::unordered_map<std::string, std::any> m_attributes;
    std::unique_ptr<Token> m_token;
    size_t m_line;
    size_t m_column;
    BaseNode* m_parent;
};

/**
 * @brief 节点访问者接口
 * 
 * 实现访问者模式，用于遍历和操作AST节点
 */
class NodeVisitor {
public:
    virtual ~NodeVisitor() = default;
    virtual void visit(BaseNode* node) = 0;
    virtual void visitElement(class ElementNode* node) {}
    virtual void visitText(class TextNode* node) {}
    virtual void visitComment(class CommentNode* node) {}
    virtual void visitStyle(class StyleNode* node) {}
    virtual void visitScript(class ScriptNode* node) {}
    virtual void visitTemplate(class TemplateNode* node) {}
    virtual void visitCustom(class CustomNode* node) {}
    virtual void visitOrigin(class OriginNode* node) {}
    virtual void visitImport(class ImportNode* node) {}
    virtual void visitConfiguration(class ConfigNode* node) {}
    virtual void visitNamespace(class NamespaceNode* node) {}
};

} // namespace CHTL