#include "CHTLJSBaseNode.h"
#include <sstream>
#include <algorithm>
#include <cctype>

namespace CHTL_JS {

CHTLJSBaseNode::CHTLJSBaseNode() 
    : nodeType(CHTLJSNodeType::BASE), line(0), column(0), position(0), isOptional(false), isNullable(false) {
}

CHTLJSBaseNode::CHTLJSBaseNode(CHTLJSNodeType type, const std::string& name, const std::string& value)
    : nodeType(type), name(name), value(value), line(0), column(0), position(0), isOptional(false), isNullable(false) {
}

// 属性管理实现
void CHTLJSBaseNode::setAttribute(const std::string& key, const std::string& value) {
    attributes[key] = value;
}

std::string CHTLJSBaseNode::getAttribute(const std::string& key) const {
    auto it = attributes.find(key);
    return it != attributes.end() ? it->second : "";
}

bool CHTLJSBaseNode::hasAttribute(const std::string& key) const {
    return attributes.find(key) != attributes.end();
}

void CHTLJSBaseNode::removeAttribute(const std::string& key) {
    attributes.erase(key);
}

// 子节点管理实现
void CHTLJSBaseNode::addChild(std::shared_ptr<CHTLJSBaseNode> child) {
    if (child) {
        child->setParent(shared_from_this());
        children.push_back(child);
    }
}

void CHTLJSBaseNode::removeChild(std::shared_ptr<CHTLJSBaseNode> child) {
    auto it = std::find(children.begin(), children.end(), child);
    if (it != children.end()) {
        (*it)->setParent(nullptr);
        children.erase(it);
    }
}

void CHTLJSBaseNode::removeChild(size_t index) {
    if (index < children.size()) {
        children[index]->setParent(nullptr);
        children.erase(children.begin() + index);
    }
}

void CHTLJSBaseNode::insertChild(size_t index, std::shared_ptr<CHTLJSBaseNode> child) {
    if (child && index <= children.size()) {
        child->setParent(shared_from_this());
        children.insert(children.begin() + index, child);
    }
}

void CHTLJSBaseNode::clearChildren() {
    for (auto& child : children) {
        child->setParent(nullptr);
    }
    children.clear();
}

std::shared_ptr<CHTLJSBaseNode> CHTLJSBaseNode::getChild(size_t index) const {
    return index < children.size() ? children[index] : nullptr;
}

void CHTLJSBaseNode::setLocation(int line, int column, int position) {
    this->line = line;
    this->column = column;
    this->position = position;
}

// 依赖管理实现
void CHTLJSBaseNode::addDependency(const std::string& dependency) {
    if (std::find(dependencies.begin(), dependencies.end(), dependency) == dependencies.end()) {
        dependencies.push_back(dependency);
    }
}

void CHTLJSBaseNode::removeDependency(const std::string& dependency) {
    dependencies.erase(std::remove(dependencies.begin(), dependencies.end(), dependency), dependencies.end());
}

void CHTLJSBaseNode::clearDependencies() {
    dependencies.clear();
}

// 元数据管理实现
void CHTLJSBaseNode::setMetadata(const std::string& key, const std::string& value) {
    metadata[key] = value;
}

std::string CHTLJSBaseNode::getMetadata(const std::string& key) const {
    auto it = metadata.find(key);
    return it != metadata.end() ? it->second : "";
}

bool CHTLJSBaseNode::hasMetadata(const std::string& key) const {
    return metadata.find(key) != metadata.end();
}

void CHTLJSBaseNode::removeMetadata(const std::string& key) {
    metadata.erase(key);
}

// 注释管理实现
void CHTLJSBaseNode::addComment(const std::string& comment) {
    comments.push_back(comment);
}

void CHTLJSBaseNode::removeComment(size_t index) {
    if (index < comments.size()) {
        comments.erase(comments.begin() + index);
    }
}

void CHTLJSBaseNode::clearComments() {
    comments.clear();
}

// 指令管理实现
void CHTLJSBaseNode::addDirective(const std::string& directive) {
    directives.push_back(directive);
}

void CHTLJSBaseNode::removeDirective(size_t index) {
    if (index < directives.size()) {
        directives.erase(directives.begin() + index);
    }
}

void CHTLJSBaseNode::clearDirectives() {
    directives.clear();
}

// 类型判断实现
bool CHTLJSBaseNode::isExpression() const {
    switch (nodeType) {
        case CHTLJSNodeType::LITERAL:
        case CHTLJSNodeType::IDENTIFIER:
        case CHTLJSNodeType::BINARY_EXPRESSION:
        case CHTLJSNodeType::UNARY_EXPRESSION:
        case CHTLJSNodeType::TERNARY_EXPRESSION:
        case CHTLJSNodeType::CALL_EXPRESSION:
        case CHTLJSNodeType::MEMBER_EXPRESSION:
        case CHTLJSNodeType::ARRAY_EXPRESSION:
        case CHTLJSNodeType::OBJECT_EXPRESSION:
        case CHTLJSNodeType::FUNCTION_EXPRESSION:
        case CHTLJSNodeType::ARROW_FUNCTION_EXPRESSION:
        case CHTLJSNodeType::ENHANCED_SELECTOR:
        case CHTLJSNodeType::LISTEN_EXPRESSION:
        case CHTLJSNodeType::DELEGATE_EXPRESSION:
        case CHTLJSNodeType::ANIMATE_EXPRESSION:
        case CHTLJSNodeType::ROUTER_EXPRESSION:
        case CHTLJSNodeType::FILELOADER_EXPRESSION:
        case CHTLJSNodeType::UTIL_EXPRESSION:
            return true;
        default:
            return false;
    }
}

bool CHTLJSBaseNode::isStatement() const {
    switch (nodeType) {
        case CHTLJSNodeType::EXPRESSION_STATEMENT:
        case CHTLJSNodeType::BLOCK_STATEMENT:
        case CHTLJSNodeType::IF_STATEMENT:
        case CHTLJSNodeType::FOR_STATEMENT:
        case CHTLJSNodeType::WHILE_STATEMENT:
        case CHTLJSNodeType::DO_WHILE_STATEMENT:
        case CHTLJSNodeType::SWITCH_STATEMENT:
        case CHTLJSNodeType::CASE_STATEMENT:
        case CHTLJSNodeType::DEFAULT_STATEMENT:
        case CHTLJSNodeType::BREAK_STATEMENT:
        case CHTLJSNodeType::CONTINUE_STATEMENT:
        case CHTLJSNodeType::RETURN_STATEMENT:
        case CHTLJSNodeType::THROW_STATEMENT:
        case CHTLJSNodeType::TRY_STATEMENT:
        case CHTLJSNodeType::CATCH_STATEMENT:
        case CHTLJSNodeType::FINALLY_STATEMENT:
            return true;
        default:
            return false;
    }
}

bool CHTLJSBaseNode::isDeclaration() const {
    switch (nodeType) {
        case CHTLJSNodeType::VARIABLE_DECLARATION:
        case CHTLJSNodeType::FUNCTION_DECLARATION:
        case CHTLJSNodeType::CLASS_DECLARATION:
        case CHTLJSNodeType::INTERFACE_DECLARATION:
        case CHTLJSNodeType::ENUM_DECLARATION:
        case CHTLJSNodeType::NAMESPACE_DECLARATION:
        case CHTLJSNodeType::MODULE_DECLARATION:
        case CHTLJSNodeType::IMPORT_DECLARATION:
        case CHTLJSNodeType::EXPORT_DECLARATION:
            return true;
        default:
            return false;
    }
}

bool CHTLJSBaseNode::isLiteral() const {
    return nodeType == CHTLJSNodeType::LITERAL;
}

bool CHTLJSBaseNode::isIdentifier() const {
    return nodeType == CHTLJSNodeType::IDENTIFIER;
}

bool CHTLJSBaseNode::isFunction() const {
    return nodeType == CHTLJSNodeType::FUNCTION_DECLARATION ||
           nodeType == CHTLJSNodeType::FUNCTION_EXPRESSION ||
           nodeType == CHTLJSNodeType::ARROW_FUNCTION_EXPRESSION;
}

bool CHTLJSBaseNode::isClass() const {
    return nodeType == CHTLJSNodeType::CLASS_DECLARATION;
}

bool CHTLJSBaseNode::isModule() const {
    return nodeType == CHTLJSNodeType::MODULE_DECLARATION;
}

bool CHTLJSBaseNode::isCHTLJSSpecific() const {
    switch (nodeType) {
        case CHTLJSNodeType::ENHANCED_SELECTOR:
        case CHTLJSNodeType::VIRTUAL_OBJECT:
        case CHTLJSNodeType::LISTEN_EXPRESSION:
        case CHTLJSNodeType::DELEGATE_EXPRESSION:
        case CHTLJSNodeType::ANIMATE_EXPRESSION:
        case CHTLJSNodeType::ROUTER_EXPRESSION:
        case CHTLJSNodeType::FILELOADER_EXPRESSION:
        case CHTLJSNodeType::UTIL_EXPRESSION:
            return true;
        default:
            return false;
    }
}

// 验证实现
bool CHTLJSBaseNode::isValid() const {
    return validateNode() && validateChildren() && validateAttributes() && validateMetadata();
}

bool CHTLJSBaseNode::isComplete() const {
    return !name.empty() || !value.empty() || !children.empty();
}

std::vector<std::string> CHTLJSBaseNode::validate() const {
    std::vector<std::string> errors;
    
    if (!validateNode()) {
        errors.push_back("节点验证失败");
    }
    
    if (!validateChildren()) {
        errors.push_back("子节点验证失败");
    }
    
    if (!validateAttributes()) {
        errors.push_back("属性验证失败");
    }
    
    if (!validateMetadata()) {
        errors.push_back("元数据验证失败");
    }
    
    return errors;
}

// 转换实现
std::string CHTLJSBaseNode::toJavaScript() const {
    return convertToJavaScript();
}

std::string CHTLJSBaseNode::toCHTLJS() const {
    return convertToCHTLJS();
}

std::string CHTLJSBaseNode::toHTML() const {
    return convertToHTML();
}

std::string CHTLJSBaseNode::toCSS() const {
    return convertToCSS();
}

std::string CHTLJSBaseNode::toJSON() const {
    return convertToJSON();
}

// 字符串表示实现
std::string CHTLJSBaseNode::toString() const {
    return value.empty() ? name : value;
}

std::string CHTLJSBaseNode::toDebugString() const {
    std::ostringstream oss;
    oss << "CHTLJSBaseNode{type=" << getNodeTypeName()
        << ", name='" << name
        << "', value='" << value
        << "', line=" << line
        << ", column=" << column
        << ", position=" << position
        << ", children=" << children.size() << "}";
    return oss.str();
}

std::string CHTLJSBaseNode::toPrettyString(int indent) const {
    std::ostringstream oss;
    oss << formatIndent(indent) << getNodeTypeName();
    
    if (!name.empty()) {
        oss << " name='" << name << "'";
    }
    
    if (!value.empty()) {
        oss << " value='" << value << "'";
    }
    
    if (!attributes.empty()) {
        oss << " " << formatAttributes();
    }
    
    if (!children.empty()) {
        oss << " {\n" << formatChildren(indent + 1) << formatIndent(indent) << "}";
    }
    
    return oss.str();
}

// 遍历实现
void CHTLJSBaseNode::traverse(std::function<void(std::shared_ptr<CHTLJSBaseNode>)> visitor) {
    traverseNode(visitor);
}

void CHTLJSBaseNode::traverse(std::function<void(std::shared_ptr<CHTLJSBaseNode>)> visitor) const {
    traverseNode(visitor);
}

void CHTLJSBaseNode::traverseChildren(std::function<void(std::shared_ptr<CHTLJSBaseNode>)> visitor) {
    for (auto& child : children) {
        if (child) {
            child->traverse(visitor);
        }
    }
}

void CHTLJSBaseNode::traverseChildren(std::function<void(std::shared_ptr<CHTLJSBaseNode>)> visitor) const {
    for (const auto& child : children) {
        if (child) {
            child->traverse(visitor);
        }
    }
}

// 查找实现
std::vector<std::shared_ptr<CHTLJSBaseNode>> CHTLJSBaseNode::findNodes(CHTLJSNodeType type) const {
    std::vector<std::shared_ptr<CHTLJSBaseNode>> result;
    collectNodes(type, result);
    return result;
}

std::vector<std::shared_ptr<CHTLJSBaseNode>> CHTLJSBaseNode::findNodes(std::function<bool(std::shared_ptr<CHTLJSBaseNode>)> predicate) const {
    std::vector<std::shared_ptr<CHTLJSBaseNode>> result;
    collectNodes(predicate, result);
    return result;
}

std::shared_ptr<CHTLJSBaseNode> CHTLJSBaseNode::findFirstNode(CHTLJSNodeType type) const {
    auto nodes = findNodes(type);
    return nodes.empty() ? nullptr : nodes[0];
}

std::shared_ptr<CHTLJSBaseNode> CHTLJSBaseNode::findFirstNode(std::function<bool(std::shared_ptr<CHTLJSBaseNode>)> predicate) const {
    auto nodes = findNodes(predicate);
    return nodes.empty() ? nullptr : nodes[0];
}

// 克隆实现
std::shared_ptr<CHTLJSBaseNode> CHTLJSBaseNode::clone() const {
    auto cloned = std::make_shared<CHTLJSBaseNode>(nodeType, name, value);
    copyTo(cloned);
    return cloned;
}

std::shared_ptr<CHTLJSBaseNode> CHTLJSBaseNode::deepClone() const {
    auto cloned = std::make_shared<CHTLJSBaseNode>(nodeType, name, value);
    deepCopyTo(cloned);
    return cloned;
}

// 比较实现
bool CHTLJSBaseNode::equals(std::shared_ptr<CHTLJSBaseNode> other) const {
    return other && compareNode(other);
}

bool CHTLJSBaseNode::equals(const CHTLJSBaseNode& other) const {
    return compareNode(other);
}

// 序列化实现
std::string CHTLJSBaseNode::serialize() const {
    return serializeNode();
}

bool CHTLJSBaseNode::deserialize(const std::string& data) {
    return deserializeNode(data);
}

// 优化实现
std::shared_ptr<CHTLJSBaseNode> CHTLJSBaseNode::optimize() const {
    return optimizeNode();
}

std::shared_ptr<CHTLJSBaseNode> CHTLJSBaseNode::compress() const {
    return compressNode();
}

std::shared_ptr<CHTLJSBaseNode> CHTLJSBaseNode::decompress() const {
    return decompressNode();
}

// 格式化实现
std::string CHTLJSBaseNode::format() const {
    return formatNode();
}

std::string CHTLJSBaseNode::minify() const {
    return minifyNode();
}

std::string CHTLJSBaseNode::beautify() const {
    return beautifyNode();
}

// 辅助方法实现
std::string CHTLJSBaseNode::getNodeTypeName() const {
    switch (nodeType) {
        case CHTLJSNodeType::BASE: return "BASE";
        case CHTLJSNodeType::EXPRESSION: return "EXPRESSION";
        case CHTLJSNodeType::STATEMENT: return "STATEMENT";
        case CHTLJSNodeType::DECLARATION: return "DECLARATION";
        case CHTLJSNodeType::LITERAL: return "LITERAL";
        case CHTLJSNodeType::IDENTIFIER: return "IDENTIFIER";
        case CHTLJSNodeType::BINARY_EXPRESSION: return "BINARY_EXPRESSION";
        case CHTLJSNodeType::UNARY_EXPRESSION: return "UNARY_EXPRESSION";
        case CHTLJSNodeType::TERNARY_EXPRESSION: return "TERNARY_EXPRESSION";
        case CHTLJSNodeType::CALL_EXPRESSION: return "CALL_EXPRESSION";
        case CHTLJSNodeType::MEMBER_EXPRESSION: return "MEMBER_EXPRESSION";
        case CHTLJSNodeType::ARRAY_EXPRESSION: return "ARRAY_EXPRESSION";
        case CHTLJSNodeType::OBJECT_EXPRESSION: return "OBJECT_EXPRESSION";
        case CHTLJSNodeType::FUNCTION_EXPRESSION: return "FUNCTION_EXPRESSION";
        case CHTLJSNodeType::ARROW_FUNCTION_EXPRESSION: return "ARROW_FUNCTION_EXPRESSION";
        case CHTLJSNodeType::EXPRESSION_STATEMENT: return "EXPRESSION_STATEMENT";
        case CHTLJSNodeType::BLOCK_STATEMENT: return "BLOCK_STATEMENT";
        case CHTLJSNodeType::IF_STATEMENT: return "IF_STATEMENT";
        case CHTLJSNodeType::FOR_STATEMENT: return "FOR_STATEMENT";
        case CHTLJSNodeType::WHILE_STATEMENT: return "WHILE_STATEMENT";
        case CHTLJSNodeType::DO_WHILE_STATEMENT: return "DO_WHILE_STATEMENT";
        case CHTLJSNodeType::SWITCH_STATEMENT: return "SWITCH_STATEMENT";
        case CHTLJSNodeType::CASE_STATEMENT: return "CASE_STATEMENT";
        case CHTLJSNodeType::DEFAULT_STATEMENT: return "DEFAULT_STATEMENT";
        case CHTLJSNodeType::BREAK_STATEMENT: return "BREAK_STATEMENT";
        case CHTLJSNodeType::CONTINUE_STATEMENT: return "CONTINUE_STATEMENT";
        case CHTLJSNodeType::RETURN_STATEMENT: return "RETURN_STATEMENT";
        case CHTLJSNodeType::THROW_STATEMENT: return "THROW_STATEMENT";
        case CHTLJSNodeType::TRY_STATEMENT: return "TRY_STATEMENT";
        case CHTLJSNodeType::CATCH_STATEMENT: return "CATCH_STATEMENT";
        case CHTLJSNodeType::FINALLY_STATEMENT: return "FINALLY_STATEMENT";
        case CHTLJSNodeType::VARIABLE_DECLARATION: return "VARIABLE_DECLARATION";
        case CHTLJSNodeType::FUNCTION_DECLARATION: return "FUNCTION_DECLARATION";
        case CHTLJSNodeType::CLASS_DECLARATION: return "CLASS_DECLARATION";
        case CHTLJSNodeType::INTERFACE_DECLARATION: return "INTERFACE_DECLARATION";
        case CHTLJSNodeType::ENUM_DECLARATION: return "ENUM_DECLARATION";
        case CHTLJSNodeType::NAMESPACE_DECLARATION: return "NAMESPACE_DECLARATION";
        case CHTLJSNodeType::MODULE_DECLARATION: return "MODULE_DECLARATION";
        case CHTLJSNodeType::ENHANCED_SELECTOR: return "ENHANCED_SELECTOR";
        case CHTLJSNodeType::VIRTUAL_OBJECT: return "VIRTUAL_OBJECT";
        case CHTLJSNodeType::LISTEN_EXPRESSION: return "LISTEN_EXPRESSION";
        case CHTLJSNodeType::DELEGATE_EXPRESSION: return "DELEGATE_EXPRESSION";
        case CHTLJSNodeType::ANIMATE_EXPRESSION: return "ANIMATE_EXPRESSION";
        case CHTLJSNodeType::ROUTER_EXPRESSION: return "ROUTER_EXPRESSION";
        case CHTLJSNodeType::FILELOADER_EXPRESSION: return "FILELOADER_EXPRESSION";
        case CHTLJSNodeType::UTIL_EXPRESSION: return "UTIL_EXPRESSION";
        case CHTLJSNodeType::COMMENT: return "COMMENT";
        case CHTLJSNodeType::DIRECTIVE: return "DIRECTIVE";
        case CHTLJSNodeType::IMPORT_DECLARATION: return "IMPORT_DECLARATION";
        case CHTLJSNodeType::EXPORT_DECLARATION: return "EXPORT_DECLARATION";
        case CHTLJSNodeType::ERROR: return "ERROR";
        case CHTLJSNodeType::UNKNOWN: return "UNKNOWN";
        default: return "UNKNOWN";
    }
}

std::string CHTLJSBaseNode::formatIndent(int indent) const {
    return std::string(indent * 2, ' ');
}

std::string CHTLJSBaseNode::formatAttributes() const {
    if (attributes.empty()) return "";
    
    std::ostringstream oss;
    oss << "[";
    bool first = true;
    for (const auto& pair : attributes) {
        if (!first) oss << ", ";
        oss << pair.first << "='" << pair.second << "'";
        first = false;
    }
    oss << "]";
    return oss.str();
}

std::string CHTLJSBaseNode::formatChildren(int indent) const {
    if (children.empty()) return "";
    
    std::ostringstream oss;
    for (const auto& child : children) {
        if (child) {
            oss << child->toPrettyString(indent) << "\n";
        }
    }
    return oss.str();
}

std::string CHTLJSBaseNode::formatMetadata() const {
    if (metadata.empty()) return "";
    
    std::ostringstream oss;
    oss << "metadata={";
    bool first = true;
    for (const auto& pair : metadata) {
        if (!first) oss << ", ";
        oss << pair.first << "='" << pair.second << "'";
        first = false;
    }
    oss << "}";
    return oss.str();
}

std::string CHTLJSBaseNode::formatComments() const {
    if (comments.empty()) return "";
    
    std::ostringstream oss;
    oss << "comments=[";
    for (size_t i = 0; i < comments.size(); ++i) {
        if (i > 0) oss << ", ";
        oss << "'" << comments[i] << "'";
    }
    oss << "]";
    return oss.str();
}

std::string CHTLJSBaseNode::formatDirectives() const {
    if (directives.empty()) return "";
    
    std::ostringstream oss;
    oss << "directives=[";
    for (size_t i = 0; i < directives.size(); ++i) {
        if (i > 0) oss << ", ";
        oss << "'" << directives[i] << "'";
    }
    oss << "]";
    return oss.str();
}

// 验证辅助实现
bool CHTLJSBaseNode::validateNode() const {
    return nodeType != CHTLJSNodeType::UNKNOWN && nodeType != CHTLJSNodeType::ERROR;
}

bool CHTLJSBaseNode::validateChildren() const {
    for (const auto& child : children) {
        if (!child || !child->isValid()) {
            return false;
        }
    }
    return true;
}

bool CHTLJSBaseNode::validateAttributes() const {
    // 简化的属性验证
    return true;
}

bool CHTLJSBaseNode::validateMetadata() const {
    // 简化的元数据验证
    return true;
}

// 转换辅助实现
std::string CHTLJSBaseNode::convertToJavaScript() const {
    return toString();
}

std::string CHTLJSBaseNode::convertToCHTLJS() const {
    return toString();
}

std::string CHTLJSBaseNode::convertToHTML() const {
    return toString();
}

std::string CHTLJSBaseNode::convertToCSS() const {
    return toString();
}

std::string CHTLJSBaseNode::convertToJSON() const {
    return toString();
}

// 遍历辅助实现
void CHTLJSBaseNode::traverseNode(std::function<void(std::shared_ptr<CHTLJSBaseNode>)> visitor) {
    visitor(shared_from_this());
    traverseChildren(visitor);
}

void CHTLJSBaseNode::traverseNode(std::function<void(std::shared_ptr<CHTLJSBaseNode>)> visitor) const {
    visitor(const_cast<CHTLJSBaseNode*>(this)->shared_from_this());
    traverseChildren(visitor);
}

// 查找辅助实现
void CHTLJSBaseNode::collectNodes(CHTLJSNodeType type, std::vector<std::shared_ptr<CHTLJSBaseNode>>& result) const {
    if (nodeType == type) {
        result.push_back(const_cast<CHTLJSBaseNode*>(this)->shared_from_this());
    }
    for (const auto& child : children) {
        if (child) {
            child->collectNodes(type, result);
        }
    }
}

void CHTLJSBaseNode::collectNodes(std::function<bool(std::shared_ptr<CHTLJSBaseNode>)> predicate, std::vector<std::shared_ptr<CHTLJSBaseNode>>& result) const {
    if (predicate(const_cast<CHTLJSBaseNode*>(this)->shared_from_this())) {
        result.push_back(const_cast<CHTLJSBaseNode*>(this)->shared_from_this());
    }
    for (const auto& child : children) {
        if (child) {
            child->collectNodes(predicate, result);
        }
    }
}

// 克隆辅助实现
void CHTLJSBaseNode::copyTo(std::shared_ptr<CHTLJSBaseNode> target) const {
    target->nodeType = nodeType;
    target->name = name;
    target->value = value;
    target->attributes = attributes;
    target->line = line;
    target->column = column;
    target->position = position;
    target->typeAnnotation = typeAnnotation;
    target->isOptional = isOptional;
    target->isNullable = isNullable;
    target->scope = scope;
    target->dependencies = dependencies;
    target->metadata = metadata;
    target->comments = comments;
    target->directives = directives;
}

void CHTLJSBaseNode::deepCopyTo(std::shared_ptr<CHTLJSBaseNode> target) const {
    copyTo(target);
    
    for (const auto& child : children) {
        if (child) {
            auto clonedChild = child->deepClone();
            target->addChild(clonedChild);
        }
    }
}

// 比较辅助实现
bool CHTLJSBaseNode::compareNode(std::shared_ptr<CHTLJSBaseNode> other) const {
    return compareNode(*other);
}

bool CHTLJSBaseNode::compareNode(const CHTLJSBaseNode& other) const {
    return nodeType == other.nodeType &&
           name == other.name &&
           value == other.value &&
           compareAttributes(other) &&
           compareChildren(other) &&
           compareMetadata(other);
}

bool CHTLJSBaseNode::compareChildren(std::shared_ptr<CHTLJSBaseNode> other) const {
    return compareChildren(*other);
}

bool CHTLJSBaseNode::compareChildren(const CHTLJSBaseNode& other) const {
    if (children.size() != other.children.size()) {
        return false;
    }
    
    for (size_t i = 0; i < children.size(); ++i) {
        if (!children[i] || !other.children[i]) {
            return false;
        }
        if (!children[i]->equals(other.children[i])) {
            return false;
        }
    }
    
    return true;
}

bool CHTLJSBaseNode::compareAttributes(std::shared_ptr<CHTLJSBaseNode> other) const {
    return compareAttributes(*other);
}

bool CHTLJSBaseNode::compareAttributes(const CHTLJSBaseNode& other) const {
    return attributes == other.attributes;
}

bool CHTLJSBaseNode::compareMetadata(std::shared_ptr<CHTLJSBaseNode> other) const {
    return compareMetadata(*other);
}

bool CHTLJSBaseNode::compareMetadata(const CHTLJSBaseNode& other) const {
    return metadata == other.metadata;
}

// 序列化辅助实现
std::string CHTLJSBaseNode::serializeNode() const {
    // 简化的序列化实现
    return toJSON();
}

bool CHTLJSBaseNode::deserializeNode(const std::string& data) {
    // 简化的反序列化实现
    return false;
}

// 优化辅助实现
std::shared_ptr<CHTLJSBaseNode> CHTLJSBaseNode::optimizeNode() const {
    return clone();
}

std::shared_ptr<CHTLJSBaseNode> CHTLJSBaseNode::compressNode() const {
    return clone();
}

std::shared_ptr<CHTLJSBaseNode> CHTLJSBaseNode::decompressNode() const {
    return clone();
}

// 格式化辅助实现
std::string CHTLJSBaseNode::formatNode() const {
    return toPrettyString(0);
}

std::string CHTLJSBaseNode::minifyNode() const {
    return toString();
}

std::string CHTLJSBaseNode::beautifyNode() const {
    return toPrettyString(0);
}

} // namespace CHTL_JS