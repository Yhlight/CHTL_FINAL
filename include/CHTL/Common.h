#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <optional>
#include <variant>
#include <any>

namespace CHTL {

// 基础类型定义
using String = std::string;
using StringView = std::string_view;
using StringList = std::vector<String>;
using StringMap = std::unordered_map<String, String>;
using StringSet = std::unordered_set<String>;

// 位置信息
struct Position {
    size_t line = 1;
    size_t column = 1;
    size_t offset = 0;
    
    Position() = default;
    Position(size_t l, size_t c, size_t o = 0) : line(l), column(c), offset(o) {}
};

// 源码位置范围
struct SourceLocation {
    Position start;
    Position end;
    
    SourceLocation() = default;
    SourceLocation(const Position& s, const Position& e) : start(s), end(e) {}
};

// 错误类型
enum class ErrorType {
    LexicalError,
    SyntaxError,
    SemanticError,
    RuntimeError,
    ModuleError
};

// 错误信息
struct Error {
    ErrorType type;
    String message;
    SourceLocation location;
    
    Error(ErrorType t, const String& msg, const SourceLocation& loc = {})
        : type(t), message(msg), location(loc) {}
};

// 结果类型
template<typename T>
using Result = std::variant<T, Error>;

// 节点类型枚举
enum class NodeType {
    // 基础节点
    Root,
    Text,
    Comment,
    Element,
    
    // 样式相关
    Style,
    StyleProperty,
    StyleRule,
    
    // 脚本相关
    Script,
    ScriptStatement,
    
    // 模板相关
    Template,
    Custom,
    Import,
    Namespace,
    Configuration,
    
    // 原始嵌入
    Origin,
    
    // 操作符
    Operator,
    
    // CHTL JS相关
    CHTLJSFunction,
    VirtualObject,
    EventBinding,
    Animation,
    Router
};

// 属性类型
enum class AttributeType {
    String,
    Number,
    Boolean,
    Expression,
    Reference
};

// 属性值
struct AttributeValue {
    AttributeType type;
    String value;
    std::any data; // 存储额外数据
    
    AttributeValue(AttributeType t, const String& v) : type(t), value(v) {}
};

// 前向声明
class BaseNode;
class ElementNode;
class TextNode;
class StyleNode;
class ScriptNode;
class TemplateNode;
class CustomNode;
class ImportNode;
class NamespaceNode;
class ConfigurationNode;
class OriginNode;
class OperatorNode;

using NodePtr = std::shared_ptr<BaseNode>;
using ElementNodePtr = std::shared_ptr<ElementNode>;
using TextNodePtr = std::shared_ptr<TextNode>;
using StyleNodePtr = std::shared_ptr<StyleNode>;
using ScriptNodePtr = std::shared_ptr<ScriptNode>;
using TemplateNodePtr = std::shared_ptr<TemplateNode>;
using CustomNodePtr = std::shared_ptr<CustomNode>;
using ImportNodePtr = std::shared_ptr<ImportNode>;
using NamespaceNodePtr = std::shared_ptr<NamespaceNode>;
using ConfigurationNodePtr = std::shared_ptr<ConfigurationNode>;
using OriginNodePtr = std::shared_ptr<OriginNode>;
using OperatorNodePtr = std::shared_ptr<OperatorNode>;

} // namespace CHTL