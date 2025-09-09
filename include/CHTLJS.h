#pragma once

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <functional>

namespace CHTLJS {

// Forward declarations
class CHTLJSContext;
class CHTLJSGenerator;
class CHTLJSLexer;
class CHTLJSLoader;
class CHTLJSManage;
class CHTLJSNode;
class CHTLJSParser;
class CHTLJSState;
class CHTLJSIOStream;
class CJMODSystem;

// CHTL JS Token types
enum class CHTLJSTokenType {
    // Literals
    UNQUOTED_LITERAL,
    SINGLE_QUOTED,
    DOUBLE_QUOTED,
    NUMBER,
    BOOLEAN,
    
    // Operators
    DOT,                    // .
    ARROW,                  // ->
    COLON,                  // :
    SEMICOLON,              // ;
    COMMA,                  // ,
    EQUALS,                 // =
    QUESTION,               // ?
    EXCLAMATION,            // !
    AMPERSAND,              // &
    PIPE,                   // |
    PLUS,                   // +
    MINUS,                  // -
    MULTIPLY,               // *
    DIVIDE,                 // /
    MODULO,                 // %
    LESS,                   // <
    GREATER,                // >
    LESS_EQUAL,             // <=
    GREATER_EQUAL,          // >=
    EQUAL_EQUAL,            // ==
    NOT_EQUAL,              // !=
    AND_AND,                // &&
    OR_OR,                  // ||
    
    // Brackets
    LEFT_BRACE,             // {
    RIGHT_BRACE,            // }
    LEFT_BRACKET,           // [
    RIGHT_BRACKET,          // ]
    LEFT_PAREN,             // (
    RIGHT_PAREN,            // )
    
    // Keywords
    FILELOADER,
    LISTEN,
    DELEGATE,
    ANIMATE,
    ROUTER,
    VIR,
    CONST,
    LET,
    VAR,
    FUNCTION,
    IF,
    ELSE,
    FOR,
    WHILE,
    DO,
    SWITCH,
    CASE,
    DEFAULT,
    BREAK,
    CONTINUE,
    RETURN,
    TRY,
    CATCH,
    FINALLY,
    THROW,
    NEW,
    THIS,
    SUPER,
    CLASS,
    EXTENDS,
    IMPLEMENTS,
    INTERFACE,
    ENUM,
    TYPE,
    NAMESPACE,
    MODULE,
    IMPORT,
    EXPORT,
    AS,
    FROM,
    TARGET,
    DURATION,
    EASING,
    BEGIN,
    WHEN,
    AT,
    END,
    LOOP,
    DIRECTION,
    DELAY,
    CALLBACK,
    URL,
    MODE,
    WIDTH,
    HEIGHT,
    SCALE,
    ROOT,
    PAGE,
    
    // CHTL JS specific
    ENHANCED_SELECTOR,      // {{...}}
    VIRTUAL_OBJECT,         // vir
    CHAIN_OPERATOR,         // ->
    
    // Identifiers
    IDENTIFIER,
    
    // End of file
    EOF_TOKEN
};

// CHTL JS Token
struct CHTLJSToken {
    CHTLJSTokenType type;
    std::string value;
    size_t line;
    size_t column;
    
    CHTLJSToken(CHTLJSTokenType t, const std::string& v, size_t l, size_t c)
        : type(t), value(v), line(l), column(c) {}
};

// CHTL JS Node types
enum class CHTLJSNodeType {
    EXPRESSION,
    STATEMENT,
    FUNCTION,
    OBJECT,
    ARRAY,
    ENHANCED_SELECTOR,
    VIRTUAL_OBJECT,
    FILELOADER,
    LISTEN,
    DELEGATE,
    ANIMATE,
    ROUTER,
    VARIABLE,
    ASSIGNMENT,
    CALL,
    MEMBER_ACCESS,
    BINARY_OP,
    UNARY_OP,
    CONDITIONAL,
    LOOP,
    BLOCK
};

// Base CHTL JS AST node
class CHTLJSNode {
public:
    CHTLJSNodeType type;
    std::string name;
    std::string value;
    std::vector<std::unique_ptr<CHTLJSNode>> children;
    std::map<std::string, std::unique_ptr<CHTLJSNode>> properties;
    size_t line;
    size_t column;
    
    CHTLJSNode(CHTLJSNodeType t, const std::string& n = "", const std::string& v = "", size_t l = 0, size_t c = 0)
        : type(t), name(n), value(v), line(l), column(c) {}
    
    virtual ~CHTLJSNode() = default;
};

// Enhanced selector node {{...}}
class EnhancedSelectorNode : public CHTLJSNode {
public:
    std::string selector;
    std::string selectorType; // class, id, tag, etc.
    
