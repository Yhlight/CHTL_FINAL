#ifndef CHTL_JS_BASE_NODE_H
#define CHTL_JS_BASE_NODE_H

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>

namespace CHTL_JS {

enum class CHTLJSNodeType {
    // 基础节点
    BASE,
    EXPRESSION,
    STATEMENT,
    DECLARATION,
    
    // 表达式节点
    LITERAL,
    IDENTIFIER,
    BINARY_EXPRESSION,
    UNARY_EXPRESSION,
    TERNARY_EXPRESSION,
    CALL_EXPRESSION,
    MEMBER_EXPRESSION,
    ARRAY_EXPRESSION,
    OBJECT_EXPRESSION,
    FUNCTION_EXPRESSION,
    ARROW_FUNCTION_EXPRESSION,
    
    // 语句节点
    EXPRESSION_STATEMENT,
    BLOCK_STATEMENT,
    IF_STATEMENT,
    FOR_STATEMENT,
    WHILE_STATEMENT,
    DO_WHILE_STATEMENT,
    SWITCH_STATEMENT,
    CASE_STATEMENT,
    DEFAULT_STATEMENT,
    BREAK_STATEMENT,
    CONTINUE_STATEMENT,
    RETURN_STATEMENT,
    THROW_STATEMENT,
    TRY_STATEMENT,
    CATCH_STATEMENT,
    FINALLY_STATEMENT,
    
    // 声明节点
    VARIABLE_DECLARATION,
    FUNCTION_DECLARATION,
    CLASS_DECLARATION,
    INTERFACE_DECLARATION,
    ENUM_DECLARATION,
    NAMESPACE_DECLARATION,
    MODULE_DECLARATION,
    
    // CHTL JS特有节点
    ENHANCED_SELECTOR,
    RESPONSIVE_VALUE,
    VIRTUAL_OBJECT,
    LISTEN_EXPRESSION,
    DELEGATE_EXPRESSION,
    ANIMATE_EXPRESSION,
    ROUTER_EXPRESSION,
    FILELOADER_EXPRESSION,
    UTIL_EXPRESSION,
    
    // 特殊节点
    COMMENT,
    DIRECTIVE,
    IMPORT_DECLARATION,
    EXPORT_DECLARATION,
    
    // 错误节点
    ERROR,
    UNKNOWN
};

class CHTLJSBaseNode : public std::enable_shared_from_this<CHTLJSBaseNode> {
protected:
    CHTLJSNodeType nodeType;
    std::string name;
    std::string value;
    std::map<std::string, std::string> attributes;
    std::vector<std::shared_ptr<CHTLJSBaseNode>> children;
    std::shared_ptr<CHTLJSBaseNode> parent;
    
    // 位置信息
    int line;
    int column;
    int position;
    
    // 类型信息
    std::string typeAnnotation;
    bool optional;
    bool nullable;
    
    // 作用域信息
    std::string scope;
    std::vector<std::string> dependencies;
    
    // 元数据
    std::map<std::string, std::string> metadata;
    std::vector<std::string> comments;
    std::vector<std::string> directives;

public:
    CHTLJSBaseNode();
    CHTLJSBaseNode(CHTLJSNodeType type, const std::string& name = "", const std::string& value = "");
    virtual ~CHTLJSBaseNode() = default;
    
    // 基本属性
    CHTLJSNodeType getNodeType() const { return nodeType; }
    void setNodeType(CHTLJSNodeType type) { nodeType = type; }
    
    const std::string& getName() const { return name; }
    void setName(const std::string& name) { this->name = name; }
    
    const std::string& getValue() const { return value; }
    void setValue(const std::string& value) { this->value = value; }
    
    // 属性管理
    void setAttribute(const std::string& key, const std::string& value);
    std::string getAttribute(const std::string& key) const;
    bool hasAttribute(const std::string& key) const;
    void removeAttribute(const std::string& key);
    const std::map<std::string, std::string>& getAttributes() const { return attributes; }
    
