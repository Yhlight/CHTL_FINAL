# CHTL API 参考

本文档提供了 CHTL 编译器的完整 API 参考，包括所有类、方法和函数的详细说明。

## 核心 API

### CHTLContext

CHTL 编译器的核心上下文类，管理全局状态和配置。

```cpp
class CHTLContext {
public:
    // 构造函数
    CHTLContext();
    ~CHTLContext();
    
    // 错误管理
    void addError(const std::string& error);
    void addWarning(const std::string& warning);
    std::vector<std::string> getErrors() const;
    std::vector<std::string> getWarnings() const;
    bool hasErrors() const;
    void clearErrors();
    
    // 配置管理
    void setConfiguration(const std::string& key, const std::string& value);
    std::string getConfiguration(const std::string& key) const;
    void loadConfiguration(const std::string& filePath);
    void saveConfiguration(const std::string& filePath);
    
    // 模块管理
    void addModule(const std::string& name, std::shared_ptr<CHTLModule> module);
    std::shared_ptr<CHTLModule> getModule(const std::string& name) const;
    bool hasModule(const std::string& name) const;
    
    // 模板管理
    void addTemplate(const std::string& name, std::shared_ptr<CHTLTemplate> template_);
    std::shared_ptr<CHTLTemplate> getTemplate(const std::string& name) const;
    bool hasTemplate(const std::string& name) const;
};
```

### CHTLLexer

词法分析器，将 CHTL 代码转换为令牌序列。

```cpp
class CHTLLexer {
public:
    explicit CHTLLexer(std::shared_ptr<CHTLContext> context);
    ~CHTLLexer();
    
    // 词法分析
    std::vector<CHTLToken> tokenize(const std::string& code);
    std::vector<CHTLToken> tokenizeFile(const std::string& filePath);
    
    // 令牌类型检测
    CHTLTokenType getTokenType(const std::string& token) const;
    bool isKeyword(const std::string& token) const;
    bool isOperator(const std::string& token) const;
    bool isDelimiter(const std::string& token) const;
    
    // 错误处理
    std::vector<CHTLLexerError> getErrors() const;
    bool hasErrors() const;
    void clearErrors();
};
```

### CHTLParser

语法分析器，将令牌序列转换为抽象语法树。

```cpp
class CHTLParser {
public:
    explicit CHTLParser(std::shared_ptr<CHTLContext> context);
    ~CHTLParser();
    
    // 语法分析
    std::shared_ptr<CHTLNode> parse(const std::vector<CHTLToken>& tokens);
    std::shared_ptr<CHTLNode> parse(const std::string& code);
    std::shared_ptr<CHTLNode> parseFile(const std::string& filePath);
    
    // 节点类型检测
    bool isElementNode(const CHTLNode& node) const;
    bool isTextNode(const CHTLNode& node) const;
    bool isStyleNode(const CHTLNode& node) const;
    bool isScriptNode(const CHTLNode& node) const;
    bool isTemplateNode(const CHTLNode& node) const;
    bool isCustomNode(const CHTLNode& node) const;
    bool isOriginNode(const CHTLNode& node) const;
    bool isImportNode(const CHTLNode& node) const;
    bool isConstraintNode(const CHTLNode& node) const;
    bool isConfigNode(const CHTLNode& node) const;
    
    // 错误处理
    std::vector<CHTLParserError> getErrors() const;
    bool hasErrors() const;
    void clearErrors();
};
```

### CHTLGenerator

代码生成器，将抽象语法树转换为目标代码。

```cpp
class CHTLGenerator {
public:
    explicit CHTLGenerator(std::shared_ptr<CHTLContext> context);
    ~CHTLGenerator();
    
    // 代码生成
    std::string generateHTML(const CHTLNode& node);
    std::string generateCSS(const CHTLNode& node);
    std::string generateJavaScript(const CHTLNode& node);
    std::string generateAll(const CHTLNode& node);
    
    // 文件生成
    void generateToFile(const CHTLNode& node, const std::string& filePath);
    void generateToFiles(const CHTLNode& node, const std::string& outputDir);
    
    // 生成选项
    void setGenerateHTML(bool generate) { m_generateHTML = generate; }
    void setGenerateCSS(bool generate) { m_generateCSS = generate; }
    void setGenerateJavaScript(bool generate) { m_generateJavaScript = generate; }
    void setMinifyOutput(bool minify) { m_minifyOutput = minify; }
    void setIncludeSourceMaps(bool include) { m_includeSourceMaps = include; }
    
    // 错误处理
    std::vector<CHTLGeneratorError> getErrors() const;
    bool hasErrors() const;
    void clearErrors();
};
```

