# CHTL - 基于C++的超文本语言

CHTL是一个基于C++语言实现的超文本语言，旨在提供一种更符合开发者编写HTML代码的方式。

## 项目状态

当前项目正在开发中，已完成以下核心组件：

### ✅ 已完成
1. **项目架构设计** - 基于CHTL.md规范建立了完整的项目结构
2. **统一扫描器** - 实现了CHTL、CHTL JS、CSS、JS代码的精准分离
3. **CJMOD API** - 实现了CHTL JS模块的扩展组件，支持语法扩展
4. **CHTL词法分析器** - 支持注释、文本节点、元素节点、属性等基础语法
5. **CHTL节点系统** - 实现了完整的AST节点类型

### 🚧 进行中
1. **CHTL语法分析器** - 正在实现完整的语法解析功能
2. **CHTL JS编译器** - 计划实现增强选择器、虚对象、路由等高级功能

### 📋 待实现
1. **模板系统** - 样式组模板、元素模板、变量组模板及其继承机制
2. **自定义系统** - 自定义样式组、元素、变量组的特例化操作
3. **导入和命名空间系统** - 模块化开发和命名空间管理
4. **CMOD和CJMOD模块系统** - 模块打包、导入和分发
5. **代码合并器** - 将CHTL和CHTL JS处理后的代码与CSS、JS编译器结果合并
6. **CLI工具** - 常规命令行和命令行程序
7. **VSCode扩展** - 代码高亮、格式化、提示、预览等功能

## 项目结构

```
CHTL/
├── CHTL/                    # CHTL编译器核心
│   ├── CHTLContext/         # 上下文管理
│   ├── CHTLGenerator/       # 代码生成器
│   ├── CHTLLexer/          # 词法分析器
│   ├── CHTLParser/         # 语法分析器
│   ├── CHTLNode/           # AST节点系统
│   └── ...
├── CHTL_JS/                # CHTL JS编译器
│   ├── CHTLJSContext/      # CHTL JS上下文
│   ├── CHTLJSGenerator/    # CHTL JS代码生成器
│   ├── CHTLJSLexer/        # CHTL JS词法分析器
│   ├── CHTLJSParser/       # CHTL JS语法分析器
│   ├── CHTLJSNode/         # CHTL JS AST节点
│   └── CJMODSystem/        # CJMOD API系统
├── Scanner/                 # 统一扫描器
├── CodeMerger/             # 代码合并器
├── CompilerDispatcher/     # 编译器调度器
├── Util/                   # 工具类
├── Test/                   # 测试
└── Module/                 # 模块源码
```

## 核心特性

### CHTL语法特性
- 注释系统（//、/* */、#）
- 文本节点和元素节点
- 属性系统（支持字面量和CE对等式）
- 局部样式块（支持内联样式、自动化类名/ID等）
- 模板系统（样式组、元素、变量组模板）
- 自定义系统（支持特例化操作）
- 导入和命名空间系统

### CHTL JS特性
- 增强选择器（{{selector}}）
- 虚对象（vir）
- 增强监听器（listen）
- 动画系统（animate）
- 路由系统（router）
- 脚本加载器（scriptloader）
- 响应式值（$variable$）

### 统一扫描器
- 宽判严判机制
- 占位符机制分离JS和CHTL JS代码
- 支持CHTL、CHTL JS、CSS、JS代码的精准分离

## 编译和运行

```bash
# 创建构建目录
mkdir build && cd build

# 配置CMake
cmake ..

# 编译
make

# 运行测试
./chtl ../test.chtl output.html
```

## 测试文件

项目包含一个测试文件 `test.chtl`，展示了CHTL的基本语法：

```chtl
html
{
    head
    {
        title
        {
            text
            {
                CHTL测试页面
            }
        }
    }

    body
    {
        div
        {
            id: main;
            class: container;

            text
            {
                欢迎使用CHTL！
            }

            style
            {
                .container
                {
                    width: 100%;
                    max-width: 1200px;
                    margin: 0 auto;
                    padding: 20px;
                }

                &:hover
                {
                    background-color: #f0f0f0;
                }
            }
        }

        script
        {
            console.log("Hello from CHTL JS!");
            
            {{main}}->addEventListener('click', () => {
                console.log('Main div clicked!');
            });
        }
    }
}
```

## 开发计划

项目将按照以下顺序逐步实现：

1. **Phase 1**: 核心编译器（CHTL词法分析器、语法分析器、代码生成器）
2. **Phase 2**: CHTL JS编译器（增强选择器、虚对象、路由等）
3. **Phase 3**: 模板和自定义系统
4. **Phase 4**: 模块系统（CMOD、CJMOD）
5. **Phase 5**: 工具链（CLI、VSCode扩展）

## 贡献

欢迎贡献代码！请确保：
1. 遵循项目的代码风格
2. 添加适当的测试
3. 更新相关文档

## 许可证

本项目采用MIT开源协议。