    EnhancedSelectorNode(const std::string& sel, size_t line = 0, size_t column = 0)
        : CHTLJSNode(CHTLJSNodeType::ENHANCED_SELECTOR, "selector", "", line, column), selector(sel) {}
};

// Virtual object node (vir)
class VirtualObjectNode : public CHTLJSNode {
public:
    std::string objectName;
    std::map<std::string, std::unique_ptr<CHTLJSNode>> methods;
    
    VirtualObjectNode(const std::string& name, size_t line = 0, size_t column = 0)
        : CHTLJSNode(CHTLJSNodeType::VIRTUAL_OBJECT, "vir", "", line, column), objectName(name) {}
};

// Fileloader node
class FileloaderNode : public CHTLJSNode {
public:
    std::vector<std::string> loadPaths;
    
    FileloaderNode(size_t line = 0, size_t column = 0)
        : CHTLJSNode(CHTLJSNodeType::FILELOADER, "fileloader", "", line, column) {}
};

// Listen node
class ListenNode : public CHTLJSNode {
public:
    std::string target;
    std::map<std::string, std::unique_ptr<CHTLJSNode>> eventHandlers;
    
    ListenNode(const std::string& tgt, size_t line = 0, size_t column = 0)
        : CHTLJSNode(CHTLJSNodeType::LISTEN, "listen", "", line, column), target(tgt) {}
};

// Delegate node
class DelegateNode : public CHTLJSNode {
public:
    std::string parentSelector;
    std::vector<std::string> targetSelectors;
    std::map<std::string, std::unique_ptr<CHTLJSNode>> eventHandlers;
    
    DelegateNode(const std::string& parent, size_t line = 0, size_t column = 0)
        : CHTLJSNode(CHTLJSNodeType::DELEGATE, "delegate", "", line, column), parentSelector(parent) {}
};

// Animate node
class AnimateNode : public CHTLJSNode {
public:
    std::string target;
    int duration;
    std::string easing;
    std::map<std::string, std::string> beginStyles;
    std::vector<std::map<std::string, std::string>> whenStyles;
    std::map<std::string, std::string> endStyles;
    int loop;
    std::string direction;
    int delay;
    std::string callback;
    
    AnimateNode(size_t line = 0, size_t column = 0)
        : CHTLJSNode(CHTLJSNodeType::ANIMATE, "animate", "", line, column), duration(0), loop(0), delay(0) {}
};

// Router node
class RouterNode : public CHTLJSNode {
public:
    std::string root;
    std::string mode; // history, hash
    std::map<std::string, std::string> routes; // url -> page selector
    
    RouterNode(size_t line = 0, size_t column = 0)
        : CHTLJSNode(CHTLJSNodeType::ROUTER, "router", "", line, column) {}
};

// CJMOD API classes (Rust equivalent)
class Syntax {
public:
    static std::vector<std::string> analyze(const std::string& pattern);
    static bool isObject(const std::string& code);
    static bool isFunction(const std::string& code);
    static bool isArray(const std::string& code);
    static bool isCHTLJSFunction(const std::string& code);
};

class Arg {
public:
    std::vector<std::string> values;
    std::map<std::string, std::function<std::string(const std::string&)>> bindings;
    
    void bind(const std::string& key, std::function<std::string(const std::string&)> func);
    void fillValue(const std::vector<std::string>& values);
    void transform(const std::string& pattern);
    void print() const;
};

class CJMODScanner {
public:
    static Arg scan(const Arg& args, const std::string& keyword);
};

class CJMODGenerator {
public:
    static void exportResult(const Arg& args);
};

class CHTLJSFunction {
public:
    static std::unique_ptr<CHTLJSFunction> CreateCHTLJSFunction(const std::string& pattern);
    static void bindVirtualObject(const std::string& functionName);
    
    std::string functionName;
    std::string pattern;
    std::map<std::string, std::string> parameters;
};

// Main CHTL JS compiler class
class CHTLJSCompiler {
private:
    std::unique_ptr<CHTLJSContext> context;
    std::unique_ptr<CHTLJSLexer> lexer;
    std::unique_ptr<CHTLJSParser> parser;
    std::unique_ptr<CHTLJSGenerator> generator;
    std::unique_ptr<CHTLJSLoader> loader;
    std::unique_ptr<CHTLJSManage> manager;
    std::unique_ptr<CHTLJSState> state;
    std::unique_ptr<CHTLJSIOStream> ioStream;
    std::unique_ptr<CJMODSystem> cjmodSystem;

public:
    CHTLJSCompiler();
    ~CHTLJSCompiler();
    
    // Main compilation interface
    bool compile(const std::string& input, std::string& output);
    
    // Error handling
    std::vector<std::string> getErrors() const;
    std::vector<std::string> getWarnings() const;
    
    // Module system
    bool loadCJMOD(const std::string& modulePath);
    std::vector<std::string> getAvailableCJMODs() const;
};

} // namespace CHTLJS