    // 子节点管理
    void addChild(std::shared_ptr<CHTLJSBaseNode> child);
    void removeChild(std::shared_ptr<CHTLJSBaseNode> child);
    void removeChild(size_t index);
    void insertChild(size_t index, std::shared_ptr<CHTLJSBaseNode> child);
    void clearChildren();
    
    std::shared_ptr<CHTLJSBaseNode> getChild(size_t index) const;
    std::vector<std::shared_ptr<CHTLJSBaseNode>> getChildren() const { return children; }
    size_t getChildCount() const { return children.size(); }
    
    // 父节点管理
    std::shared_ptr<CHTLJSBaseNode> getParent() const { return parent; }
    void setParent(std::shared_ptr<CHTLJSBaseNode> parent) { this->parent = parent; }
    
    // 位置信息
    int getLine() const { return line; }
    void setLine(int line) { this->line = line; }
    
    int getColumn() const { return column; }
    void setColumn(int column) { this->column = column; }
    
    int getPosition() const { return position; }
    void setPosition(int position) { this->position = position; }
    
    void setLocation(int line, int column, int position);
    
    // 类型信息
    const std::string& getTypeAnnotation() const { return typeAnnotation; }
    void setTypeAnnotation(const std::string& type) { typeAnnotation = type; }
    
    bool isOptional() const { return optional; }
    void setOptional(bool optional) { this->optional = optional; }
    
    bool isNullable() const { return nullable; }
    void setNullable(bool nullable) { this->nullable = nullable; }
    
    // 作用域信息
    const std::string& getScope() const { return scope; }
    void setScope(const std::string& scope) { this->scope = scope; }
    
    const std::vector<std::string>& getDependencies() const { return dependencies; }
    void addDependency(const std::string& dependency);
    void removeDependency(const std::string& dependency);
    void clearDependencies();
    
    // 元数据管理
    void setMetadata(const std::string& key, const std::string& value);
    std::string getMetadata(const std::string& key) const;
    bool hasMetadata(const std::string& key) const;
    void removeMetadata(const std::string& key);
    const std::map<std::string, std::string>& getMetadata() const { return metadata; }
    
    // 注释管理
    void addComment(const std::string& comment);
    void removeComment(size_t index);
    void clearComments();
    const std::vector<std::string>& getComments() const { return comments; }
    
    // 指令管理
    void addDirective(const std::string& directive);
    void removeDirective(size_t index);
    void clearDirectives();
    const std::vector<std::string>& getDirectives() const { return directives; }
    
    // 类型判断
    virtual bool isExpression() const;
    virtual bool isStatement() const;
    virtual bool isDeclaration() const;
    virtual bool isLiteral() const;
    virtual bool isIdentifier() const;
    virtual bool isFunction() const;
    virtual bool isClass() const;
    virtual bool isModule() const;
    virtual bool isCHTLJSSpecific() const;
    
    // 验证
    virtual bool isValid() const;
    virtual bool isComplete() const;
    virtual std::vector<std::string> validate() const;
    
    // 转换
    virtual std::string toJavaScript() const;
    virtual std::string toCHTLJS() const;
    virtual std::string toHTML() const;
    virtual std::string toCSS() const;
    virtual std::string toJSON() const;
    
    // 字符串表示
    virtual std::string toString() const;
    virtual std::string toDebugString() const;
    virtual std::string toPrettyString(int indent = 0) const;
    
    // 遍历
    virtual void traverse(std::function<void(std::shared_ptr<CHTLJSBaseNode>)> visitor);
    virtual void traverse(std::function<void(std::shared_ptr<CHTLJSBaseNode>)> visitor) const;
    virtual void traverseChildren(std::function<void(std::shared_ptr<CHTLJSBaseNode>)> visitor);
    virtual void traverseChildren(std::function<void(std::shared_ptr<CHTLJSBaseNode>)> visitor) const;
    
    // 查找
    virtual std::vector<std::shared_ptr<CHTLJSBaseNode>> findNodes(CHTLJSNodeType type) const;
    virtual std::vector<std::shared_ptr<CHTLJSBaseNode>> findNodes(std::function<bool(std::shared_ptr<CHTLJSBaseNode>)> predicate) const;
    virtual std::shared_ptr<CHTLJSBaseNode> findFirstNode(CHTLJSNodeType type) const;
    virtual std::shared_ptr<CHTLJSBaseNode> findFirstNode(std::function<bool(std::shared_ptr<CHTLJSBaseNode>)> predicate) const;
    
