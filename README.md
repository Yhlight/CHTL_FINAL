# CHTL - C++ HyperText Language

CHTL是一个基于C++语言实现的超文本语言，旨在提供一种更符合开发者编写HTML代码的方式。CHTL使用MIT开源协议。

## 特性

### 核心语法
- **C++风格语法**：使用大括号和分号的语法结构
- **注释系统**：支持`//`、`/* */`、`--`三种注释类型
- **字面量支持**：无修饰字面量、双引号、单引号字符串
- **CE对等式**：`:`与`=`完全等价

### 高级特性
- **局部样式块**：`style {}`支持内联样式、类选择器、ID选择器
- **属性条件表达式**：支持条件判断和链式调用
- **模板系统**：样式组、元素、变量组模板
- **自定义系统**：支持特例化操作
- **导入系统**：支持多种文件类型导入
- **命名空间**：防止模块污染
- **配置系统**：自定义关键字和配置组

### CHTL JS扩展
- **文件加载器**：AMD风格的模块加载
- **局部脚本**：`script {}`支持
- **增强选择器**：`{{选择器}}`语法
- **事件系统**：监听器和事件委托
- **动画系统**：简化的动画API
- **路由系统**：SPA页面支持

### 模块系统
- **CMOD**：CHTL模块格式
- **CJMOD**：CHTL JS模块格式
- **官方模块**：Chtholly、Yuigahama等主题模块

## 快速开始

### 安装

```bash
# 克隆仓库
git clone https://github.com/chtl-team/chtl.git
cd chtl

# 创建构建目录
mkdir build && cd build

# 配置和构建
cmake ..
make -j4

# 安装
sudo make install
```

### 基本用法

```bash
# 编译CHTL文件
chtl input.chtl

# 指定输出目录
chtl -o output/ input.chtl

# 启用压缩和调试模式
chtl -m -d input.chtl
```

### 示例代码

```chtl
// 基本HTML结构
html
{
    head
    {
        title
        {
            text: "Hello CHTL";
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
                text: "Welcome to CHTL!";
            }
        }
    }
}
```

## 项目结构

```
CHTL/
├── include/                 # 头文件
│   ├── CHTL/               # CHTL核心头文件
│   ├── CHTL_JS/            # CHTL JS头文件
│   ├── Scanner/             # 扫描器头文件
│   ├── CodeMerger/          # 代码合并器头文件
│   └── CompilerDispatcher/  # 编译器调度器头文件
├── src/                     # 源代码
│   ├── CHTL/               # CHTL核心实现
│   ├── CHTL_JS/            # CHTL JS实现
│   ├── Scanner/             # 扫描器实现
│   ├── CodeMerger/          # 代码合并器实现
│   ├── CompilerDispatcher/  # 编译器调度器实现
│   └── main.cpp            # 主程序入口
├── examples/               # 示例文件
├── test/                   # 测试文件
├── CMakeLists.txt          # CMake配置
└── README.md              # 项目说明
```

## 开发

### 构建要求
- C++17 或更高版本
- CMake 3.16 或更高版本
- 支持C++17的编译器（GCC 7+, Clang 5+, MSVC 2017+）

### 构建选项
- `BUILD_TESTING`: 启用测试（默认：ON）
- `CHTL_DEBUG`: 启用调试模式（默认：OFF）
- `CHTL_INSTALL`: 安装到系统（默认：ON）

### 运行测试
```bash
cd build
make test
```

## 文档

详细的语法文档请参考 [CHTL.md](CHTL.md)。

## 贡献

欢迎贡献代码！请查看 [CONTRIBUTING.md](CONTRIBUTING.md) 了解如何参与项目。

## 许可证

本项目使用 MIT 许可证。详情请查看 [LICENSE](LICENSE) 文件。

## 联系方式

- 项目主页：https://github.com/chtl-team/chtl
- 问题反馈：https://github.com/chtl-team/chtl/issues
- 讨论区：https://github.com/chtl-team/chtl/discussions