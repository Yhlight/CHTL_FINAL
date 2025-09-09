# CHTL 快速开始指南

本指南将帮助您快速上手 CHTL 语言，从安装到创建第一个 CHTL 项目。

## 安装 CHTL

### 系统要求

- C++ 17 兼容编译器
- CMake 3.10+
- 至少 512MB RAM

### 安装步骤

1. **克隆仓库**
   ```bash
   git clone https://github.com/chtl/chtl.git
   cd chtl
   ```

2. **构建项目**
   ```bash
   mkdir build
   cd build
   cmake ..
   make -j4
   ```

3. **验证安装**
   ```bash
   ./CHTL_Compiler --version
   ```

## 第一个 CHTL 项目

### 1. 创建项目目录

```bash
mkdir my-chtl-project
cd my-chtl-project
```

### 2. 创建第一个 CHTL 文件

创建 `index.chtl` 文件：

```chtl
// 我的第一个 CHTL 文件
html
{
    head
    {
        title { "我的第一个 CHTL 页面" }
        meta
        {
            charset: "UTF-8";
        }
        style
        {
            body {
                font-family: Arial, sans-serif;
                margin: 0;
                padding: 20px;
                background-color: #f5f5f5;
            }
            .container {
                max-width: 800px;
                margin: 0 auto;
                background: white;
                padding: 20px;
                border-radius: 8px;
                box-shadow: 0 2px 10px rgba(0,0,0,0.1);
            }
            h1 {
                color: #333;
                text-align: center;
            }
            p {
                line-height: 1.6;
                color: #666;
            }
        }
    }
    body
    {
        div
        {
            class: "container";
            
            h1 { "欢迎使用 CHTL!" }
            
            p
            {
                text
                {
                    "CHTL 是一个强大的超文本语言，让您能够更高效地创建 Web 应用程序。"
                }
            }
            
            p
            {
                text
                {
                    "这个页面展示了 CHTL 的基本语法和功能。"
                }
            }
        }
    }
}
```

### 3. 编译 CHTL 文件

```bash
# 默认编译（生成纯净内容，适合 SPA）
chtl compile index.chtl -o index.html

# 生成带有默认 HTML 结构的完整页面
chtl compile index.chtl -o index.html --default-struct
```

### 4. 查看结果

在浏览器中打开 `index.html` 文件，您将看到一个美观的网页。

**重要说明**：CHTL 编译器默认生成纯净的内容，不包含默认的 HTML 结构（如 `<!DOCTYPE html>`、`<html>`、`<head>`、`<body>` 等）。这种设计特别适合 SPA（单页应用）开发，因为 SPA 框架通常不需要这些默认结构。

如果您需要生成完整的 HTML 页面，请使用 `--default-struct` 选项。

## CHTL 基础语法

### 注释

```chtl
// 单行注释

/*
多行注释
可以跨越多行
*/

-- 生成器识别的注释
```

### 文本节点

```chtl
// 使用 text 关键字
text { "Hello World" }

// 简写形式
text: "Hello World";
```

### 元素节点

```chtl
// HTML 元素
div { "内容" }
span { "文本" }
p { "段落" }

// 嵌套元素
div
{
    h1 { "标题" }
    p { "段落内容" }
}
```

### 属性

```chtl
div
{
    class: "container";
    id: "main-content";
    style: "color: red;";
}
```

### 局部样式块

```chtl
div
{
    class: "styled-div";
    
    style
    {
        background-color: #f0f0f0;
        padding: 20px;
        border-radius: 5px;
    }
    
    text { "带样式的文本" }
}
```

## 模板系统

### 样式组模板

```chtl
[Template] @Style ButtonStyle
{
    background-color: #007bff;
    color: white;
    padding: 10px 20px;
    border: none;
    border-radius: 4px;
    cursor: pointer;
}

button
{
    style
    {
        @Style ButtonStyle;
    }
    text { "点击我" }
}
```

### 元素模板

```chtl
[Template] @Element Card
{
    div
    {
        class: "card";
        style
        {
            border: 1px solid #ddd;
            border-radius: 8px;
            padding: 20px;
            margin: 10px;
        }
        
        h3 { "卡片标题" }
        p { "卡片内容" }
    }
}

// 使用模板
@Element Card;
```

## 自定义系统

### 自定义样式组

```chtl
[Custom] @Style MyButton
{
    background-color,
    color,
    padding;
}

button
{
    style
    {
        @Style MyButton
        {
            background-color: #28a745;
            color: white;
            padding: 12px 24px;
        }
    }
    text { "自定义按钮" }
}
```

## 原始嵌入

### 嵌入 HTML

```chtl
[Origin] @Html
{
    <div class="raw-html">
        <p>这是原始的 HTML 代码</p>
    </div>
}
```

### 嵌入 CSS

```chtl
[Origin] @Style
{
    .custom-style {
        font-size: 18px;
        font-weight: bold;
    }
}
```

### 嵌入 JavaScript

```chtl
[Origin] @JavaScript
{
    console.log("这是原始的 JavaScript 代码");
    document.addEventListener('DOMContentLoaded', function() {
        alert('页面加载完成！');
    });
}
```

## 导入系统

### 导入 CHTL 文件

