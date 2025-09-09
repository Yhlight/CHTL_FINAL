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

// 前向声明
class CHTLNode;
class CHTLContext;
class CHTLLexer;
class CHTLParser;
class CHTLGenerator;

// 基础类型定义
using String = std::string;
using StringView = std::string_view;
using StringList = std::vector<String>;
using StringSet = std::unordered_set<String>;

// 节点类型枚举
enum class NodeType {
    // 基础节点
    Text,
    Element,
    Comment,
    
    // 模板节点
    TemplateStyle,
    TemplateElement,
    TemplateVar,
    
    // 自定义节点
    CustomStyle,
    CustomElement,
    CustomVar,
    
    // 其他节点
    Style,
    Script,
    Origin,
    Import,
    Config,
    Namespace,
    Operator
};

// 属性类型
struct Attribute {
    String name;
    String value;
    bool isQuoted = false;
    
    Attribute() = default;
    Attribute(const String& n, const String& v, bool quoted = false)
        : name(n), value(v), isQuoted(quoted) {}
};

using AttributeList = std::vector<Attribute>;
using AttributeMap = std::unordered_map<String, String>;

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

// 配置选项
struct ConfigOptions {
    bool debugMode = false;
    int indexInitialCount = 0;
    bool disableStyleAutoAddClass = false;
    bool disableStyleAutoAddId = false;
    bool disableScriptAutoAddClass = true;
    bool disableScriptAutoAddId = true;
    bool disableDefaultNamespace = false;
    bool disableCustomOriginType = false;
    bool disableNameGroup = false;
    
    // 自定义关键字
    StringList customStyleKeywords = {"@Style", "@style", "@CSS", "@Css", "@css"};
    String customElementKeyword = "@Element";
    String customVarKeyword = "@Var";
    String templateStyleKeyword = "@Style";
    String templateElementKeyword = "@Element";
    String templateVarKeyword = "@Var";
    String originHtmlKeyword = "@Html";
    String originStyleKeyword = "@Style";
    String originJavascriptKeyword = "@JavaScript";
    String importHtmlKeyword = "@Html";
    String importStyleKeyword = "@Style";
    String importJavascriptKeyword = "@JavaScript";
    String importChtlKeyword = "@Chtl";
    String importCjmodKeyword = "@CJmod";
    String keywordInherit = "inherit";
    String keywordDelete = "delete";
    String keywordInsert = "insert";
    String keywordAfter = "after";
    String keywordBefore = "before";
    String keywordReplace = "replace";
    String keywordAtTop = "at top";
    String keywordAtBottom = "at bottom";
    String keywordFrom = "from";
    String keywordAs = "as";
    String keywordExcept = "except";
    String keywordUse = "use";
    String keywordHtml5 = "html5";
    String keywordText = "text";
    String keywordStyle = "style";
    String keywordScript = "script";
    String keywordCustom = "[Custom]";
    String keywordTemplate = "[Template]";
    String keywordOrigin = "[Origin]";
    String keywordImport = "[Import]";
    String keywordNamespace = "[Namespace]";
    String keywordConfiguration = "[Configuration]";
    
    int optionCount = 3;
};

// 命名空间信息
struct NamespaceInfo {
    String name;
    String parent;
    std::unordered_map<String, std::shared_ptr<CHTLNode>> elements;
    
    NamespaceInfo() = default;
    NamespaceInfo(const String& n, const String& p = "")
        : name(n), parent(p) {}
};

// 模块信息
struct ModuleInfo {
    String name;
    String version;
    String description;
    String author;
    String license;
    String dependencies;
    String category;
    String minCHTLVersion;
    String maxCHTLVersion;
    
    ModuleInfo() = default;
};

// 导出信息
struct ExportInfo {
    StringList customStyles;
    StringList customElements;
    StringList customVars;
    StringList templateStyles;
    StringList templateElements;
    StringList templateVars;
    StringList originHtmls;
    StringList originStyles;
    StringList originJavascripts;
    StringList configurations;
    
    ExportInfo() = default;
};

} // namespace CHTL