## 模板系统 API

### CHTLTemplate

模板系统的基础类，支持样式组、元素和变量组模板。

```cpp
class CHTLTemplate {
public:
    enum class TemplateType {
        Style,
        Element,
        Variable
    };
    
    // 构造函数
    CHTLTemplate(const std::string& name, TemplateType type);
    ~CHTLTemplate();
    
    // 基本信息
    std::string getName() const { return m_name; }
    TemplateType getType() const { return m_type; }
    void setName(const std::string& name) { m_name = name; }
    void setType(TemplateType type) { m_type = type; }
    
    // 内容管理
    void setContent(const std::string& content) { m_content = content; }
    std::string getContent() const { return m_content; }
    
    // 继承管理
    void addInheritance(const std::string& parentName);
    void removeInheritance(const std::string& parentName);
    std::vector<std::string> getInheritances() const { return m_inheritances; }
    
    // 变量管理
    void addVariable(const std::string& name, const std::string& value);
    void removeVariable(const std::string& name);
    std::string getVariable(const std::string& name) const;
    std::unordered_map<std::string, std::string> getVariables() const { return m_variables; }
    
    // 验证
    bool validate() const;
    std::vector<std::string> getValidationErrors() const;
};
```

### CHTLCustom

自定义系统类，支持模板的特例化操作。

```cpp
class CHTLCustom {
public:
    enum class CustomType {
        Style,
        Element,
        Variable
    };
    
    // 构造函数
    CHTLCustom(const std::string& name, CustomType type);
    ~CHTLCustom();
    
    // 基本信息
    std::string getName() const { return m_name; }
    CustomType getType() const { return m_type; }
    
    // 特例化操作
    void addSpecialization(const std::string& property, const std::string& value);
    void removeSpecialization(const std::string& property);
    void modifySpecialization(const std::string& property, const std::string& value);
    
    // 删除操作
    void addDeletion(const std::string& property);
    void removeDeletion(const std::string& property);
    
    // 插入操作
    void addInsertion(const std::string& property, const std::string& value);
    void removeInsertion(const std::string& property);
    
    // 获取操作
    std::unordered_map<std::string, std::string> getSpecializations() const { return m_specializations; }
    std::vector<std::string> getDeletions() const { return m_deletions; }
    std::unordered_map<std::string, std::string> getInsertions() const { return m_insertions; }
    
    // 验证
    bool validate() const;
    std::vector<std::string> getValidationErrors() const;
};
```

## 原始嵌入 API

### CHTLOrigin

原始代码嵌入系统，支持 HTML、CSS、JavaScript 代码的直接嵌入。

```cpp
class CHTLOrigin {
public:
    enum class OriginType {
        HTML,
        CSS,
        JavaScript,
        Custom
    };
    
    // 构造函数
    CHTLOrigin(const std::string& name, OriginType type);
    ~CHTLOrigin();
    
    // 基本信息
    std::string getName() const { return m_name; }
    OriginType getType() const { return m_type; }
    
    // 内容管理
    void setContent(const std::string& content) { m_content = content; }
    std::string getContent() const { return m_content; }
    
    // 类型管理
    void setCustomType(const std::string& type) { m_customType = type; }
    std::string getCustomType() const { return m_customType; }
    
    // 处理
    std::string process() const;
    bool validate() const;
    
    // 错误处理
    std::vector<std::string> getErrors() const { return m_errors; }
    bool hasErrors() const { return !m_errors.empty(); }
};
```

## 导入系统 API

### CHTLImport

导入系统，支持各种文件的导入和命名空间管理。

```cpp
class CHTLImport {
public:
    enum class ImportType {
        CHTL,
        HTML,
        CSS,
        JavaScript,
        JSON,
        Module
    };
    
    // 构造函数
    CHTLImport(const std::string& path, ImportType type);
    ~CHTLImport();
    
    // 基本信息
    std::string getPath() const { return m_path; }
    ImportType getType() const { return m_type; }
    
    // 别名管理
    void setAlias(const std::string& alias) { m_alias = alias; }
    std::string getAlias() const { return m_alias; }
    
    // 命名空间管理
    void setNamespace(const std::string& namespace_) { m_namespace = namespace_; }
    std::string getNamespace() const { return m_namespace; }
    
    // 路径解析
    std::string resolvePath(const std::string& basePath) const;
    bool isResolved() const { return m_resolved; }
    
    // 内容加载
    std::string loadContent() const;
    bool isLoaded() const { return m_loaded; }
    
    // 错误处理
    std::vector<std::string> getErrors() const { return m_errors; }
    bool hasErrors() const { return !m_errors.empty(); }
};
```

