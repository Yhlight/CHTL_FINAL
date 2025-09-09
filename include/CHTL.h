#pragma once

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <variant>

namespace CHTL {

// Forward declarations
class CHTLContext;
class CHTLGenerator;
class CHTLLexer;
class CHTLLoader;
class CHTLManage;
class CHTLNode;
class CHTLParser;
class CHTLState;
class CHTLIOStream;
class CMODSystem;

// Token types for CHTL lexer
enum class TokenType {
    // Comments
    LINE_COMMENT,    // //
    BLOCK_COMMENT,   // /* */
    GENERATOR_COMMENT, // --
    
    // Literals
    UNQUOTED_LITERAL,  // unquoted string
    SINGLE_QUOTED,     // 'string'
    DOUBLE_QUOTED,     // "string"
    
    // Operators
    COLON,           // :
    EQUALS,          // =
    SEMICOLON,       // ;
    COMMA,           // ,
    DOT,             // .
    HASH,            // #
    
    // Brackets
    LEFT_BRACE,      // {
    RIGHT_BRACE,     // }
    LEFT_BRACKET,    // [
    RIGHT_BRACKET,   // ]
    LEFT_PAREN,      // (
    RIGHT_PAREN,     // )
    
    // Keywords
    TEXT,
    STYLE,
    SCRIPT,
    TEMPLATE,
    CUSTOM,
    ORIGIN,
    IMPORT,
    NAMESPACE,
    CONFIGURATION,
    EXCEPT,
    INHERIT,
    DELETE,
    INSERT,
    AFTER,
    BEFORE,
    REPLACE,
    AT_TOP,
    AT_BOTTOM,
    FROM,
    AS,
    USE,
    HTML5,
    
    // Template types
    AT_STYLE,
    AT_ELEMENT,
    AT_VAR,
    AT_HTML,
    AT_JAVASCRIPT,
    AT_CHTL,
    AT_CONFIG,
    
    // CHTL JS keywords
    FILELOADER,
    LISTEN,
    DELEGATE,
    ANIMATE,
    ROUTER,
    VIR,
    
    // Identifiers
    IDENTIFIER,
    
    // End of file
    EOF_TOKEN
};

// Token structure
struct Token {
    TokenType type;
    std::string value;
    size_t line;
    size_t column;
    
    Token(TokenType t, const std::string& v, size_t l, size_t c)
        : type(t), value(v), line(l), column(c) {}
};

// Node types for AST
enum class NodeType {
    ELEMENT,
    TEXT,
    COMMENT,
    TEMPLATE,
    CUSTOM,
    STYLE,
    SCRIPT,
    ORIGIN,
    IMPORT,
    CONFIG,
    NAMESPACE,
    OPERATOR
};

// Base AST node
class CHTLNode {
public:
    NodeType type;
    std::string name;
    std::string value;
    std::vector<std::unique_ptr<CHTLNode>> children;
    std::map<std::string, std::string> attributes;
    size_t line;
    size_t column;
    
    CHTLNode(NodeType t, const std::string& n = "", const std::string& v = "", size_t l = 0, size_t c = 0)
        : type(t), name(n), value(v), line(l), column(c) {}
    
    virtual ~CHTLNode() = default;
};

// Element node
class ElementNode : public CHTLNode {
public:
    ElementNode(const std::string& name, size_t line = 0, size_t column = 0)
        : CHTLNode(NodeType::ELEMENT, name, "", line, column) {}
};

// Text node
class TextNode : public CHTLNode {
public:
    TextNode(const std::string& value, size_t line = 0, size_t column = 0)
        : CHTLNode(NodeType::TEXT, "text", value, line, column) {}
};

// Comment node
class CommentNode : public CHTLNode {
public:
    bool isGeneratorComment;
    
    CommentNode(const std::string& value, bool isGen = false, size_t line = 0, size_t column = 0)
        : CHTLNode(NodeType::COMMENT, "comment", value, line, column), isGeneratorComment(isGen) {}
};

// Template node
class TemplateNode : public CHTLNode {
public:
    std::string templateType; // @Style, @Element, @Var
    
