#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <optional>
#include <variant>
#include <any>

namespace CHTL_JS {

// 前向声明
class CHTLJSNode;
class CHTLJSContext;
class CHTLJSLexer;
class CHTLJSParser;
class CHTLJSGenerator;

// 基础类型定义
using String = std::string;
using StringView = std::string_view;
using StringList = std::vector<String>;
using StringSet = std::unordered_set<String>;

// CHTL JS 节点类型枚举
enum class CHTLJSNodeType {
    // 基础节点
    Identifier,
    Literal,
    Expression,
    Statement,
    Block,
    
    // 函数节点
    Function,
    FunctionCall,
    ArrowFunction,
    
    // 对象节点
    Object,
    ObjectProperty,
    Array,
    ArrayElement,
    
    // 控制流节点
    IfStatement,
    ForStatement,
    WhileStatement,
    SwitchStatement,
    CaseStatement,
    BreakStatement,
    ContinueStatement,
    ReturnStatement,
    
    // 变量节点
    VariableDeclaration,
    VariableAssignment,
    
    // 操作符节点
    BinaryOperator,
    UnaryOperator,
    TernaryOperator,
    
    // CHTL JS 特定节点
    VirtualObject,
    Listen,
    Delegate,
    Animate,
    Router,
    FileLoader,
    UtilThen,
    PrintMyLove,
    INeverAway
};

// 位置信息
struct Position {
    size_t line = 1;
    size_t column = 1;
    size_t offset = 0;
    
    Position() = default;
    Position(size_t l, size_t c, size_t o = 0)
        : line(l), column(c), offset(o) {}
};

// 错误信息
struct ErrorInfo {
    String message;
    Position position;
    String severity = "error";
    
    ErrorInfo() = default;
    ErrorInfo(const String& msg, const Position& pos, const String& sev = "error")
        : message(msg), position(pos), severity(sev) {}
};

using ErrorList = std::vector<ErrorInfo>;

// 字面量类型
enum class LiteralType {
    String,
    Number,
    Boolean,
    Null,
    Undefined,
    Array,
    Object
};

// 字面量值
struct LiteralValue {
    LiteralType type;
    String value;
    
    LiteralValue() : type(LiteralType::Null) {}
    LiteralValue(LiteralType t, const String& v) : type(t), value(v) {}
};

// 操作符类型
enum class OperatorType {
    // 算术操作符
    Add,        // +
    Subtract,   // -
    Multiply,   // *
    Divide,     // /
    Modulo,     // %
    Power,      // **
    
    // 比较操作符
    Equal,      // ==
    NotEqual,   // !=
    StrictEqual, // ===
    StrictNotEqual, // !==
    Less,       // <
    LessEqual,  // <=
    Greater,    // >
    GreaterEqual, // >=
    
    // 逻辑操作符
    And,        // &&
    Or,         // ||
    Not,        // !
    
    // 赋值操作符
    Assign,     // =
    AddAssign,  // +=
    SubtractAssign, // -=
    MultiplyAssign, // *=
    DivideAssign,   // /=
    ModuloAssign,   // %=
    
    // 其他操作符
    Dot,        // .
    Arrow,      // ->
    Question,   // ?
    Colon,      // :
    Comma,      // ,
    Semicolon,  // ;
    LeftParen,  // (
    RightParen, // )
    LeftBracket, // [
    RightBracket, // ]
    LeftBrace,  // {
    RightBrace  // }
};

// 选择器类型
enum class SelectorType {
    Tag,        // div
    Class,      // .class
    Id,         // #id
    Descendant, // .parent .child
    Index       // div[0]
};

// 选择器
struct Selector {
    SelectorType type;
    String value;
    int index = -1;
    
    Selector() = default;
    Selector(SelectorType t, const String& v, int i = -1)
        : type(t), value(v), index(i) {}
};

// 事件类型
enum class EventType {
    Click,
    MouseEnter,
    MouseLeave,
    MouseMove,
    MouseOver,
    MouseOut,
    KeyDown,
    KeyUp,
    KeyPress,
    Focus,
    Blur,
    Change,
    Submit,
    Load,
    Unload,
    Resize,
    Scroll
};

// 动画缓动函数
enum class EasingFunction {
    Linear,
    EaseIn,
    EaseOut,
    EaseInOut,
    EaseInQuad,
    EaseOutQuad,
    EaseInOutQuad,
    EaseInCubic,
    EaseOutCubic,
    EaseInOutCubic,
    EaseInQuart,
    EaseOutQuart,
    EaseInOutQuart,
    EaseInQuint,
    EaseOutQuint,
    EaseInOutQuint,
    EaseInSine,
    EaseOutSine,
    EaseInOutSine,
    EaseInExpo,
    EaseOutExpo,
    EaseInOutExpo,
    EaseInCirc,
    EaseOutCirc,
    EaseInOutCirc,
    EaseInBack,
    EaseOutBack,
    EaseInOutBack,
    EaseInElastic,
    EaseOutElastic,
    EaseInOutElastic,
    EaseInBounce,
    EaseOutBounce,
    EaseInOutBounce
};

// 动画方向
enum class AnimationDirection {
    Normal,
    Reverse,
    Alternate,
    AlternateReverse
};

// 动画关键帧
struct AnimationKeyframe {
    double at;  // 时间点 (0.0 - 1.0)
    std::unordered_map<String, String> properties;
    
    AnimationKeyframe() : at(0.0) {}
    AnimationKeyframe(double t) : at(t) {}
};

// 动画配置
struct AnimationConfig {
    String target;
    int duration = 1000;
    EasingFunction easing = EasingFunction::EaseInOut;
    std::unordered_map<String, String> begin;
    std::vector<AnimationKeyframe> when;
    std::unordered_map<String, String> end;
    int loop = 1;
    AnimationDirection direction = AnimationDirection::Normal;
    int delay = 0;
    String callback;
    
    AnimationConfig() = default;
};

// 路由配置
struct RouteConfig {
    String url;
    String page;
    String root = "/";
    String mode = "history"; // "history" or "hash"
    
    RouteConfig() = default;
    RouteConfig(const String& u, const String& p) : url(u), page(p) {}
};

// 文件加载器配置
struct FileLoaderConfig {
    StringList files;
    
    FileLoaderConfig() = default;
};

// 虚对象配置
struct VirtualObjectConfig {
    String name;
    std::unordered_map<String, String> properties;
    
    VirtualObjectConfig() = default;
    VirtualObjectConfig(const String& n) : name(n) {}
};

// 监听器配置
struct ListenerConfig {
    String target;
    std::unordered_map<String, String> events;
    
    ListenerConfig() = default;
};

// 事件委托配置
struct DelegateConfig {
    String parent;
    StringList targets;
    std::unordered_map<String, String> events;
    
    DelegateConfig() = default;
};

// 工具函数配置
struct UtilConfig {
    String condition;
    String change;
    String then;
    
    UtilConfig() = default;
};

// 打印配置
struct PrintConfig {
    String url;
    String mode = "auto"; // "auto", "ascii", "pixel"
    String width = "100%";
    String height = "100%";
    double scale = 1.0;
    
    PrintConfig() = default;
};

// 永不离开配置
struct INeverAwayConfig {
    String name;
    std::unordered_map<String, String> functions;
    
    INeverAwayConfig() = default;
    INeverAwayConfig(const String& n) : name(n) {}
};

} // namespace CHTL_JS