## 约束系统 API

### CHTLConstraints

约束系统，支持精确约束、类型约束和全局约束。

```cpp
class CHTLConstraints {
public:
    enum class ConstraintType {
        Exact,
        Type,
        Global
    };
    
    // 构造函数
    CHTLConstraints();
    ~CHTLConstraints();
    
    // 约束添加
    void addExactConstraint(const std::string& element, const std::string& context);
    void addTypeConstraint(const std::string& element, const std::string& type);
    void addGlobalConstraint(const std::string& element);
    
    // 约束移除
    void removeExactConstraint(const std::string& element, const std::string& context);
    void removeTypeConstraint(const std::string& element, const std::string& type);
    void removeGlobalConstraint(const std::string& element);
    
    // 约束检查
    bool checkConstraint(const std::string& element, const std::string& context) const;
    bool checkTypeConstraint(const std::string& element, const std::string& type) const;
    bool checkGlobalConstraint(const std::string& element) const;
    
    // 约束获取
    std::vector<std::pair<std::string, std::string>> getExactConstraints() const;
    std::vector<std::pair<std::string, std::string>> getTypeConstraints() const;
    std::vector<std::string> getGlobalConstraints() const;
    
    // 验证
    bool validate() const;
    std::vector<std::string> getValidationErrors() const;
};
```

## 配置组 API

### CHTLConfig

配置组系统，支持自定义关键字名称和模块配置。

```cpp
class CHTLConfig {
public:
    // 构造函数
    CHTLConfig(const std::string& name);
    ~CHTLConfig();
    
    // 基本信息
    std::string getName() const { return m_name; }
    void setName(const std::string& name) { m_name = name; }
    
    // 配置管理
    void setConfiguration(const std::string& key, const std::string& value);
    std::string getConfiguration(const std::string& key) const;
    void removeConfiguration(const std::string& key);
    std::unordered_map<std::string, std::string> getConfigurations() const { return m_configurations; }
    
    // 关键字重命名
    void addNameMapping(const std::string& original, const std::string& mapped);
    void removeNameMapping(const std::string& original);
    std::string getMappedName(const std::string& original) const;
    std::unordered_map<std::string, std::string> getNameMappings() const { return m_nameMappings; }
    
    // 原始类型
    void addOriginType(const std::string& type, const std::string& handler);
    void removeOriginType(const std::string& type);
    std::string getOriginTypeHandler(const std::string& type) const;
    std::unordered_map<std::string, std::string> getOriginTypes() const { return m_originTypes; }
    
    // 验证
    bool validate() const;
    std::vector<std::string> getValidationErrors() const;
};
```

## CHTL JS 扩展 API

### CHTLJS

CHTL JS 扩展语法系统。

```cpp
class CHTLJS {
public:
    // 构造函数
    CHTLJS(std::shared_ptr<CHTLContext> context);
    ~CHTLJS();
    
    // 文件加载器
    void addFileLoader(const std::string& filePath);
    std::vector<std::string> getFileLoaders() const { return m_fileLoaders; }
    
    // 局部脚本
    void addLocalScript(const std::string& script);
    std::vector<std::string> getLocalScripts() const { return m_localScripts; }
    
    // 增强选择器
    std::string processSelectors(const std::string& code) const;
    
    // 事件监听器
    void addEventListener(const std::string& event, const std::string& handler);
    std::unordered_map<std::string, std::string> getEventListeners() const { return m_eventListeners; }
    
    // 动画
    void addAnimation(const std::string& target, const std::string& config);
    std::vector<std::string> getAnimations() const { return m_animations; }
    
    // 虚对象
    void addVirtualObject(const std::string& name, const std::string& definition);
    std::unordered_map<std::string, std::string> getVirtualObjects() const { return m_virtualObjects; }
    
    // 路由
    void addRoute(const std::string& url, const std::string& page);
    std::unordered_map<std::string, std::string> getRoutes() const { return m_routes; }
    
    // 编译
    std::string compile() const;
    bool validate() const;
};
```

## 模块系统 API

### CHTLModule

模块系统，支持 CMOD 和 CJMOD 模块。