```chtl
[Import] "components.chtl";
[Import] "styles.chtl" as globalStyles;
[Import] "utils.chtl"::HelperFunction;
```

### 导入其他文件

```chtl
[Import] "external.css";
[Import] "script.js";
[Import] "data.json" as configData;
```

## 配置组

### 基本配置

```chtl
[Configuration] MyConfig
{
    DEBUG_MODE: true;
    INDEX_INITIAL_COUNT: 100;
    DISABLE_NAME_GROUP: false;
}

use MyConfig;
```

### 自定义关键字

```chtl
[Configuration] CustomKeywords
{
    [Name]
    {
        element: "el";
        attribute: "attr";
        text: "txt";
    }
}

use CustomKeywords;

el
{
    attr: "value";
    txt { "使用自定义关键字" }
}
```

## CHTL JS 扩展

### 文件加载器

```chtl
fileloader
{
    load: "jquery.min.js";
    load: "bootstrap.js";
}
```

### 局部脚本

```chtl
div
{
    id: "interactive-div";
    
    script
    {
        // 局部脚本
        const element = {{#interactive-div}};
        element.addEventListener('click', function() {
            alert('点击了交互式元素！');
        });
    }
}
```

### 增强选择器

```chtl
script
{
    const elements = {{.my-class}};
    const element = {{#my-id}};
    const buttons = {{button}};
}
```

### 事件监听器

```chtl
script
{
    listen
    {
        click: "console.log('按钮被点击了！');";
        mouseenter: "this.style.backgroundColor = 'lightblue';";
        mouseleave: "this.style.backgroundColor = '';";
    }
}
```

### 动画

```chtl
script
{
    animate
    {
        target: "{{.fade-in}}";
        duration: 1000;
        easing: "ease-in-out";
        begin: "opacity: 0;";
        end: "opacity: 1;";
    }
}
```

### 虚对象

```chtl
script
{
    vir myObject = function
    {
        name: "MyObject";
        version: "1.0.0";
        log: "这是一个虚对象";
    };
}
```

### 路由

```chtl
script
{
    router
    {
        url: "/";
        page: "home.chtl";
        root: "#app";
        mode: "history";
    }
}
```

## 模块系统

### CMOD 模块

创建 `my-module.cmod`：

```chtl
[Info]
{
    name: "MyModule";
    version: "1.0.0";
    author: "Your Name";
    description: "我的第一个 CHTL 模块";
}

[Export] @Style ModuleStyle
{
    .module-class {
        background-color: #e9ecef;
        padding: 15px;
        border-radius: 5px;
    }
}

[Export] @Element ModuleElement
{
    div
    {
        class: "module-class";
        text { "来自模块的内容" }
    }
}
```

### 使用模块

```chtl
[Import] "my-module.cmod";

div
{
    style
    {
        @Style ModuleStyle;
    }
    @Element ModuleElement;
}
```

## 命令行工具

### 基本用法

```bash
# 编译单个文件
chtl compile input.chtl -o output.html

# 编译多个文件
chtl compile *.chtl -o dist/

# 监听文件变化
chtl watch src/ -o dist/

# 验证语法
chtl validate input.chtl

# 格式化代码
chtl format input.chtl -o formatted.chtl

# 生成文档
chtl docs -o docs/
```

### 高级选项

```bash
# 指定编译模式
chtl compile input.chtl -m release -o output.html

# 启用调试模式
chtl compile input.chtl -d -o output.html

# 指定配置文件
chtl compile input.chtl -c config.json -o output.html

# 生成源映射
chtl compile input.chtl -s -o output.html
```

## SPA 开发支持

CHTL 特别适合 SPA（单页应用）开发，因为：

### 纯净内容生成
- **默认行为**：CHTL 编译器默认生成纯净的内容，不包含默认的 HTML 结构
- **SPA 友好**：生成的内容可以直接集成到 React、Vue、Angular 等 SPA 框架中
- **灵活输出**：支持生成 HTML 片段、CSS 样式和 JavaScript 代码

### 示例对比

**SPA 模式（默认）**：
```bash
chtl compile spa-example.chtl -o spa-content.html
```
生成纯净的内容，适合 SPA 框架使用。

**传统网页模式**：
```bash
chtl compile traditional-page.chtl -o traditional-page.html --default-struct
```
生成完整的 HTML 页面，包含 DOCTYPE、html、head、body 等结构。

### SPA 开发最佳实践

1. **使用纯净模式**：默认不添加 HTML 结构，让 SPA 框架处理
2. **模块化设计**：将页面拆分为多个 CHTL 文件，便于管理
3. **组件化开发**：使用模板和自定义系统创建可复用的组件
4. **样式隔离**：使用局部样式块避免样式冲突

## 下一步

现在您已经了解了 CHTL 的基础知识，可以：

1. 查看 [语言规范](language-specification.md) 了解完整的语法
2. 阅读 [API 参考](api-reference.md) 了解详细的 API
3. 浏览 [示例集合](examples.md) 查看更多示例
4. 学习 [最佳实践](best-practices.md) 提高代码质量

## 获取帮助

如果您遇到问题：

1. 查看 [故障排除](troubleshooting.md) 指南
2. 在 GitHub 上提交 Issue
3. 加入我们的社区讨论
4. 查看示例代码

祝您使用 CHTL 愉快！