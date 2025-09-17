# CHTL - C++ HyperText Language

CHTL是一个基于C++语言实现的超文本语言，旨在提供一种更符合开发者编写HTML代码的方式。使用MIT开源协议。

## 项目特性

### 核心功能
- **C++风格语法**: 使用大括号和分号的语法结构，更符合开发者习惯
- **局部样式块**: 在元素内部嵌套style{}，支持内联样式、类选择器、ID选择器
- **模板系统**: 支持样式组模板、元素模板、变量组模板
- **自定义系统**: 提供更灵活的组件化开发方式
- **命名空间**: 有效防止模块污染
- **配置组**: 自定义编译器行为

### CHTL JS扩展
- **ScriptLoader**: AMD风格的JavaScript文件加载器
- **增强选择器**: 使用{{选择器}}语法快速创建DOM对象
- **事件系统**: Listen和事件绑定操作符&->简化事件处理
- **动画系统**: 封装requestAnimationFrame的动画API
- **路由系统**: 快速创建SPA页面架构
- **响应式值**: 与CHTL交互的响应式数据绑定

### 模块系统
- **CMOD**: CHTL模块化分发系统
- **CJMOD**: CHTL JS模块扩展系统
- **官方模块**: 珂朵莉模块和团子模块

## 项目结构

```
CHTL/
├── CHTL/                    # CHTL编译器核心
│   ├── CHTLContext/         # 上下文管理
│   ├── CHTLGenerator/       # 代码生成器
│   ├── CHTLLexer/           # 词法分析器
│   ├── CHTLLoader/          # 文件加载器
│   ├── CHTLNode/            # AST节点
│   ├── CHTLParser/          # 语法分析器
│   ├── CHTLState/           # 状态管理
│   └── CMODSystem/          # CMOD模块系统
├── CHTL JS/                 # CHTL JS编译器
│   ├── CHTLJSContext/       # CHTL JS上下文
│   ├── CHTLJSGenerator/     # CHTL JS代码生成器
│   ├── CHTLJSLexer/         # CHTL JS词法分析器
│   ├── CHTLJSNode/          # CHTL JS AST节点
│   ├── CHTLJSParser/        # CHTL JS语法分析器
│   └── CJMODSystem/         # CJMOD API系统
├── Scanner/                 # 统一扫描器
├── CLI/                     # 命令行工具
├── VSCodeExtension/         # VSCode扩展
├── Module/                  # 官方模块
│   ├── Chtholly/            # 珂朵莉主题模块
│   └── Yuigahama/           # 团子主题模块
└── example.chtl            # 示例文件
```

## 快速开始

### 安装

```bash
# 克隆项目
git clone https://github.com/chtl-team/chtl.git
cd chtl

# 构建项目
mkdir build && cd build
cmake ..
make

# 安装
sudo make install
```

### 基本使用

```bash
# 编译CHTL文件
chtl compile example.chtl -o example.html

# 构建整个项目
chtl build

# 初始化新项目
chtl init my-project

# 模块管理
chtl module install chtholly
chtl module list
```

### 示例代码

```chtl
use html5;

html
{
    head
    {
        title
        {
            "Hello CHTL"
        }
    }
    
    body
    {
        div
        {
            class: container;
            
            style
            {
                .container
                {
                    max-width: 800px;
                    margin: 0 auto;
                    padding: 20px;
                }
            }
            
            h1
            {
                "Welcome to CHTL"
            }
            
            p
            {
                "This is a simple CHTL example."
            }
            
            button
            {
                class: btn;
                
                style
                {
                    .btn
                    {
                        background-color: #007bff;
                        color: white;
                        border: none;
                        padding: 10px 20px;
                        border-radius: 4px;
                        cursor: pointer;
                    }
                }
                
                "Click me"
            }
        }
    }
}
```

## 开发指南

### 构建要求
- C++17 或更高版本
- CMake 3.16 或更高版本
- 支持C++17的编译器（GCC 7+, Clang 5+, MSVC 2017+）

### 开发环境设置

```bash
# 安装依赖
sudo apt-get install build-essential cmake

# 构建项目
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Debug ..
make

# 运行测试
make test
```

### VSCode扩展开发

```bash
cd VSCodeExtension
npm install
npm run compile
```

## 贡献指南

我们欢迎所有形式的贡献！请查看 [CONTRIBUTING.md](CONTRIBUTING.md) 了解详细信息。

### 报告问题
如果您发现了bug或有功能请求，请在 [Issues](https://github.com/chtl-team/chtl/issues) 页面创建新的issue。

### 提交代码
1. Fork 项目
2. 创建特性分支 (`git checkout -b feature/amazing-feature`)
3. 提交更改 (`git commit -m 'Add some amazing feature'`)
4. 推送到分支 (`git push origin feature/amazing-feature`)
5. 创建 Pull Request

## 许可证

本项目使用 MIT 许可证 - 查看 [LICENSE](LICENSE) 文件了解详细信息。

## 致谢

感谢所有为CHTL项目做出贡献的开发者们！

## 联系方式

- 项目主页: https://chtl.dev
- 文档: https://chtl.dev/docs
- 问题反馈: https://github.com/chtl-team/chtl/issues
- 邮箱: contact@chtl.dev

---

**CHTL** - 让HTML开发更简单，更优雅！