```cpp
class CHTLModule {
public:
    enum class ModuleType {
        CMOD,
        CJMOD,
        Hybrid
    };
    
    // 构造函数
    CHTLModule(const std::string& name, ModuleType type);
    ~CHTLModule();
    
    // 基本信息
    std::string getName() const { return m_name; }
    ModuleType getType() const { return m_type; }
    
    // 模块信息
    void setModuleInfo(const std::string& key, const std::string& value);
    std::string getModuleInfo(const std::string& key) const;
    std::unordered_map<std::string, std::string> getModuleInfo() const { return m_moduleInfo; }
    
    // 导出管理
    void addExport(const std::string& name, const std::string& content);
    void removeExport(const std::string& name);
    std::string getExport(const std::string& name) const;
    std::unordered_map<std::string, std::string> getExports() const { return m_exports; }
    
    // 依赖管理
    void addDependency(const std::string& dependency);
    void removeDependency(const std::string& dependency);
    std::vector<std::string> getDependencies() const { return m_dependencies; }
    
    // 验证
    bool validate() const;
    std::vector<std::string> getValidationErrors() const;
};
```

## 统一扫描器 API

### CHTLScanner

统一扫描器，支持代码切割和占位符机制。

```cpp
class CHTLScanner {
public:
    // 构造函数
    CHTLScanner(std::shared_ptr<CHTLContext> context);
    ~CHTLScanner();
    
    // 扫描模式
    enum class ScanMode {
        Broad,
        Strict
    };
    
    // 扫描功能
    std::vector<std::string> scan(const std::string& code, ScanMode mode = ScanMode::Broad);
    std::vector<std::string> scanFile(const std::string& filePath, ScanMode mode = ScanMode::Broad);
    
    // 占位符处理
    void addPlaceholder(const std::string& placeholder, const std::string& replacement);
    void removePlaceholder(const std::string& placeholder);
    std::string processPlaceholders(const std::string& code) const;
    
    // 代码切割
    std::vector<std::string> cutCode(const std::string& code, const std::string& delimiter) const;
    std::vector<std::string> smartCut(const std::string& code) const;
    
    // 边界处理
    std::vector<std::string> findBoundaries(const std::string& code) const;
    std::string processBoundaries(const std::string& code) const;
    
    // 智能增强
    std::string enhanceCode(const std::string& code) const;
    std::string optimizeCode(const std::string& code) const;
};
```

## 命令行工具 API

### CHTLCli

命令行工具，支持各种命令和选项。

```cpp
class CHTLCli {
public:
    // 构造函数
    CHTLCli(std::shared_ptr<CHTLContext> context);
    ~CHTLCli();
    
    // 命令注册
    void registerCommand(const std::string& name, std::function<int(const std::vector<std::string>&)> handler);
    void unregisterCommand(const std::string& name);
    
    // 命令执行
    int executeCommand(const std::string& name, const std::vector<std::string>& args);
    int executeCommandLine(const std::string& commandLine);
    
    // 选项解析
    void addOption(const std::string& name, const std::string& description, bool required = false);
    void parseOptions(int argc, char* argv[]);
    
    // 帮助系统
    void printHelp() const;
    void printCommandHelp(const std::string& command) const;
    void printVersion() const;
    
    // 错误处理
    std::vector<std::string> getErrors() const { return m_errors; }
    bool hasErrors() const { return !m_errors.empty(); }
};
```

## VSCode 扩展 API

### VSCodeExtensionManager

VSCode 扩展管理器，提供完整的 IDE 支持。

```cpp
class VSCodeExtensionManager {
public:
    // 构造函数
    VSCodeExtensionManager(std::shared_ptr<CHTLContext> context);
    ~VSCodeExtensionManager();
    
    // 扩展管理
    void initialize();
    void shutdown();
    void reload();
    
    // 组件获取
    std::shared_ptr<SyntaxHighlighter> getSyntaxHighlighter() const;
    std::shared_ptr<CodeFormatter> getCodeFormatter() const;
    std::shared_ptr<CodeCompleter> getCodeCompleter() const;
    std::shared_ptr<PagePreviewer> getPagePreviewer() const;
    std::shared_ptr<Exporter> getExporter() const;
    std::shared_ptr<BuiltinCompiler> getBuiltinCompiler() const;
    std::shared_ptr<ModuleUnpacker> getModuleUnpacker() const;
    std::shared_ptr<JSONQueryTable> getJSONQueryTable() const;
    std::shared_ptr<ModuleSyntaxHelper> getModuleSyntaxHelper() const;
    
    // 配置管理
    void loadConfiguration(const std::string& configFile);
    void saveConfiguration(const std::string& configFile);
    void setConfiguration(const std::string& key, const std::string& value);
    std::string getConfiguration(const std::string& key) const;
};
```