    TemplateNode(const std::string& name, const std::string& type, size_t line = 0, size_t column = 0)
        : CHTLNode(NodeType::TEMPLATE, name, "", line, column), templateType(type) {}
};

// Custom node
class CustomNode : public CHTLNode {
public:
    std::string customType; // @Style, @Element, @Var
    
    CustomNode(const std::string& name, const std::string& type, size_t line = 0, size_t column = 0)
        : CHTLNode(NodeType::CUSTOM, name, "", line, column), customType(type) {}
};

// Style node
class StyleNode : public CHTLNode {
public:
    bool isLocal; // true for local style blocks
    
    StyleNode(bool local = false, size_t line = 0, size_t column = 0)
        : CHTLNode(NodeType::STYLE, "style", "", line, column), isLocal(local) {}
};

// Script node
class ScriptNode : public CHTLNode {
public:
    bool isLocal; // true for local script blocks
    
    ScriptNode(bool local = false, size_t line = 0, size_t column = 0)
        : CHTLNode(NodeType::SCRIPT, "script", "", line, column), isLocal(local) {}
};

// Origin node
class OriginNode : public CHTLNode {
public:
    std::string originType; // @Html, @Style, @JavaScript, custom types
    
    OriginNode(const std::string& name, const std::string& type, size_t line = 0, size_t column = 0)
        : CHTLNode(NodeType::ORIGIN, name, "", line, column), originType(type) {}
};

// Import node
class ImportNode : public CHTLNode {
public:
    std::string importType;
    std::string importPath;
    std::string alias;
    
    ImportNode(const std::string& type, const std::string& path, const std::string& alias = "", size_t line = 0, size_t column = 0)
        : CHTLNode(NodeType::IMPORT, "import", "", line, column), importType(type), importPath(path), alias(alias) {}
};

// Configuration node
class ConfigNode : public CHTLNode {
public:
    std::string configName;
    std::map<std::string, std::string> configValues;
    
    ConfigNode(const std::string& name = "", size_t line = 0, size_t column = 0)
        : CHTLNode(NodeType::CONFIG, "configuration", "", line, column), configName(name) {}
};

// Namespace node
class NamespaceNode : public CHTLNode {
public:
    std::string namespaceName;
    
    NamespaceNode(const std::string& name, size_t line = 0, size_t column = 0)
        : CHTLNode(NodeType::NAMESPACE, "namespace", "", line, column), namespaceName(name) {}
};

// Operator node (for delete, insert, etc.)
class OperatorNode : public CHTLNode {
public:
    std::string operatorType; // delete, insert, etc.
    
    OperatorNode(const std::string& op, size_t line = 0, size_t column = 0)
        : CHTLNode(NodeType::OPERATOR, op, "", line, column), operatorType(op) {}
};

// Main CHTL compiler class
class CHTLCompiler {
private:
    std::unique_ptr<CHTLContext> context;
    std::unique_ptr<CHTLLexer> lexer;
    std::unique_ptr<CHTLParser> parser;
    std::unique_ptr<CHTLGenerator> generator;
    std::unique_ptr<CHTLLoader> loader;
    std::unique_ptr<CHTLManage> manager;
    std::unique_ptr<CHTLState> state;
    std::unique_ptr<CHTLIOStream> ioStream;
    std::unique_ptr<CMODSystem> cmodSystem;

public:
    CHTLCompiler();
    ~CHTLCompiler();
    
    // Main compilation interface
    bool compile(const std::string& inputFile, const std::string& outputFile);
    bool compileString(const std::string& input, std::string& output);
    
    // Configuration
    void setConfiguration(const std::map<std::string, std::string>& config);
    void loadConfiguration(const std::string& configFile);
    
    // Error handling
    std::vector<std::string> getErrors() const;
    std::vector<std::string> getWarnings() const;
    
    // Module system
    bool loadModule(const std::string& modulePath);
    std::vector<std::string> getAvailableModules() const;
};

} // namespace CHTL