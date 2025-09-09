# CHTL - 超文本语言编译器

CHTL 是基于 C++ 语言实现的超文本语言，提供更符合开发者编写 HTML 代码的方式。

## 项目结构

```
CHTL/
├── CHTL/                    # CHTL 编译器核心
│   ├── CHTLContext/         # 编译上下文管理
│   ├── CHTLGenerator/       # HTML 代码生成器
│   ├── CHTLLexer/          # 词法分析器
│   │   ├── Lexer.hpp
│   │   ├── GlobalMap.hpp
│   │   └── Token.hpp
│   ├── CHTLLoader/         # 文件加载器
│   ├── CHTLManage/         # 编译器管理器
│   ├── CHTLNode/           # AST 节点定义
│   │   ├── BaseNode.hpp
│   │   ├── ElementNode.hpp
│   │   ├── TextNode.hpp
│   │   ├── CommentNode.hpp
│   │   ├── TemplateNode.hpp
│   │   ├── CustomNode.hpp
│   │   ├── StyleNode.hpp
│   │   ├── ScriptNode.hpp
│   │   ├── OriginNode.hpp
│   │   ├── ImportNode.hpp
│   │   ├── ConfigNode.hpp
│   │   ├── NamespaceNode.hpp
│   │   └── OperatorNode.hpp
│   ├── CHTLParser/         # 语法分析器
│   ├── CHTLState/          # 编译状态管理
│   ├── CHTLIOStream/       # IO 流处理
│   └── CMODSystem/         # CMOD 模块系统
├── CHTL_JS/                # CHTL JS 编译器
│   ├── CHTLJSContext/
│   ├── CHTLJSGenerator/
│   ├── CHTLJSLexer/
│   ├── CHTLJSLoader/
│   ├── CHTLJSManage/
│   ├── CHTLJSNode/
│   ├── CHTLJSParser/
│   ├── CHTLJSState/
│   ├── CHTLJSIOStream/
│   └── CJMODSystem/
├── CSS/                    # CSS 编译器
├── JS/                     # JavaScript 编译器
├── Scanner/                # 统一扫描器
├── CodeMerger/             # 代码合并器
├── CompilerDispatcher/     # 编译器调度器
├── ThirdParty/             # 第三方库
├── Util/                   # 工具类
│   ├── FileSystem/
│   ├── ZipUtil/
│   └── StringUtil/
├── Test/                   # 测试套件
│   ├── UtilTest/
│   ├── TokenTest/
│   └── AstTest/
└── Module/                 # 模块源码
```

## 编译流程

1. **CHTLUnifiedScanner** - 精准代码切割器
2. **CompilerDispatcher** - 编译器调度器
3. **CHTL Compiler** - 手写 CHTL 编译器
4. **CHTL JS Compiler** - 手写 CHTL JS 编译器
5. **CodeMerger** - 代码合并器
6. **CSS/JS Compiler** - ANTLR 编译器
7. **编译结果合并** - HTML 输出

## 验收标准

每个组件都必须通过以下验收测试：
- 单元测试覆盖率 ≥ 90%
- 集成测试通过率 100%
- 性能测试满足要求
- 语法兼容性测试通过
- 错误处理测试通过