## 测试系统 API

### TestRunner

测试运行器，支持单元测试、集成测试和功能测试。

```cpp
class TestRunner {
public:
    // 构造函数
    TestRunner(std::shared_ptr<CHTLContext> context);
    ~TestRunner();
    
    // 测试运行
    void runAllTests();
    void runUnitTests();
    void runIntegrationTests();
    void runFunctionalTests();
    void runTestSuite(const std::string& suiteName);
    void runTest(const std::string& suiteName, const std::string& testName);
    
    // 测试报告
    void generateReport();
    void generateHTMLReport();
    void generateJSONReport();
    void generateXMLReport();
    
    // 测试配置
    void setVerbose(bool verbose) { m_verbose = verbose; }
    void setStopOnFailure(bool stop) { m_stopOnFailure = stop; }
    void setOutputDirectory(const std::string& dir) { m_outputDirectory = dir; }
};
```

## 错误处理

所有 API 都提供统一的错误处理机制：

```cpp
// 错误类型
enum class CHTLErrorType {
    LexerError,
    ParserError,
    GeneratorError,
    TemplateError,
    CustomError,
    OriginError,
    ImportError,
    ConstraintError,
    ConfigError,
    CHTLJSError,
    ModuleError,
    ScannerError,
    CliError,
    VSCodeError,
    TestError
};

// 错误类
class CHTLError {
public:
    CHTLError(CHTLErrorType type, const std::string& message, int line = -1, int column = -1);
    
    CHTLErrorType getType() const { return m_type; }
    std::string getMessage() const { return m_message; }
    int getLine() const { return m_line; }
    int getColumn() const { return m_column; }
    
    std::string toString() const;
};
```

## 使用示例

### 基本使用

```cpp
#include "CHTL/CHTLContext.h"
#include "CHTL/CHTLLexer.h"
#include "CHTL/CHTLParser.h"
#include "CHTL/CHTLGenerator.h"

int main() {
    // 创建上下文
    auto context = std::make_shared<CHTLContext>();
    
    // 创建词法分析器
    CHTLLexer lexer(context);
    
    // 创建语法分析器
    CHTLParser parser(context);
    
    // 创建代码生成器
    CHTLGenerator generator(context);
    
    // 编译 CHTL 代码
    std::string chtlCode = "html { head { title { \"Hello\" } } }";
    auto tokens = lexer.tokenize(chtlCode);
    auto ast = parser.parse(tokens);
    std::string html = generator.generateHTML(*ast);
    
    std::cout << html << std::endl;
    
    return 0;
}
```

### 使用模板系统

```cpp
#include "CHTL/CHTLTemplate.h"

// 创建样式组模板
auto styleTemplate = std::make_shared<CHTLTemplate>("ButtonStyle", CHTLTemplate::TemplateType::Style);
styleTemplate->setContent("background-color: #007bff; color: white; padding: 10px;");

// 创建元素模板
auto elementTemplate = std::make_shared<CHTLTemplate>("Button", CHTLTemplate::TemplateType::Element);
elementTemplate->setContent("button { style { @Style ButtonStyle; } text { \"Click me\" } }");

// 添加到上下文
context->addTemplate("ButtonStyle", styleTemplate);
context->addTemplate("Button", elementTemplate);
```

### 使用模块系统

```cpp
#include "CHTL/CHTLModule.h"

// 创建 CMOD 模块
auto module = std::make_shared<CHTLModule>("MyModule", CHTLModule::ModuleType::CMOD);
module->setModuleInfo("name", "MyModule");
module->setModuleInfo("version", "1.0.0");
module->addExport("MyStyle", ".my-style { color: red; }");

// 添加到上下文
context->addModule("MyModule", module);
```

## 注意事项

1. **内存管理**: 所有 API 都使用智能指针进行内存管理，无需手动释放内存
2. **错误处理**: 始终检查错误状态，使用 `hasErrors()` 方法检查是否有错误
3. **线程安全**: 大多数 API 不是线程安全的，在多线程环境中使用时需要适当的同步
4. **性能**: 对于大型项目，建议使用模块系统来组织代码
5. **配置**: 使用配置组来管理项目设置，避免硬编码

## 版本兼容性

- **C++ 17**: 需要支持 C++ 17 的编译器
- **CMake 3.10+**: 构建系统要求
- **JSON**: 使用 jsoncpp 库进行 JSON 处理
- **正则表达式**: 使用 C++ 标准库的正则表达式支持

---

*最后更新: 2024年*