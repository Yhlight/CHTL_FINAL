# CHTL API 文档

本文档详细介绍了CHTL项目的API接口和使用方法。

## 目录

- [核心编译器API](#核心编译器api)
- [模块系统API](#模块系统api)
- [CLI工具API](#cli工具api)
- [VSCode扩展API](#vscode扩展api)
- [示例代码](#示例代码)

## 核心编译器API

### UnifiedScanner

统一扫描器，负责代码分离和类型识别。

```cpp
#include "Scanner/UnifiedScanner.h"

// 创建扫描器
UnifiedScanner scanner;

// 扫描代码
std::string source_code = "div { text { Hello } }";
auto fragments = scanner.scan(source_code);

// 处理扫描结果
for (const auto& fragment : fragments) {
    std::cout << "Type: " << static_cast<int>(fragment.type) << std::endl;
    std::cout << "Content: " << fragment.content << std::endl;
}
```

#### 主要方法

- `scan(const std::string& source_code)` - 扫描源代码并返回代码片段
- `setDebugMode(bool debug)` - 设置调试模式
- `getErrors()` - 获取错误信息

### CHTLLexer

CHTL词法分析器，将CHTL代码转换为词法单元。

```cpp
#include "CHTL/CHTLLexer.h"

// 创建词法分析器
CHTL::CHTLLexer lexer;

// 词法分析
std::string source = "div { text { Hello } }";
auto tokens = lexer.tokenize(source);

// 处理词法单元
for (const auto& token : tokens) {
    std::cout << "Type: " << static_cast<int>(token.type) << std::endl;
    std::cout << "Value: " << token.value << std::endl;
}
```

#### 主要方法

- `tokenize(const std::string& source)` - 词法分析
- `setDebugMode(bool debug)` - 设置调试模式
- `getErrors()` - 获取错误信息

### CHTLParser

CHTL语法分析器，构建抽象语法树。

```cpp
#include "CHTL/CHTLParser.h"

// 创建语法分析器
CHTL::CHTLParser parser;

// 语法分析
auto ast = parser.parse(tokens);

// 处理AST
if (ast) {
    std::cout << "AST created successfully" << std::endl;
    // 遍历AST节点
}
```

#### 主要方法

- `parse(const std::vector<Token>& tokens)` - 语法分析
- `getErrors()` - 获取错误信息
- `setDebugMode(bool debug)` - 设置调试模式

### CHTLGenerator

CHTL代码生成器，生成HTML、CSS、JavaScript代码。

```cpp
#include "CHTL/CHTLGenerator.h"

// 创建代码生成器
CHTL::CHTLGenerator generator;

// 生成代码
auto result = generator.generate(ast);

// 处理生成结果
if (result.success) {
    std::cout << "HTML: " << result.html << std::endl;
    std::cout << "CSS: " << result.css << std::endl;
    std::cout << "JS: " << result.js << std::endl;
}
```

#### 主要方法

- `generate(std::shared_ptr<CHTLNode> ast)` - 生成代码
- `setDebugMode(bool debug)` - 设置调试模式
- `getErrors()` - 获取错误信息

## 模块系统API

### CMODSystem

CMOD模块系统，管理CHTL模块。

```cpp
#include "Module/CMODSystem.h"

// 创建CMOD系统
CHTL::CMODSystem cmod_system;

// 初始化系统
cmod_system.initialize("./modules", "./user_modules");

// 加载模块
auto module = cmod_system.loadModule("my-module");

if (module) {
    std::cout << "Module loaded: " << module->name << std::endl;
    std::cout << "Version: " << module->info.version << std::endl;
    
    // 获取导出表
    auto exports = module->info.exports;
    for (const auto& export_item : exports) {
        std::cout << export_item.first << ": " << export_item.second << std::endl;
    }
}
```

#### 主要方法

- `initialize(const std::string& official_path, const std::string& user_path)` - 初始化系统
- `loadModule(const std::string& module_name)` - 加载模块
- `moduleExists(const std::string& module_name)` - 检查模块是否存在
- `getModuleExports(const std::string& module_name)` - 获取模块导出表

### CJMODSystem

CJMOD模块系统，管理CHTL JS模块。

```cpp
#include "Module/CJMODSystem.h"

// 创建CJMOD系统
CHTL::CJMODSystem cjmod_system;

// 初始化系统
cjmod_system.initialize("./modules", "./user_modules");

// 注册函数
cjmod_system.registerFunction("my-module", "hello", 
    [](const std::vector<std::string>& args) -> std::string {
        return "Hello from CJMOD!";
    });

// 执行函数
auto result = cjmod_system.executeFunction("my-module", "hello", {});
std::cout << "Result: " << result << std::endl;
```

#### 主要方法

- `initialize(const std::string& official_path, const std::string& user_path)` - 初始化系统
- `loadModule(const std::string& module_name)` - 加载模块
- `registerFunction(const std::string& module_name, const std::string& function_name, std::function<std::string(const std::vector<std::string>&)> function)` - 注册函数
- `executeFunction(const std::string& module_name, const std::string& function_name, const std::vector<std::string>& args)` - 执行函数

### CJMOD API

CJMOD API提供语法分析和代码生成功能。

```cpp
#include "Module/CJMODSystem.h"

// 语法分析
Arg args = Syntax::analyze("$ ** $");
args.print(); // 输出: ["$", "**", "$"]

// 扫描语法
Arg result = CJMODScanner::scan(args, "**");
result.print(); // 输出: ["$", "**", "$"]

// 生成代码
std::string output = CJMODGenerator::exportResult(result);

// 创建CHTL JS函数
std::string function = CHTLJSFunction::createFunction("add", {"a", "b"}, "return a + b;");
```

## CLI工具API

### CHTLCli

CHTL命令行工具，提供完整的命令行界面。

```cpp
#include "CLI/CHTLCli.h"

// 创建CLI工具
CHTL::CHTLCli cli;

// 解析命令行参数
int argc = 3;
char* argv[] = {"chtl", "compile", "input.chtl"};
CommandType command = cli.parseArguments(argc, argv);

// 执行命令
int result = cli.executeCommand(command);
```

#### 主要方法

- `parseArguments(int argc, char* argv[])` - 解析命令行参数
- `executeCommand(CommandType command_type)` - 执行命令
- `compileFile(const std::string& input_file, const std::string& output_file)` - 编译文件
- `watchFiles(const std::string& input_path)` - 监视文件变化
- `initializeProject(const std::string& project_name)` - 初始化项目

#### 支持的命令

- `compile` - 编译CHTL文件
- `watch` - 监视文件变化
- `init` - 初始化项目
- `build` - 构建项目
- `clean` - 清理构建文件
- `serve` - 启动开发服务器
- `module` - 管理模块

## VSCode扩展API

### 扩展激活

```typescript
import * as vscode from 'vscode';

export function activate(context: vscode.ExtensionContext) {
    // 注册命令
    const compileCommand = vscode.commands.registerCommand('chtl.compile', compileCHTL);
    const previewCommand = vscode.commands.registerCommand('chtl.preview', previewCHTL);
    
    context.subscriptions.push(compileCommand, previewCommand);
}
```

### 语言服务器

```typescript
import { CHTLLanguageServer } from './languageServer';

const languageServer = new CHTLLanguageServer();
languageServer.activate(context);

// 提供代码补全
const completionProvider = vscode.languages.registerCompletionItemProvider(
    { scheme: 'file', language: 'chtl' },
    {
        provideCompletionItems(document, position) {
            return languageServer.provideCompletionItems(document, position);
        }
    }
);
```

### 模块视图

```typescript
import { CHTLModuleView } from './moduleViewProvider';

const moduleView = new CHTLModuleView(context);

// 刷新模块视图
moduleView.refresh();
```

## 示例代码

### 基本编译示例

```cpp
#include "Compiler/CompilerDispatcher.h"
#include <iostream>

int main() {
    // 创建编译器调度器
    CHTL::CompilerDispatcher compiler;
    
    // 设置调试模式
    compiler.setDebugMode(true);
    
    // 编译CHTL代码
    std::string source_code = R"(
        html {
            head {
                title { text { "Hello CHTL!" } }
            }
            body {
                div {
                    text { "Hello from CHTL!" }
                    style {
                        color: blue;
                        font-size: 24px;
                    }
                }
            }
        }
    )";
    
    auto result = compiler.compile(source_code);
    
    if (result.success) {
        std::cout << "Compilation successful!" << std::endl;
        std::cout << "HTML Output:" << std::endl;
        std::cout << result.html << std::endl;
    } else {
        std::cout << "Compilation failed!" << std::endl;
        for (const auto& error : result.errors) {
            std::cout << "Error: " << error << std::endl;
        }
    }
    
    return 0;
}
```

### 模块使用示例

```cpp
#include "Module/CMODSystem.h"
#include <iostream>

int main() {
    // 创建CMOD系统
    CHTL::CMODSystem cmod_system;
    
    // 初始化系统
    cmod_system.initialize("./modules", "./user_modules");
    
    // 加载模块
    auto module = cmod_system.loadModule("button-component");
    
    if (module) {
        std::cout << "Module: " << module->name << std::endl;
        std::cout << "Version: " << module->info.version << std::endl;
        std::cout << "Description: " << module->info.description << std::endl;
        
        // 使用模块源码
        std::cout << "Source code:" << std::endl;
        std::cout << module->source_code << std::endl;
    }
    
    return 0;
}
```

### CLI工具示例

```cpp
#include "CLI/CHTLCli.h"
#include <iostream>

int main(int argc, char* argv[]) {
    // 创建CLI工具
    CHTL::CHTLCli cli;
    
    // 解析参数
    CommandType command = cli.parseArguments(argc, argv);
    
    // 执行命令
    int result = cli.executeCommand(command);
    
    // 显示错误信息
    auto errors = cli.getErrors();
    if (!errors.empty()) {
        for (const auto& error : errors) {
            std::cerr << "Error: " << error << std::endl;
        }
    }
    
    return result;
}
```

## 错误处理

所有API都提供了错误处理机制：

```cpp
// 检查编译结果
if (!result.success) {
    for (const auto& error : result.errors) {
        std::cerr << "Error: " << error << std::endl;
    }
}

// 检查模块加载
if (!module) {
    auto errors = cmod_system.getErrors();
    for (const auto& error : errors) {
        std::cerr << "Module Error: " << error << std::endl;
    }
}
```

## 调试支持

大多数API都支持调试模式：

```cpp
// 启用调试模式
scanner.setDebugMode(true);
lexer.setDebugMode(true);
parser.setDebugMode(true);
generator.setDebugMode(true);
compiler.setDebugMode(true);
cmod_system.setDebugMode(true);
cjmod_system.setDebugMode(true);
cli.setDebugMode(true);
```

调试模式会输出详细的执行信息，帮助开发者诊断问题。

## 性能优化

- 使用智能指针管理内存
- 实现高效的字符串处理
- 优化AST遍历算法
- 提供编译缓存机制
- 支持并行编译

## 线程安全

- 编译器组件是线程安全的
- 模块系统支持多线程访问
- CLI工具支持并发操作
- VSCode扩展使用异步处理

---

更多详细信息请参考源代码和测试用例。