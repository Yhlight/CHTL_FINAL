# CHTL API Reference

This document provides comprehensive API reference for the CHTL (C++ HyperText Language) compiler and related tools.

## Table of Contents

1. [Core Compiler API](#core-compiler-api)
2. [Module System API](#module-system-api)
3. [CHTL JS API](#chtl-js-api)
4. [CLI API](#cli-api)
5. [VSCode Extension API](#vscode-extension-api)
6. [Configuration API](#configuration-api)
7. [Utility Classes](#utility-classes)

## Core Compiler API

### CHTLLexer

The lexical analyzer for CHTL source code.

```cpp
namespace CHTL {

class CHTLLexer {
public:
    // Constructor
    explicit CHTLLexer(const std::string& source);
    
    // Main tokenization method
    std::vector<Token> tokenize();
    
    // Utility methods
    bool hasMoreTokens() const;
    Token peekToken() const;
    Token nextToken();
    
    // Error handling
    std::vector<std::string> getErrors() const;
    bool hasErrors() const;
    
private:
    std::string source;
    size_t position;
    size_t line;
    size_t column;
    std::vector<Token> tokens;
    std::vector<std::string> errors;
    
    // Internal scanning methods
    Token scanIdentifier();
    Token scanString();
    Token scanNumber();
    Token scanOperator();
    Token scanComment();
    void skipWhitespace();
    void throwError(const std::string& message);
};

}
```

#### Token Types

```cpp
enum class TokenType {
    // Basic types
    IDENTIFIER,     // Variable names, keywords
    STRING,         // String literals
    NUMBER,         // Numeric literals
    COMMENT,        // Comments
    GENERATOR_COMMENT, // Generator comments (--)
    
    // Delimiters
    LEFT_BRACE,     // {
    RIGHT_BRACE,    // }
    LEFT_BRACKET,   // [
    RIGHT_BRACKET,  // ]
    LEFT_PAREN,     // (
    RIGHT_PAREN,    // )
    SEMICOLON,      // ;
    COMMA,          // ,
    DOT,            // .
    COLON,          // :
    EQUAL,          // =
    
    // Operators
    PLUS,           // +
    MINUS,          // -
    MULTIPLY,       // *
    DIVIDE,         // /
    MODULO,         // %
    AND,            // &&
    OR,             // ||
    NOT,            // !
    GREATER,        // >
    LESS,           // <
    GREATER_EQUAL,  // >=
    LESS_EQUAL,     // <=
    EQUAL_EQUAL,    // ==
    NOT_EQUAL,      // !=
    ARROW,          // ->
    
    // Keywords
    TEMPLATE,       // [Template]
    CUSTOM,         // [Custom]
    ORIGIN,         // [Origin]
    IMPORT,         // [Import]
    NAMESPACE,      // [Namespace]
    CONFIGURATION,  // [Configuration]
    INFO,           // [Info]
    EXPORT,         // [Export]
    
    // CHTL keywords
    TEXT,           // text
    STYLE,          // style
    SCRIPT,         // script
    USE,            // use
    EXCEPT,         // except
    DELETE,         // delete
    INSERT,         // insert
    INHERIT,        // inherit
    FROM,           // from
    AS,             // as
    
    // CHTL JS keywords
    FILELOADER,     // fileloader
    LISTEN,         // listen
    DELEGATE,       // delegate
    ANIMATE,        // animate
    VIR,            // vir
    ROUTER,         // router
    UTIL,           // util
    INEVERAWAY,     // iNeverAway
    
    // Special
    NEWLINE,        // Newline
    WHITESPACE,     // Whitespace
    EOF_TOKEN,      // End of file
    ERROR           // Error token
};
```

#### Token Structure

```cpp
struct Token {
    TokenType type;
    std::string value;
    size_t line;
    size_t column;
    size_t position;
    
    Token(TokenType type, const std::string& value, 
          size_t line, size_t column, size_t position);
    std::string toString() const;
};
```

### CHTLParser

The syntactic analyzer for CHTL source code.

```cpp
namespace CHTL {

class CHTLParser {
public:
    // Constructor
    explicit CHTLParser(const std::vector<Token>& tokens);
    
    // Main parsing method
    std::shared_ptr<BaseNode> parse();
    
    // Utility methods
    bool hasMoreTokens() const;
    Token peekToken() const;
    Token nextToken();
    bool match(TokenType type) const;
    void consume(TokenType type);
    
    // Error handling
    std::vector<std::string> getErrors() const;
    bool hasErrors() const;
    
private:
    std::vector<Token> tokens;
    size_t currentTokenIndex;
    std::vector<std::string> errors;
    
    // Parsing methods
    std::shared_ptr<BaseNode> parseElement();
    std::shared_ptr<BaseNode> parseTemplate();
    std::shared_ptr<BaseNode> parseCustom();
    std::shared_ptr<BaseNode> parseImport();
    std::shared_ptr<BaseNode> parseNamespace();
    std::shared_ptr<BaseNode> parseConfiguration();
    std::shared_ptr<BaseNode> parseStyleBlock();
    std::shared_ptr<BaseNode> parseScriptBlock();
    void throwError(const std::string& message);
};

}
```

### CHTLGenerator

The code generator for CHTL AST.

```cpp
namespace CHTL {

class CHTLGenerator {
public:
    // Constructor
    CHTLGenerator();
    
    // Main generation method
    std::string generate(std::shared_ptr<BaseNode> ast);
    
    // Output options
    void setMinify(bool minify);
    void setSourceMap(bool sourceMap);
    void setOutputFormat(const std::string& format);
    
    // Utility methods
    std::string generateHTML(std::shared_ptr<BaseNode> ast);
    std::string generateCSS(std::shared_ptr<BaseNode> ast);
    std::string generateJS(std::shared_ptr<BaseNode> ast);
    
private:
    bool minify;
    bool sourceMap;
    std::string outputFormat;
    
    // Generation methods
    std::string generateNode(std::shared_ptr<BaseNode> node, int indent = 0);
    std::string generateElement(std::shared_ptr<ElementNode> node, int indent);
    std::string generateTemplate(std::shared_ptr<TemplateNode> node, int indent);
    std::string generateCustom(std::shared_ptr<CustomNode> node, int indent);
    std::string generateStyle(std::shared_ptr<StyleNode> node, int indent);
    std::string generateScript(std::shared_ptr<ScriptNode> node, int indent);
    std::string getIndent(int level);
};

}
```

## Module System API

### CMODModule

CHTL Module for templates and custom elements.

```cpp
namespace CHTL {

struct CMODInfo {
    std::string name;
    std::string version;
    std::string description;
    std::string author;
    std::string license;
    std::vector<std::string> dependencies;
    std::map<std::string, std::string> metadata;
};

enum class ExportType {
    Template,
    Custom,
    Style,
    Element,
    Var,
    Namespace,
    Configuration
};

struct ExportItem {
    ExportType type;
    std::string name;
    std::string description;
    std::shared_ptr<BaseNode> node;
    bool isPublic;
};

class CMODModule {
public:
    // Constructor
    CMODModule();
    
    // Module information
    void setInfo(const CMODInfo& info);
    const CMODInfo& getInfo() const;
    
    // Source management
    void setSource(const std::string& source);
    const std::string& getSource() const;
    
    // Export management
    void addExport(const ExportItem& item);
    void removeExport(const std::string& name);
    bool hasExport(const std::string& name) const;
    ExportItem getExport(const std::string& name) const;
    std::vector<ExportItem> getAllExports() const;
    std::vector<ExportItem> getPublicExports() const;
    
    // Submodule management
    void addSubmodule(const std::string& name, std::shared_ptr<CMODModule> submodule);
    void removeSubmodule(const std::string& name);
    bool hasSubmodule(const std::string& name) const;
    std::shared_ptr<CMODModule> getSubmodule(const std::string& name) const;
    std::vector<std::string> getSubmoduleNames() const;
    
    // Dependency management
    void addDependency(const std::string& moduleName, const std::string& version = "");
    void removeDependency(const std::string& moduleName);
    bool hasDependency(const std::string& moduleName) const;
    std::vector<std::string> getDependencies() const;
    
    // Validation
    bool isValid() const;
    std::vector<std::string> getValidationErrors() const;
    
    // Serialization
    std::string toJSON() const;
    std::string toString() const;
};

}
```

### CJMODModule

CHTL JS Module for JavaScript extensions.

```cpp
namespace CHTL {

struct CJMODInfo {
    std::string name;
    std::string version;
    std::string description;
    std::string author;
    std::string license;
    std::vector<std::string> dependencies;
    std::map<std::string, std::string> metadata;
};

enum class CJMODFunctionType {
    Utility,
    EventHandler,
    Animation,
    Router,
    VirtualObject,
    Custom
};

struct CJMODFunction {
    std::string name;
    std::string description;
    CJMODFunctionType type;
    std::vector<std::string> parameters;
    std::string returnType;
    std::function<void()> implementation;
    bool isPublic;
};

class CJMODModule {
public:
    // Constructor
    CJMODModule();
    
    // Module information
    void setInfo(const CJMODInfo& info);
    const CJMODInfo& getInfo() const;
    
    // Source management
    void setSource(const std::string& source);
    const std::string& getSource() const;
    
    // Function management
    void addFunction(const CJMODFunction& func);
    void removeFunction(const std::string& name);
    bool hasFunction(const std::string& name) const;
    CJMODFunction getFunction(const std::string& name) const;
    std::vector<CJMODFunction> getAllFunctions() const;
    std::vector<CJMODFunction> getPublicFunctions() const;
    std::vector<CJMODFunction> getFunctionsByType(CJMODFunctionType type) const;
    
    // Dependency management
    void addDependency(const std::string& moduleName, const std::string& version = "");
    void removeDependency(const std::string& moduleName);
    bool hasDependency(const std::string& moduleName) const;
    std::vector<std::string> getDependencies() const;
    
    // Validation
    bool isValid() const;
    std::vector<std::string> getValidationErrors() const;
    
    // Serialization
    std::string toJSON() const;
    std::string toString() const;
};

}
```

### HybridModuleSystem

System for managing hybrid CMOD/CJMOD modules.

```cpp
namespace CHTL {

enum class HybridModuleType {
    CMOD_ONLY,
    CJMOD_ONLY,
    HYBRID
};

struct HybridModuleInfo {
    std::string name;
    std::string version;
    std::string description;
    std::string author;
    std::string license;
    HybridModuleType type;
    std::vector<std::string> dependencies;
    std::map<std::string, std::string> metadata;
};

class HybridModule {
public:
    // Constructor
    HybridModule();
    
    // Module information
    void setInfo(const HybridModuleInfo& info);
    const HybridModuleInfo& getInfo() const;
    
    // CMOD part
    void setCMODModule(std::shared_ptr<CMODModule> cmod);
    std::shared_ptr<CMODModule> getCMODModule() const;
    bool hasCMODPart() const;
    
    // CJMOD part
    void setCJMODModule(std::shared_ptr<CJMODModule> cjmod);
    std::shared_ptr<CJMODModule> getCJMODModule() const;
    bool hasCJMODPart() const;
    
    // Dependency management
    void addDependency(const std::string& moduleName, const std::string& version = "");
    void removeDependency(const std::string& moduleName);
    bool hasDependency(const std::string& moduleName) const;
    std::vector<std::string> getDependencies() const;
    
    // Validation
    bool isValid() const;
    std::vector<std::string> getValidationErrors() const;
    
    // Serialization
    std::string toJSON() const;
    std::string toString() const;
};

class HybridModuleSystem {
public:
    // Constructor
    HybridModuleSystem();
    
    // Module management
    void registerModule(std::shared_ptr<HybridModule> module);
    void unregisterModule(const std::string& name);
    bool hasModule(const std::string& name) const;
    std::shared_ptr<HybridModule> getModule(const std::string& name) const;
    std::vector<std::string> getAllModuleNames() const;
    
    // Module search
    std::vector<std::string> searchModules(const std::string& query) const;
    std::vector<std::string> findModulesByType(HybridModuleType type) const;
    
    // Dependency resolution
    std::vector<std::string> resolveDependencies(const std::string& moduleName) const;
    bool checkDependencyConflicts(const std::string& moduleName) const;
    
    // Module loading
    std::shared_ptr<HybridModule> loadModule(const std::string& path);
    bool saveModule(std::shared_ptr<HybridModule> module, const std::string& path);
    
    // Function calling
    bool callFunction(const std::string& moduleName, const std::string& functionName, 
                     const std::vector<std::string>& args = {});
    
    // Export access
    std::vector<ExportItem> getExports(const std::string& moduleName) const;
    
    // Module information
    std::map<std::string, HybridModuleInfo> getAllModuleInfo() const;
    
    // Subsystem access
    CMODSystem& getCMODSystem();
    CJMODSystem& getCJMODSystem();
};

}
```

## CHTL JS API

### CHTLJSLexer

Lexical analyzer for CHTL JS code.

```cpp
namespace CHTL {
namespace CHTLJS {

enum class CHTLJSTokenType {
    // Basic types
    IDENTIFIER,
    STRING,
    NUMBER,
    
    // Keywords
    FILELOADER, LISTEN, DELEGATE, ANIMATE, VIR, ROUTER, UTIL, INEVERAWAY,
    TARGET, DURATION, EASING, BEGIN, WHEN, END, LOOP, DIRECTION, DELAY, 
    CALLBACK, URL, MODE, WIDTH, HEIGHT, SCALE, ROOT, PAGE,
    
    // Operators and Punctuation
    LEFT_BRACE, RIGHT_BRACE, LEFT_PAREN, RIGHT_PAREN, LEFT_BRACKET, RIGHT_BRACKET,
    COLON, SEMICOLON, COMMA, DOT, ARROW,
    EQUAL, PLUS, MINUS, MULTIPLY, DIVIDE, MODULO,
    AND, OR, NOT, GREATER, LESS, GREATER_EQUAL, LESS_EQUAL, EQUAL_EQUAL, NOT_EQUAL,
    
    // CHTL JS Specific
    SELECTOR_START, // {{
    SELECTOR_END,   // }}
    
    // Special
    EOF_TOKEN,
    ERROR
};

struct CHTLJSToken {
    CHTLJSTokenType type;
    std::string value;
    size_t line;
    size_t column;
    size_t position;
    
    CHTLJSToken(CHTLJSTokenType type, std::string value, 
                size_t line, size_t column, size_t position);
    std::string toString() const;
};

class CHTLJSLexer {
public:
    explicit CHTLJSLexer(std::string source);
    std::vector<CHTLJSToken> tokenize();
    
private:
    std::string source;
    size_t position;
    size_t line;
    size_t column;
    std::vector<CHTLJSToken> tokens;
    
    // Internal methods
    char currentChar() const;
    char peekChar(size_t offset = 1) const;
    void advance();
    void skipWhitespace();
    void throwError(const std::string& message) const;
    
    CHTLJSToken scanString();
    CHTLJSToken scanNumber();
    CHTLJSToken scanIdentifierOrKeyword();
    CHTLJSToken scanOperatorOrDelimiter();
    CHTLJSToken scanSelector();
    
    bool isAlpha(char c) const;
    bool isDigit(char c) const;
    bool isAlphanumeric(char c) const;
    bool isWhitespace(char c) const;
    bool isNewline(char c) const;
    bool isStringDelimiter(char c) const;
    bool isSelectorStart() const;
};

}
}
```

### CHTLJSParser

Syntactic analyzer for CHTL JS code.

```cpp
namespace CHTL {
namespace CHTLJS {

class CHTLJSParser {
public:
    explicit CHTLJSParser(const std::vector<CHTLJSToken>& tokens);
    std::shared_ptr<CHTLJSBaseNode> parse();
    
private:
    const std::vector<CHTLJSToken>& tokens;
    size_t currentTokenIndex;
    
    // Utility methods
    CHTLJSToken peek(int offset = 0) const;
    CHTLJSToken consume(CHTLJSTokenType type);
    CHTLJSToken consumeIdentifier();
    bool match(CHTLJSTokenType type);
    void throwError(const std::string& message) const;
    
    // Parsing methods
    std::shared_ptr<CHTLJSBaseNode> parseProgram();
    std::shared_ptr<CHTLJSBaseNode> parseStatement();
    std::shared_ptr<CHTLJSBaseNode> parseFileloader();
    std::shared_ptr<CHTLJSBaseNode> parseListen();
    std::shared_ptr<CHTLJSBaseNode> parseDelegate();
    std::shared_ptr<CHTLJSBaseNode> parseAnimate();
    std::shared_ptr<CHTLJSBaseNode> parseSelector();
};

}
}
```

### CHTLJSGenerator

Code generator for CHTL JS AST.

```cpp
namespace CHTL {
namespace CHTLJS {

class CHTLJSGenerator {
public:
    CHTLJSGenerator();
    std::string generate(std::shared_ptr<CHTLJSBaseNode> ast);
    
private:
    std::string generateNode(std::shared_ptr<CHTLJSBaseNode> node, int indent = 0);
    std::string generateFileloader(std::shared_ptr<CHTLJSFileloaderNode> node, int indent);
    std::string generateSelector(std::shared_ptr<CHTLJSSelectorNode> node, int indent);
    std::string getIndent(int indent);
};

}
}
```

## CLI API

### CHTLCli

Command-line interface for CHTL tools.

```cpp
namespace CHTL {

enum class ArgType {
    String,
    Number,
    Boolean,
    File,
    Directory
};

struct CliArgument {
    std::string name;
    std::string shortName;
    ArgType type;
    std::string description;
    std::string defaultValue;
    bool isRequired;
    bool isMultiple;
};

struct CliCommand {
    std::string name;
    std::string description;
    std::vector<CliArgument> arguments;
    std::function<int(const std::map<std::string, std::string>&)> handler;
};

class CHTLCli {
public:
    // Constructor
    CHTLCli();
    
    // Command management
    void addCommand(const CliCommand& command);
    void removeCommand(const std::string& name);
    bool hasCommand(const std::string& name) const;
    CliCommand getCommand(const std::string& name) const;
    std::vector<std::string> getAllCommandNames() const;
    
    // Argument management
    void addGlobalArgument(const CliArgument& argument);
    void removeGlobalArgument(const std::string& name);
    bool hasGlobalArgument(const std::string& name) const;
    
    // Execution
    int execute(int argc, char* argv[]);
    int execute(const std::vector<std::string>& args);
    
    // Help
    void printHelp() const;
    void printCommandHelp(const std::string& commandName) const;
    void printVersion() const;
    
    // Configuration
    void setProgramName(const std::string& name);
    void setVersion(const std::string& version);
    void setDescription(const std::string& description);
    
    // Utility methods
    static std::string getCurrentDirectory();
    static bool fileExists(const std::string& path);
    static bool directoryExists(const std::string& path);
    static std::string readFile(const std::string& path);
    static bool writeFile(const std::string& path, const std::string& content);
    
private:
    std::string programName;
    std::string version;
    std::string description;
    std::map<std::string, CliCommand> commands;
    std::map<std::string, CliArgument> globalArguments;
    
    // Built-in commands
    void initializeBuiltinCommands();
    int handleCompile(const std::map<std::string, std::string>& args);
    int handleWatch(const std::map<std::string, std::string>& args);
    int handleInit(const std::map<std::string, std::string>& args);
    int handleBuild(const std::map<std::string, std::string>& args);
    int handleClean(const std::map<std::string, std::string>& args);
    int handleTest(const std::map<std::string, std::string>& args);
    int handleServe(const std::map<std::string, std::string>& args);
    int handleHelp(const std::map<std::string, std::string>& args);
    int handleVersion(const std::map<std::string, std::string>& args);
    
    // Argument parsing
    std::map<std::string, std::string> parseArguments(
        const std::vector<std::string>& args, 
        const std::vector<CliArgument>& commandArgs) const;
    bool validateArguments(
        const std::map<std::string, std::string>& parsedArgs,
        const std::vector<CliArgument>& commandArgs) const;
    
    // Output
    void printError(const std::string& message) const;
    void printInfo(const std::string& message) const;
    void printSuccess(const std::string& message) const;
};

}
```

## VSCode Extension API

### TypeScript Interfaces

```typescript
// Extension activation
export function activate(context: vscode.ExtensionContext): void;

// Extension deactivation
export function deactivate(): void;

// Language providers
export class CHTLProvider implements 
    vscode.CompletionItemProvider,
    vscode.HoverProvider,
    vscode.DefinitionProvider,
    vscode.ReferenceProvider,
    vscode.RenameProvider {
    
    // Completion
    provideCompletionItems(
        document: vscode.TextDocument,
        position: vscode.Position,
        token: vscode.CancellationToken,
        context: vscode.CompletionContext
    ): Promise<vscode.CompletionItem[] | vscode.CompletionList | undefined>;
    
    // Hover
    provideHover(
        document: vscode.TextDocument,
        position: vscode.Position,
        token: vscode.CancellationToken
    ): Promise<vscode.Hover | undefined>;
    
    // Definition
    provideDefinition(
        document: vscode.TextDocument,
        position: vscode.Position,
        token: vscode.CancellationToken
    ): Promise<vscode.Definition | vscode.LocationLink[] | undefined>;
    
    // References
    provideReferences(
        document: vscode.TextDocument,
        position: vscode.Position,
        context: vscode.ReferenceContext,
        token: vscode.CancellationToken
    ): Promise<vscode.Location[] | undefined>;
    
    // Rename
    provideRenameEdits(
        document: vscode.TextDocument,
        position: vscode.Position,
        newName: string,
        token: vscode.CancellationToken
    ): Promise<vscode.WorkspaceEdit | undefined>;
}

// Formatter
export class CHTLFormatter implements vscode.DocumentFormattingEditProvider {
    provideDocumentFormattingEdits(
        document: vscode.TextDocument,
        options: vscode.FormattingOptions,
        token: vscode.CancellationToken
    ): Promise<vscode.TextEdit[]>;
}

// Linter
export class CHTLLinter implements vscode.DiagnosticProvider {
    provideDiagnostics(
        document: vscode.TextDocument,
        token: vscode.CancellationToken
    ): Promise<vscode.Diagnostic[]>;
}

// Preview
export class CHTLPreview {
    show(): Promise<void>;
}

// Compiler
export class CHTLCompiler {
    compile(): Promise<void>;
    compileDocument(document: vscode.TextDocument): Promise<string | null>;
    watch(): Promise<void>;
    build(): Promise<void>;
    clean(): Promise<void>;
}
```

## Configuration API

### ConfigurationManager

Manages CHTL configuration options.

```cpp
namespace CHTL {

enum class ConfigOptionType {
    String,
    Number,
    Boolean,
    List
};

struct ConfigOption {
    ConfigOptionType type;
    std::string value;
    std::vector<std::string> listValues;
};

class ConfigurationManager {
public:
    // Constructor
    ConfigurationManager();
    
    // Configuration management
    void addConfiguration(const std::string& name, 
                         const std::map<std::string, ConfigOption>& options);
    void removeConfiguration(const std::string& name);
    bool hasConfiguration(const std::string& name) const;
    
    // Option management
    void setOption(const std::string& configName, const std::string& optionName, 
                   const ConfigOption& option);
    ConfigOption getOption(const std::string& configName, 
                          const std::string& optionName) const;
    bool hasOption(const std::string& configName, const std::string& optionName) const;
    
    // Keyword customization
    void addCustomKeyword(const std::string& keyword);
    void removeCustomKeyword(const std::string& keyword);
    bool isCustomKeyword(const std::string& keyword) const;
    
    // Origin type customization
    void addCustomOriginType(const std::string& originType);
    void removeCustomOriginType(const std::string& originType);
    bool isCustomOriginType(const std::string& originType) const;
    
    // Import configuration
    void importConfiguration(const std::string& configName, 
                            const std::string& importPath);
    
    // Get all configurations
    std::map<std::string, std::map<std::string, ConfigOption>> getAllConfigurations() const;
    std::vector<std::string> getCustomKeywords() const;
    std::vector<std::string> getCustomOriginTypes() const;
};

}
```

## Utility Classes

### UnifiedScanner

Unified scanner for CHTL source code.

```cpp
namespace CHTL {

enum class CodeType {
    CHTL,
    CHTL_JS,
    CSS,
    JS,
    HTML,
    UNKNOWN
};

struct CodeFragment {
    CodeType type;
    std::string content;
    size_t startLine;
    size_t startColumn;
    size_t endLine;
    size_t endColumn;
    std::string originalContent;
};

class UnifiedScanner {
public:
    // Constructor
    UnifiedScanner();
    
    // Main scanning methods
    std::vector<CodeFragment> scan(const std::string& source);
    std::vector<std::string> scanAsStrings(const std::string& source);
    
    // State query
    bool isInCHTLBlock() const;
    bool isInCHTLJSBlock() const;
    bool isInCSSBlock() const;
    bool isInJSBlock() const;
    bool isInHTMLBlock() const;
    
    // Placeholder management
    std::string createPlaceholder(const std::string& content);
    std::string resolvePlaceholder(const std::string& placeholder);
    std::vector<std::string> getAllPlaceholders() const;
    
    // Debug methods
    void setDebugMode(bool enabled);
    bool isDebugMode() const;
    void printStats() const;
    
private:
    std::string source;
    size_t position;
    size_t line;
    size_t column;
    
    // State management
    struct ScannerState {
        bool inCHTLBlock;
        bool inCHTLJSBlock;
        bool inCSSBlock;
        bool inJSBlock;
        bool inHTMLBlock;
        bool inComment;
        bool inString;
        bool inTemplate;
        bool inCustom;
        char stringDelimiter;
        int braceLevel;
        int bracketLevel;
        int parenLevel;
        int templateLevel;
    } state;
    
    // Placeholder management
    std::map<std::string, std::string> placeholders;
    size_t placeholderCounter;
    bool debugMode;
    
    // Statistics
    struct ScanStats {
        size_t totalFragments;
        size_t chtlFragments;
        size_t chtljsFragments;
        size_t cssFragments;
        size_t jsFragments;
        size_t htmlFragments;
        size_t scanTime;
    } stats;
    
    // Internal methods
    void resetStats();
    char currentChar() const;
    char peekChar(size_t offset = 1) const;
    void advance();
    void skipWhitespace();
    void skipNewline();
    void throwError(const std::string& message) const;
    
    // Block detection
    bool detectCHTLBlock();
    bool detectCHTLJSBlock();
    bool detectCSSBlock();
    bool detectJSBlock();
    bool detectHTMLBlock();
    
    // Block extraction
    std::string extractCHTLBlock();
    std::string extractCHTLJSBlock();
    std::string extractCSSBlock();
    std::string extractJSBlock();
    std::string extractHTMLBlock();
    
    // Placeholder handling
    std::string createPlaceholder(const std::string& content);
    std::string resolvePlaceholder(const std::string& placeholder);
};

}
```

### CJMOD API Classes

```cpp
namespace CHTL {

// Syntax class for CHTL JS
class Syntax {
public:
    Syntax();
    void addRule(const SyntaxRule& rule);
    void removeRule(const std::string& pattern);
    bool hasRule(const std::string& pattern) const;
    SyntaxRule getRule(const std::string& pattern) const;
    std::vector<SyntaxRule> getAllRules() const;
    bool validateSyntax(const std::string& input) const;
    std::vector<std::string> getSyntaxErrors(const std::string& input) const;
    std::vector<SyntaxRule> analyze(const std::string& input) const;
    std::string getSyntaxType(const std::string& token) const;
    std::string toJSON() const;
    std::string toString() const;
};

// Argument class for CHTL JS
class Arg {
public:
    Arg();
    explicit Arg(const std::string& value);
    explicit Arg(int value);
    explicit Arg(double value);
    explicit Arg(bool value);
    explicit Arg(const std::vector<Arg>& value);
    explicit Arg(const std::map<std::string, Arg>& value);
    
    ArgType getType() const;
    std::string getTypeName() const;
    std::string getString() const;
    int getInt() const;
    double getDouble() const;
    bool getBool() const;
    std::vector<Arg> getArray() const;
    std::map<std::string, Arg> getObject() const;
    
    void setString(const std::string& value);
    void setInt(int value);
    void setDouble(double value);
    void setBool(bool value);
    void setArray(const std::vector<Arg>& value);
    void setObject(const std::map<std::string, Arg>& value);
    
    std::string toString() const;
    int toInt() const;
    double toDouble() const;
    bool toBool() const;
    
    bool operator==(const Arg& other) const;
    bool operator!=(const Arg& other) const;
    bool operator<(const Arg& other) const;
    bool operator>(const Arg& other) const;
    bool operator<=(const Arg& other) const;
    bool operator>=(const Arg& other) const;
    
    std::string toJSON() const;
};

// Atom argument class
class AtomArg : public Arg {
public:
    AtomArg();
    explicit AtomArg(const std::string& value, AtomType type = AtomType::Literal);
    explicit AtomArg(const Arg& arg, AtomType type = AtomType::Literal);
    
    AtomType getAtomType() const;
    void setAtomType(AtomType type);
    
    bool isPlaceholder() const;
    void setPlaceholder(const std::string& placeholder);
    std::string getPlaceholder() const;
    
    bool isVariable() const;
    void setVariable(const std::string& variable);
    std::string getVariable() const;
    
    bool isExpression() const;
    void setExpression(const std::string& expression);
    std::string getExpression() const;
    
    bool isFunction() const;
    void setFunction(const std::string& functionName, 
                    const std::vector<AtomArg>& args = {});
    std::string getFunctionName() const;
    std::vector<AtomArg> getFunctionArgs() const;
    
    bool isProperty() const;
    void setProperty(const std::string& object, const std::string& property);
    std::string getObject() const;
    std::string getProperty() const;
    
    bool isIndex() const;
    void setIndex(const std::string& object, const AtomArg& index);
    std::string getIndexObject() const;
    AtomArg getIndexValue() const;
    
    static AtomArg parse(const std::string& input);
    std::string generate() const;
    bool isValid() const;
    std::vector<std::string> getValidationErrors() const;
    
    std::string toJSON() const override;
    std::string toString() const override;
};

// CHTL JS Function class
class CHTLJSFunction {
public:
    CHTLJSFunction();
    explicit CHTLJSFunction(const std::string& name);
    
    void setName(const std::string& name);
    std::string getName() const;
    
    void setDescription(const std::string& description);
    std::string getDescription() const;
    
    void setType(CHTLJSFunctionType type);
    CHTLJSFunctionType getType() const;
    
    void setReturnType(ArgType returnType);
    ArgType getReturnType() const;
    
    void addParameter(const FunctionParameter& param);
    void removeParameter(const std::string& paramName);
    bool hasParameter(const std::string& paramName) const;
    FunctionParameter getParameter(const std::string& paramName) const;
    std::vector<FunctionParameter> getParameters() const;
    
    void setBody(const std::string& body);
    std::string getBody() const;
    
    void setImplementation(std::function<Arg(const std::vector<Arg>&)> impl);
    std::function<Arg(const std::vector<Arg>&)> getImplementation() const;
    
    Arg call(const std::vector<Arg>& args) const;
    Arg call(const std::map<std::string, Arg>& namedArgs) const;
    
    bool isValid() const;
    std::vector<std::string> getValidationErrors() const;
    
    std::string toJSON() const;
    std::string toString() const;
};

}
```

## Error Handling

All CHTL API classes provide comprehensive error handling:

```cpp
// Error types
enum class CHTLErrorType {
    LexicalError,
    SyntaxError,
    SemanticError,
    RuntimeError,
    CompilationError,
    ModuleError,
    ConfigurationError
};

// Error structure
struct CHTLError {
    CHTLErrorType type;
    std::string message;
    size_t line;
    size_t column;
    std::string file;
    std::string suggestion;
};

// Error handling methods
bool hasErrors() const;
std::vector<CHTLError> getErrors() const;
void clearErrors();
void addError(const CHTLError& error);
void throwError(const std::string& message, CHTLErrorType type = CHTLErrorType::RuntimeError);
```

## Thread Safety

Most CHTL API classes are not thread-safe by default. For multi-threaded applications:

1. Use separate instances for each thread
2. Use mutexes for shared access
3. Consider using thread-local storage for temporary data

## Memory Management

CHTL uses smart pointers for automatic memory management:

- `std::shared_ptr` for shared ownership
- `std::unique_ptr` for exclusive ownership
- `std::weak_ptr` for non-owning references

## Performance Considerations

1. **Lexing**: O(n) where n is the source length
2. **Parsing**: O(n) for most constructs, O(n²) for complex nested structures
3. **Generation**: O(n) where n is the AST size
4. **Module Resolution**: O(m) where m is the number of modules

## Examples

### Basic Usage

```cpp
#include "CHTL/CHTLLexer/CHTLLexer.h"
#include "CHTL/CHTLParser/CHTLParser.h"
#include "CHTL/CHTLGenerator/CHTLGenerator.h"

// Create lexer
CHTL::CHTLLexer lexer(sourceCode);
auto tokens = lexer.tokenize();

// Create parser
CHTL::CHTLParser parser(tokens);
auto ast = parser.parse();

// Create generator
CHTL::CHTLGenerator generator;
std::string output = generator.generate(ast);
```

### Module Usage

```cpp
#include "CHTL/ModuleSystem/CMODSystem.h"

// Create module
auto module = std::make_shared<CHTL::CMODModule>();
CHTL::CMODInfo info;
info.name = "MyModule";
info.version = "1.0.0";
module->setInfo(info);

// Add export
CHTL::ExportItem export;
export.type = CHTL::ExportType::Template;
export.name = "MyTemplate";
export.isPublic = true;
module->addExport(export);

// Register module
CHTL::CMODSystem system;
system.registerModule(module);
```

### CLI Usage

```cpp
#include "CLI/CHTLCli.h"

// Create CLI
CHTL::CHTLCli cli;
cli.setProgramName("my-chtl-tool");
cli.setVersion("1.0.0");

// Execute command
std::vector<std::string> args = {"compile", "input.chtl", "-o", "output.html"};
int result = cli.execute(args);
```