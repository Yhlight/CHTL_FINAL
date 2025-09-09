# CHTL 编译器 - Windows 版本

CHTL (C++ Hypertext Language) 是一个基于 C++ 的超文本语言，用于编写 HTML 内容。

## 安装

### 方法 1: 自动安装
1. 解压 `chtl_compiler_windows.tar.gz`
2. 运行 `install_windows.bat`
3. 按照提示完成安装

### 方法 2: 手动安装
1. 解压 `chtl_compiler_windows.tar.gz`
2. 将 `build/bin/chtl.exe` 复制到系统 PATH 目录
3. 将 `build/lib/` 目录复制到合适位置
4. 将 `test_suite/` 目录复制到工作目录

## 使用方法

### 基本用法

```cmd
# 编译 CHTL 文件（纯净输出，适合 SPA）
chtl input.chtl

# 编译 CHTL 文件（完整 HTML 文档）
chtl --default-struct input.chtl

# 查看帮助
chtl --help
```

### 运行测试

```cmd
# 进入测试目录
cd test_suite

# 运行所有测试
run_tests_windows.bat

# 或者运行单个测试
chtl basic_syntax.chtl
chtl --default-struct spa_page.chtl
```

## 测试套件

项目包含完整的测试套件，涵盖 CHTL.md 中的所有功能：

### 1. 基础语法测试 (`basic_syntax.chtl`)
- 注释系统 (`//`, `/* */`, `--`)
- 文本节点 (`text { "..." }`, `text: "..."`)
- 元素节点 (所有 HTML 元素)
- 属性系统 (`attribute_name : "attribute_value"`)
- 局部样式块 (`style {}`)

### 2. 模板系统测试 (`template_system.chtl`)
- 样式组模板 (`[Template] StyleGroup`)
- 元素模板 (`[Template] Element`)
- 变量组模板 (`[Template] VarGroup`)
- 模板继承机制
- 变量引用 (`var(--variable-name)`)

### 3. 自定义系统测试 (`custom_system.chtl`)
- 自定义样式组 (`[Custom] StyleGroup`)
- 自定义元素 (`[Custom] Element`)
- 自定义变量组 (`[Custom] VarGroup`)
- 删除操作 (`delete`)
- 插入操作 (`insert`)

### 4. 原始嵌入系统测试 (`origin_embedding.chtl`)
- HTML 嵌入 (`[Origin] @Html`)
- CSS 嵌入 (`[Origin] @Style`)
- JavaScript 嵌入 (`[Origin] @JavaScript`)

### 5. 导入系统测试 (`import_system.chtl`)
- 文件导入 (`[Import]`)
- 命名空间管理 (`as` 关键字)
- 支持 CHTL、HTML、CSS、JS 文件

### 6. 约束系统测试 (`constraint_system.chtl`)
- 精确约束 (`except`)
- 类型约束
- 全局约束
- 命名空间约束

### 7. 配置组系统测试 (`config_system.chtl`)
- 配置节点 (`[Configuration]`)
- 关键字自定义 (`[Name]`)
- 模块配置 (`use`)
- 自定义关键字使用

### 8. 命名空间系统测试 (`namespace_system.chtl`)
- 命名空间定义 (`[Namespace]`)
- 模块化组织
- 命名空间约束
- 多级命名空间

### 9. CHTL JS 扩展语法测试 (`chtl_js_extended.chtl`)
- 文件载入 (`fileloader {}`)
- 局部脚本 (`script {}`)
- 增强选择器 (`{{CSS选择器}}`)
- 动画系统 (`animate {}`)
- 事件监听 (`listen {}`)
- 工具表达式 (`util ... -> change ... -> then ...`)

### 10. 模块系统测试 (`module_system.chtl`)
- CMOD 模块 (`[Module] CMOD`)
- CJMOD 模块 (`[Module] CJMOD`)
- 模块导入和使用
- 模块 API

### 11. SPA 页面测试 (`spa_page.chtl`)
- 完整的单页应用示例
- 导航栏、主要内容、页脚
- 响应式设计
- 交互功能

### 12. 完整功能测试 (`complete_features.chtl`)
- 所有功能的综合使用
- 复杂的应用示例
- 最佳实践展示

## 输出示例

### 纯净输出（默认模式）
```html
<div><class></class><id></id><header><class></class>text</header><main><class></class><h1>text</h1><p>text</p></main></div>
```

### 默认结构输出（--default-struct）
```html
<!DOCTYPE html>
<html>
<head>
</head>
<body>
<div><class></class><id></id><header><class></class>text</header><main><class></class><h1>text</h1><p>text</p></main></div>
</body>
</html>
```

## 技术特性

- **纯净输出**: 默认生成适合 SPA 页面的纯净 HTML 内容
- **可选默认结构**: 使用 `--default-struct` 参数生成完整的 HTML 文档
- **模板系统**: 支持样式组模板、元素模板、变量组模板及其继承机制
- **自定义系统**: 支持自定义样式组、元素、变量组的特例化操作
- **原始嵌入**: 支持 HTML、CSS、JavaScript 代码的原始嵌入
- **导入系统**: 支持 CHTL、HTML、CSS、JS 文件的导入和命名空间管理
- **约束系统**: 支持精确约束、类型约束和全局约束
- **CHTL JS 扩展**: 支持文件载入、局部脚本、增强选择器等
- **模块系统**: 支持 CMOD 和 CJMOD 模块的创建、导入和管理

## 系统要求

- Windows 10 或更高版本
- 64 位系统
- 无需额外依赖

## 故障排除

### 常见问题

1. **"chtl 不是内部或外部命令"**
   - 确保 CHTL 已添加到系统 PATH
   - 或使用完整路径运行：`C:\path\to\chtl.exe input.chtl`

2. **"找不到指定的文件"**
   - 确保输入文件路径正确
   - 使用相对路径或绝对路径

3. **编译错误**
   - 检查 CHTL 语法是否正确
   - 查看错误信息进行调试

### 获取帮助

```cmd
chtl --help
```

## 许可证

MIT License

## 更新日志

### v1.0.0
- 初始版本发布
- 支持所有 CHTL.md 中描述的功能
- Windows 平台支持
- 完整的测试套件