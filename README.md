# CHTL - 超文本语言编译器

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![C++17](https://img.shields.io/badge/C++-17-blue.svg)](https://en.cppreference.com/w/cpp/17)
[![CMake](https://img.shields.io/badge/CMake-3.10+-green.svg)](https://cmake.org/)

CHTL是一个基于C++实现的超文本语言，旨在提供更符合开发者编写HTML代码的方式。项目采用MIT开源协议，支持现代Web开发的各种特性。

## ✨ 特性

### 核心特性
- 🚀 **高性能编译** - 基于C++17的高性能编译器
- 🎯 **语法简洁** - 更符合开发者习惯的语法设计
- 🔧 **模块化** - 支持CMOD和CJMOD模块系统
- 🎨 **样式增强** - 局部样式块和属性运算
- 📱 **响应式** - 内置响应式值支持

### CHTL语法特性
- 📝 **文本节点** - `text { "内容" }` 语法
- 🏷️ **元素节点** - 支持所有HTML元素
- 🎨 **局部样式** - 元素内嵌样式块
- 📦 **模板系统** - 样式组、元素、变量组模板
- 🔄 **自定义系统** - 高度灵活的自定义内容
- 📥 **导入系统** - 支持文件导入和命名空间
- ⚙️ **配置系统** - 可自定义关键字和行为

### CHTL JS特性
- 🎭 **虚对象** - `Vir` 语法支持
- 👂 **事件监听** - `Listen` 声明式事件绑定
- 🎬 **动画系统** - `Animate` 简化动画使用
- 🛣️ **路由系统** - `Router` SPA页面支持
- 📜 **脚本加载** - `ScriptLoader` AMD风格加载器
- 🎯 **事件委托** - `Delegate` 解决动态元素问题
- 🔧 **工具函数** - `util` 表达式和条件处理

## 🏗️ 项目结构

```
CHTL/
├── CHTL JS/                    # CHTL JS编译器
│   ├── CHTLJSContext/         # 编译上下文
│   ├── CHTLJSLexer/           # 词法分析器
│   ├── CHTLJSParser/          # 语法分析器
│   └── CHTLJSGenerator/       # 代码生成器
├── CHTL/                      # CHTL编译器
│   ├── CHTLLexer/             # 词法分析器
│   ├── CHTLParser/            # 语法分析器
│   └── CHTLGenerator/         # 代码生成器
├── Scanner/                   # 统一扫描器
├── CompilerDispatcher/        # 编译器调度器
├── CSS/                       # CSS编译器
├── JS/                        # JavaScript编译器
├── Module/                    # 模块系统
└── Test/                      # 测试文件
```

## 🚀 快速开始

### 环境要求
- C++17 或更高版本
- CMake 3.10 或更高版本
- Make 或 Ninja 构建工具

### 安装和构建

1. **克隆项目**
```bash
git clone https://github.com/your-username/chtl.git
cd chtl
```

2. **构建项目**
```bash
# 使用构建脚本
./build.sh

# 或手动构建
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

3. **运行测试**
```bash
./bin/chtl test.chtl output
```

### 基本使用

创建一个简单的CHTL文件 `example.chtl`：

```chtl
use html5;

html
{
    head
    {
        title
        {
            text: "我的CHTL页面";
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
                    width: 100%;
                    max-width: 1200px;
                    margin: 0 auto;
                    padding: 20px;
                }
            }

            h1
            {
                text: "欢迎使用CHTL!";
            }

            button
            {
                text: "点击我";
                
                script
                {
                    {{button}}->Listen
                    {
                        click: () => {
                            console.log("按钮被点击了!");
                        }
                    };
                }
            }
        }
    }
}
```

编译文件：
```bash
./bin/chtl example.chtl output
```

## 📚 语法参考

### 基本语法
```chtl
// 注释
# 生成器注释

// 文本节点
text
{
    "这是一段文本"
}

// 元素节点
div
{
    id: my-div;
    class: container;
    
    text
    {
        Hello World
    }
}
```

### 局部样式
```chtl
div
{
    style
    {
        .container
        {
            width: 100px;
            height: 200px;
            background-color: red;
        }
        
        &:hover
        {
            background-color: blue;
        }
    }
}
```

### 模板系统
```chtl
[Template] @Style DefaultText
{
    color: "black";
    font-size: 16px;
    line-height: 1.6;
}

div
{
    style
    {
        @Style DefaultText;
    }
}
```

### CHTL JS语法
```chtl
// 虚对象
Vir MyObject = Listen
{
    click: () => {
        console.log("点击事件");
    }
};

// 动画
const anim = Animate
{
    target: {{.my-element}},
    duration: 1000,
    easing: ease-in-out,
    begin: {
        opacity: 0
    },
    end: {
        opacity: 1
    }
};
```

## 🔧 开发工具

### CLI工具
```bash
# 编译文件
chtl input.chtl output/

# 调试模式
chtl --debug input.chtl output/

# 指定输出目录
chtl input.chtl --output-dir /path/to/output
```

### VSCode扩展
- 语法高亮
- 代码格式化
- 智能提示
- 实时预览
- 错误检查

## 📦 模块系统

### CMOD模块
```chtl
[Info]
{
    name = "my-module";
    version = "1.0.0";
    description = "我的模块";
    author = "开发者";
    license = "MIT";
}

[Export]
{
    [Custom] @Style MyStyle;
    [Custom] @Element MyComponent;
}
```

### CJMOD模块
```cpp
// C++扩展模块
#include "CJMODAPI.h"

CHTLJSFunction func;
CHTLJSFunction::CreateCHTLJSFunction("myFunction {param: $!}");
```

## 🤝 贡献指南

我们欢迎各种形式的贡献！

### 贡献方式
1. Fork 项目
2. 创建特性分支 (`git checkout -b feature/AmazingFeature`)
3. 提交更改 (`git commit -m 'Add some AmazingFeature'`)
4. 推送到分支 (`git push origin feature/AmazingFeature`)
5. 开启 Pull Request

### 开发规范
- 遵循 C++17 标准
- 使用清晰的变量和函数命名
- 添加必要的注释和文档
- 编写单元测试
- 保持代码风格一致

## 📄 许可证

本项目采用 MIT 许可证 - 查看 [LICENSE](LICENSE) 文件了解详情。

## 🙏 致谢

感谢所有为CHTL项目做出贡献的开发者！

## 📞 联系我们

- 项目主页: https://github.com/your-username/chtl
- 问题反馈: https://github.com/your-username/chtl/issues
- 讨论区: https://github.com/your-username/chtl/discussions

---

**CHTL** - 让Web开发更简单、更高效！ 🚀