    // 克隆
    virtual std::shared_ptr<CHTLJSBaseNode> clone() const;
    virtual std::shared_ptr<CHTLJSBaseNode> deepClone() const;
    
    // 比较
    virtual bool equals(std::shared_ptr<CHTLJSBaseNode> other) const;
    virtual bool equals(const CHTLJSBaseNode& other) const;
    
    // 序列化
    virtual std::string serialize() const;
    virtual bool deserialize(const std::string& data);
    
    // 优化
    virtual std::shared_ptr<CHTLJSBaseNode> optimize() const;
    virtual std::shared_ptr<CHTLJSBaseNode> compress() const;
    virtual std::shared_ptr<CHTLJSBaseNode> decompress() const;
    
    // 格式化
    virtual std::string format() const;
    virtual std::string minify() const;
    virtual std::string beautify() const;
    
    // 辅助方法
    virtual std::string getNodeTypeName() const;

protected:
    virtual std::string formatIndent(int indent) const;
    virtual std::string formatAttributes() const;
    virtual std::string formatChildren(int indent) const;
    virtual std::string formatMetadata() const;
    virtual std::string formatComments() const;
    virtual std::string formatDirectives() const;
    
    // 验证辅助
    virtual bool validateNode() const;
    virtual bool validateChildren() const;
    virtual bool validateAttributes() const;
    virtual bool validateMetadata() const;
    
    // 转换辅助
    virtual std::string convertToJavaScript() const;
    virtual std::string convertToCHTLJS() const;
    virtual std::string convertToHTML() const;
    virtual std::string convertToCSS() const;
    virtual std::string convertToJSON() const;
    
    // 遍历辅助
    virtual void traverseNode(std::function<void(std::shared_ptr<CHTLJSBaseNode>)> visitor);
    virtual void traverseNode(std::function<void(std::shared_ptr<CHTLJSBaseNode>)> visitor) const;
    
    // 查找辅助
    virtual void collectNodes(CHTLJSNodeType type, std::vector<std::shared_ptr<CHTLJSBaseNode>>& result) const;
    virtual void collectNodes(std::function<bool(std::shared_ptr<CHTLJSBaseNode>)> predicate, std::vector<std::shared_ptr<CHTLJSBaseNode>>& result) const;
    
    // 克隆辅助
    virtual void copyTo(std::shared_ptr<CHTLJSBaseNode> target) const;
    virtual void deepCopyTo(std::shared_ptr<CHTLJSBaseNode> target) const;
    
    // 比较辅助
    virtual bool compareNode(std::shared_ptr<CHTLJSBaseNode> other) const;
    virtual bool compareNode(const CHTLJSBaseNode& other) const;
    virtual bool compareChildren(std::shared_ptr<CHTLJSBaseNode> other) const;
    virtual bool compareChildren(const CHTLJSBaseNode& other) const;
    virtual bool compareAttributes(std::shared_ptr<CHTLJSBaseNode> other) const;
    virtual bool compareAttributes(const CHTLJSBaseNode& other) const;
    virtual bool compareMetadata(std::shared_ptr<CHTLJSBaseNode> other) const;
    virtual bool compareMetadata(const CHTLJSBaseNode& other) const;
    
    // 序列化辅助
    virtual std::string serializeNode() const;
    virtual bool deserializeNode(const std::string& data);
    
    // 优化辅助
    virtual std::shared_ptr<CHTLJSBaseNode> optimizeNode() const;
    virtual std::shared_ptr<CHTLJSBaseNode> compressNode() const;
    virtual std::shared_ptr<CHTLJSBaseNode> decompressNode() const;
    
    // 格式化辅助
    virtual std::string formatNode() const;
    virtual std::string minifyNode() const;
    virtual std::string beautifyNode() const;
};

} // namespace CHTL_JS

#endif // CHTL_JS_BASE_NODE_H