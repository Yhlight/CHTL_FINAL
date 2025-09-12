# CHTL - 超文本语言编译器

CHTL是一个基于C++17实现的超文本语言，旨在提供一种更符合开发者编写HTML代码的方式。CHTL使用MIT开源协议。

## 项目特性

### 核心特性
- **注释系统**：支持`//`、`/**/`、`--`三种注释
- **文本节点**：`text {}` 和 `text:` 属性两种方式
- **字面量支持**：无修饰字面量、双引号、单引号字符串
- **CE对等式**：`:` 和 `=` 完全等价
- **元素节点**：支持所有HTML元素

### 样式系统
- **局部样式块**：`style {}` 支持内联样式、类选择器、ID选择器
- **属性运算**：支持算术运算符 `+ - * / % **`
- **引用属性**：支持引用其他元素的属性值
- **属性条件表达式**：支持条件判断和链式调用

### 模板系统
- **样式组模板**：`[Template] @Style`
- **元素模板**：`[Template] @Element`
- **变量组模板**：`[Template] @Var`
- **组合继承**：支持模板间的继承

### 自定义系统
- **自定义样式组**：支持无值样式组和特例化
- **自定义元素**：支持索引访问、插入、删除操作
- **变量组特例化**：支持变量值的特例化

### CHTL JS扩展
- **增强选择器**：`{{选择器}}` 语法
- **增强监听器**：`listen {}` 语法
- **动画系统**：`animate {}` 语法
- **虚对象**：`vir` 语法
- **路由系统**：`router {}` 语法
- **响应式值**：`$变量名$` 语法

## 项目结构

```
CHTL/
├── include/CHTL/           # 头文件
│   ├── CHTLContext.h      # 上下文管理
│   ├── Token.h            # Token定义
│   ├── CHTLLexer.h        # 词法分析器
│   ├── Scanner/           # 扫描器
│   └── CHTLNode/          # 节点系统
├── src/                   # 源文件
│   ├── CHTLContext.cpp
│   ├── Token.cpp
│   ├── CHTLLexer.cpp
│   ├── Scanner/
│   └── CHTLNode/
├── CMakeLists.txt         # CMake配置
├── build.sh              # 构建脚本
└── README.md             # 项目说明
```

## 构建说明

### 依赖要求
- C++17 编译器 (GCC 7+ 或 Clang 5+)
- CMake 3.16+
- Make 或 Ninja

### 构建步骤

1. 克隆项目
```bash
git clone <repository-url>
cd CHTL
```

2. 运行构建脚本
```bash
./build.sh
```

3. 或者手动构建
```bash
mkdir build
cd build
cmake ..
make -j$(nproc)
```

### 运行测试

```bash
# 编译测试文件
./build/bin/chtl -v test.chtl

# 查看输出
cat test.html
```

## 使用示例

### 基本语法

```chtl
html
{
    head
    {
        title
        {
            text
            {
                我的网页
            }
        }
    }
    
    body
    {
        div
        {
            id: main-container;
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
                
                background-color: #f0f0f0;
                color: #333;
            }
        }
    }
}
```

### 模板使用

```chtl
[Template] @Style DefaultText
{
    color: "black";
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
div
{
    script
    {
        {{box}}->listen {
            click: () => {
                console.log('Box clicked!');
            }
        };
    }
}
```

## 开发状态

当前项目处于开发阶段，已实现的功能包括：

- ✅ 项目架构设计
- ✅ 统一扫描器
- ✅ 词法分析器
- ✅ 基础节点系统
- ✅ CLI工具框架
- 🔄 语法分析器（进行中）
- ⏳ 代码生成器
- ⏳ 模板系统
- ⏳ 自定义系统
- ⏳ 模块系统

## 贡献指南

1. Fork 项目
2. 创建特性分支 (`git checkout -b feature/AmazingFeature`)
3. 提交更改 (`git commit -m 'Add some AmazingFeature'`)
4. 推送到分支 (`git push origin feature/AmazingFeature`)
5. 打开 Pull Request

## 许可证

本项目使用 MIT 许可证 - 查看 [LICENSE](LICENSE) 文件了解详情。

## 联系方式

如有问题或建议，请通过以下方式联系：

- 创建 Issue
- 发送邮件
- 参与讨论

---

**注意**：本项目仍在积极开发中，API可能会发生变化。请关注更新日志获取最新信息。