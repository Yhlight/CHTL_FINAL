# CHTL - C++ Hypertext Language

CHTL是一个基于C++17实现的超文本语言，旨在提供一种更符合开发者编写HTML代码的方式。CHTL使用MIT开源协议。

## 特性

### 核心语法
- **注释系统**：支持 `//`、`/**/`、`--` 三种注释
- **文本节点**：`text { }` 语法和无修饰字面量
- **元素节点**：支持所有HTML元素，单标签和双标签
- **属性系统**：`属性名 : "属性值"` 语法，支持CE对等式（`:` 和 `=` 等价）
- **局部样式块**：`style { }` 支持内联样式、类选择器、ID选择器、伪类等

### 高级功能
- **属性运算**：支持算术运算符 `+ - * / % **`
- **引用属性**：`CSS选择器.属性` 语法引用其他元素属性
- **属性条件表达式**：支持三元运算符和链式调用
- **模板系统**：样式组模板、元素模板、变量组模板
- **自定义系统**：支持特例化操作（删除、插入、修改）
- **命名空间**：防止模块污染
- **原始嵌入**：支持HTML、CSS、JS代码直接嵌入

### CHTL JS扩展
- **增强选择器**：`{{选择器}}` 语法创建DOM对象
- **增强监听器**：`listen { }` 语法绑定事件
- **事件委托**：`delegate { }` 语法处理动态元素
- **动画系统**：`animate { }` 语法封装requestAnimationFrame
- **虚对象**：`vir` 语法提供元信息访问
- **路由系统**：SPA页面路由支持
- **响应式值**：`$变量名$` 语法实现动态绑定

### 模块系统
- **CMOD**：CHTL模块格式，包含源码和信息文件
- **CJMOD**：CHTL JS模块格式，提供语法扩展
- **CJMOD API**：强大的语法扩展API

## 项目结构

```
CHTL/
├── include/                 # 头文件
│   ├── CHTL/               # CHTL编译器头文件
│   ├── CHTLJS/             # CHTL JS编译器头文件
│   ├── Scanner/            # 统一扫描器头文件
│   └── ...
├── src/                    # 源文件
│   ├── CHTL/               # CHTL编译器实现
│   ├── CHTLJS/             # CHTL JS编译器实现
│   ├── Scanner/            # 统一扫描器实现
│   └── main.cpp            # 主程序入口
├── test.chtl               # 测试文件
├── test_compiler.cpp       # 测试程序
├── CMakeLists.txt          # CMake配置
└── README.md               # 项目说明
```

## 编译和运行

### 依赖要求
- C++17 编译器
- CMake 3.16+
- ANTLR4 (用于CSS/JS编译)

### 编译步骤
```bash
mkdir build
cd build
cmake ..
make
```

### 运行测试
```bash
./test_compiler
```

### 编译CHTL文件
```bash
./chtl test.chtl --output output.html
```

## 使用示例

### 基本HTML结构
```chtl
html
{
    head
    {
        title
        {
            text
            {
                My CHTL Page
            }
        }
    }

    body
    {
        div
        {
            id: main-container;
            class: container;
            
            style
            {
                width: 100%;
                height: 100vh;
                background-color: #f0f0f0;
            }
            
            h1
            {
                text
                {
                    Welcome to CHTL!
                }
            }
        }
    }
}
```

### 局部样式块
```chtl
div
{
    style
    {
        .box
        {
            width: 300px;
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

### CHTL JS功能
```chtl
script
{
    {{.box}}->listen {
        click: () => {
            console.log("Box clicked!");
        }
    };
    
    const anim = animate {
        target: {{.box}},
        duration: 1000,
        begin: {
            opacity: 0
        },
        end: {
            opacity: 1
        }
    };
}
```

## 开发状态

当前项目处于开发阶段，已实现的核心功能包括：

- ✅ 统一扫描器 - 分离CHTL、CHTL JS、CSS、JS代码
- ✅ CHTL节点系统 - 元素、文本、注释、样式、脚本节点
- ✅ CHTL上下文管理 - 配置、命名空间、模板、自定义管理
- ✅ CJMOD系统 - CHTL JS语法扩展API
- ✅ 基本HTML生成 - 从CHTL节点生成HTML代码

正在开发的功能：
- 🔄 CHTL词法分析器
- 🔄 CHTL语法分析器
- 🔄 CHTL JS编译器
- 🔄 模板系统
- 🔄 自定义系统
- 🔄 模块系统

## 贡献

欢迎贡献代码！请遵循以下步骤：

1. Fork 项目
2. 创建特性分支
3. 提交更改
4. 推送到分支
5. 创建 Pull Request

## 许可证

本项目使用 MIT 许可证。详见 LICENSE 文件。

## 联系方式

如有问题或建议，请通过以下方式联系：

- 创建 Issue
- 发送邮件
- 参与讨论

---

**注意**：本项目仍在积极开发中，API可能会发生变化。请关注更新日志。