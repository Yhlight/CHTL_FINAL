# CHTL项目

基于C++语言实现的超文本语言，其本质是为了提供一种更符合开发者编写HTML代码的方式，使用MIT开源协议。

## 项目状态

### 已完成
- ✅ 项目基础结构建立
- ✅ 核心词法分析器（CHTLLexer）
- ✅ 基础节点系统（BaseNode, ElementNode, TextNode）
- ✅ 统一扫描器（CHTLUnifiedScanner）
- ✅ 代码合并器（CodeMerger）
- ✅ 编译器调度器（CompilerDispatcher）
- ✅ 基础Token系统

### 进行中
- 🔄 语法分析器（CHTLParser）- 部分实现
- 🔄 代码生成器 - 基础框架

### 待实现
- ⏳ 局部样式块系统
- ⏳ 引用属性系统
- ⏳ 属性条件表达式系统
- ⏳ 模板系统
- ⏳ 自定义系统
- ⏳ 原始嵌入系统
- ⏳ 导入系统
- ⏳ 命名空间系统
- ⏳ 约束系统
- ⏳ 配置组系统
- ⏳ use语法
- ⏳ CHTL JS系统
- ⏳ 模块系统
- ⏳ CJMOD API
- ⏳ CLI工具
- ⏳ VSCode扩展
- ⏳ 测试系统
- ⏳ 官方模块

## 项目结构

```
CHTL/
├── CHTL/                    # CHTL核心编译器
│   ├── CHTLContext/         # 上下文管理
│   ├── CHTLGenerator/       # 代码生成器
│   ├── CHTLLexer/          # 词法分析器
│   ├── CHTLLoader/         # 文件加载器
│   ├── CHTLManage/         # 管理器
│   ├── CHTLNode/           # 节点系统
│   ├── CHTLParser/         # 语法分析器
│   ├── CHTLState/          # 状态管理
│   ├── CHTLIOStream/       # IO流
│   └── CMODSystem/         # CMOD系统
├── CHTL_JS/                # CHTL JS编译器
├── CSS/                    # CSS编译器
├── JS/                     # JS编译器
├── Scanner/                # 统一扫描器
├── CodeMerger/             # 代码合并器
├── CompilerDispatcher/     # 编译器调度器
├── ThirdParty/             # 第三方库
├── Util/                   # 工具类
├── Test/                   # 测试
└── Module/                 # 模块源码
```

## 编译和运行

### 基础测试
```bash
cd /workspace/CHTL
mkdir build
cd build
cmake -f ../CMakeLists_test.txt ..
make
./chtl_test
```

### 完整编译
```bash
cd /workspace/CHTL
mkdir build
cd build
cmake ..
make
./chtl_compiler test.chtl output.html
```

## 功能特性

### 已实现的基础功能
1. **词法分析** - 支持CHTL语法的词法分析
2. **基础节点** - HTML元素、文本节点的表示
3. **代码扫描** - 分离不同类型的代码片段
4. **代码合并** - 将编译结果合并为最终HTML

### 计划实现的高级功能
1. **局部样式块** - 在元素内部嵌套样式
2. **属性运算** - 支持CSS属性的算术运算
3. **引用属性** - 引用其他元素的属性值
4. **条件表达式** - 基于属性的条件样式
5. **模板系统** - 可重用的代码模板
6. **自定义系统** - 高度灵活的自定义组件
7. **模块系统** - 模块化开发和分发

## 语法示例

### 基础语法
```chtl
html
{
    head
    {
        title
        {
            "我的网页"
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
                "欢迎使用CHTL！"
            }
        }
    }
}
```

### 局部样式
```chtl
div
{
    style
    {
        width: 100px;
        height: 200px;
        background-color: red;
    }
}
```

## 开发计划

1. **第一阶段** - 核心基础功能（进行中）
2. **第二阶段** - 样式系统
3. **第三阶段** - 模板和自定义系统
4. **第四阶段** - 高级功能
5. **第五阶段** - CHTL JS系统
6. **第六阶段** - 模块系统
7. **第七阶段** - 工具和IDE

## 贡献指南

本项目采用MIT开源协议，欢迎贡献代码和提出建议。

## 许可证

MIT License