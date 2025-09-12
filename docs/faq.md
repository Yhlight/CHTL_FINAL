# CHTL 常见问题解答

本文档回答了CHTL项目开发和使用过程中的常见问题。

## 目录

- [安装和构建](#安装和构建)
- [编译问题](#编译问题)
- [运行时问题](#运行时问题)
- [模块系统](#模块系统)
- [CLI工具](#cli工具)
- [VSCode扩展](#vscode扩展)
- [性能问题](#性能问题)
- [错误处理](#错误处理)

## 安装和构建

### Q: 如何安装CHTL编译器？

A: 有几种安装方式：

1. **从源码构建**：
   ```bash
   git clone <repository-url>
   cd CHTL
   mkdir build && cd build
   cmake ..
   make -j4
   sudo make install
   ```

2. **使用包管理器**（如果可用）：
   ```bash
   # Ubuntu/Debian
   sudo apt install chtl
   
   # macOS
   brew install chtl
   
   # Windows
   choco install chtl
   ```

### Q: 构建时出现C++17错误怎么办？

A: 确保你的编译器支持C++17：

```bash
# 检查GCC版本
gcc --version

# 检查Clang版本
clang --version

# 如果版本过低，请升级编译器
# Ubuntu/Debian
sudo apt update
sudo apt install gcc-9 g++-9

# 设置默认版本
sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-9 90
sudo update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-9 90
```

### Q: CMake找不到依赖怎么办？

A: 确保安装了必要的依赖：

```bash
# Ubuntu/Debian
sudo apt install build-essential cmake

# macOS
brew install cmake

# Windows
# 安装Visual Studio Build Tools
```

## 编译问题

### Q: 编译CHTL文件时出现段错误怎么办？

A: 这通常是由于语法错误或无限递归导致的：

1. **检查语法**：
   ```bash
   chtl compile input.chtl --debug
   ```

2. **检查括号匹配**：
   ```chtl
   // 错误示例
   div { text { "Hello" }  // 缺少右括号
   
   // 正确示例
   div { text { "Hello" } }
   ```

3. **检查嵌套深度**：
   ```chtl
   // 避免过深的嵌套
   div { div { div { div { text { "Too deep" } } } } }
   ```

### Q: 编译输出为空怎么办？

A: 检查以下几点：

1. **检查输入文件**：
   ```bash
   cat input.chtl
   ```

2. **检查文件编码**：
   ```bash
   file input.chtl
   ```

3. **使用调试模式**：
   ```bash
   chtl compile input.chtl --debug
   ```

4. **检查语法**：
   ```chtl
   // 确保有完整的HTML结构
   html {
       head { title { text { "Title" } } }
       body { div { text { "Content" } } }
   }
   ```

### Q: 编译速度很慢怎么办？

A: 优化编译性能：

1. **使用并行编译**：
   ```bash
   make -j4  # 使用4个并行任务
   ```

2. **启用编译缓存**：
   ```bash
   chtl compile input.chtl --cache
   ```

3. **减少调试信息**：
   ```bash
   chtl compile input.chtl --no-debug
   ```

## 运行时问题

### Q: 运行时出现内存泄漏怎么办？

A: CHTL使用智能指针管理内存，但需要注意：

1. **检查循环引用**：
   ```cpp
   // 避免循环引用
   class Node {
       std::shared_ptr<Node> parent;
       std::weak_ptr<Node> child;  // 使用weak_ptr避免循环引用
   };
   ```

2. **及时释放资源**：
   ```cpp
   {
       auto node = std::make_shared<CHTLNode>();
       // 使用node
   } // 自动释放
   ```

### Q: 运行时出现段错误怎么办？

A: 使用调试工具诊断：

1. **使用GDB**：
   ```bash
   gdb ./chtl
   (gdb) run compile input.chtl
   (gdb) bt  # 查看调用栈
   ```

2. **使用Valgrind**：
   ```bash
   valgrind --tool=memcheck ./chtl compile input.chtl
   ```

3. **使用AddressSanitizer**：
   ```bash
   g++ -fsanitize=address -g -o chtl_debug src/*.cpp
   ./chtl_debug compile input.chtl
   ```

## 模块系统

### Q: 模块加载失败怎么办？

A: 检查模块结构和路径：

1. **检查模块结构**：
   ```
   my-module/
   ├── src/
   │   └── my-module.chtl
   └── info/
       └── my-module.chtl
   ```

2. **检查模块信息**：
   ```chtl
   [Info] {
       name = "my-module";
       version = "1.0.0";
       description = "My module";
       author = "Developer";
       license = "MIT";
       category = "theme";
   }
   ```

3. **检查搜索路径**：
   ```cpp
   cmod_system.initialize("./modules", "./user_modules");
   ```

### Q: 模块导出表为空怎么办？

A: 确保模块有正确的导出块：

```chtl
[Export] {
    button: "Button component";
    card: "Card component";
    style: "Custom styles";
}
```

### Q: CJMOD函数执行失败怎么办？

A: 检查函数注册和执行：

1. **检查函数注册**：
   ```cpp
   cjmod_system.registerFunction("my-module", "hello", 
       [](const std::vector<std::string>& args) -> std::string {
           return "Hello from CJMOD!";
       });
   ```

2. **检查函数调用**：
   ```cpp
   auto result = cjmod_system.executeFunction("my-module", "hello", {});
   if (result.empty()) {
       auto errors = cjmod_system.getErrors();
       // 处理错误
   }
   ```

## CLI工具

### Q: CLI命令不工作怎么办？

A: 检查命令语法和参数：

1. **检查命令格式**：
   ```bash
   chtl compile input.chtl -o output.html
   chtl watch src/ -o dist/
   chtl init my-project
   ```

2. **检查文件路径**：
   ```bash
   ls -la input.chtl
   ```

3. **使用帮助**：
   ```bash
   chtl --help
   chtl compile --help
   ```

### Q: 监视模式不工作怎么办？

A: 检查文件系统和权限：

1. **检查文件权限**：
   ```bash
   ls -la src/
   chmod 644 src/*.chtl
   ```

2. **检查文件系统**：
   ```bash
   # 某些文件系统不支持文件监视
   mount | grep tmpfs
   ```

3. **使用轮询模式**：
   ```bash
   chtl watch src/ --poll
   ```

## VSCode扩展

### Q: 扩展不工作怎么办？

A: 检查扩展安装和配置：

1. **检查扩展安装**：
   - 打开VSCode
   - 按Ctrl+Shift+X打开扩展面板
   - 搜索"CHTL Language"
   - 确保已安装并启用

2. **检查文件关联**：
   ```json
   {
       "files.associations": {
           "*.chtl": "chtl",
           "*.cmod": "chtl"
       }
   }
   ```

3. **检查配置**：
   ```json
   {
       "chtl.compilerPath": "chtl",
       "chtl.outputDirectory": "dist",
       "chtl.autoCompile": true
   }
   ```

### Q: 语法高亮不工作怎么办？

A: 检查语言配置：

1. **检查文件扩展名**：
   - 确保文件以`.chtl`或`.cmod`结尾

2. **手动设置语言**：
   - 按Ctrl+Shift+P
   - 输入"Change Language Mode"
   - 选择"CHTL"

3. **重新加载窗口**：
   - 按Ctrl+Shift+P
   - 输入"Developer: Reload Window"

### Q: 代码补全不工作怎么办？

A: 检查语言服务器：

1. **检查语言服务器状态**：
   - 查看VSCode状态栏
   - 确保语言服务器正在运行

2. **检查配置**：
   ```json
   {
       "chtl.enableCompletion": true,
       "chtl.enableHover": true
   }
   ```

3. **重启语言服务器**：
   - 按Ctrl+Shift+P
   - 输入"Developer: Restart Extension Host"

## 性能问题

### Q: 编译速度很慢怎么办？

A: 优化编译性能：

1. **使用并行编译**：
   ```bash
   make -j$(nproc)
   ```

2. **启用编译缓存**：
   ```bash
   chtl compile input.chtl --cache
   ```

3. **减少调试信息**：
   ```bash
   chtl compile input.chtl --no-debug
   ```

4. **使用增量编译**：
   ```bash
   chtl watch src/ --incremental
   ```

### Q: 内存使用过高怎么办？

A: 优化内存使用：

1. **使用内存池**：
   ```cpp
   // 使用内存池减少分配开销
   class MemoryPool {
       // 实现内存池
   };
   ```

2. **及时释放资源**：
   ```cpp
   {
       auto result = compiler.compile(source);
       // 使用result
   } // 自动释放
   ```

3. **使用移动语义**：
   ```cpp
   std::string result = std::move(compiler.compile(source));
   ```

## 错误处理

### Q: 如何获取详细的错误信息？

A: 使用调试模式：

1. **CLI工具**：
   ```bash
   chtl compile input.chtl --debug --verbose
   ```

2. **编程接口**：
   ```cpp
   compiler.setDebugMode(true);
   auto result = compiler.compile(source);
   if (!result.success) {
       for (const auto& error : result.errors) {
           std::cerr << "Error: " << error << std::endl;
       }
   }
   ```

3. **VSCode扩展**：
   ```json
   {
       "chtl.debug": true,
       "chtl.verbose": true
   }
   ```

### Q: 如何报告错误？

A: 提供以下信息：

1. **系统信息**：
   - 操作系统版本
   - 编译器版本
   - CHTL版本

2. **错误信息**：
   - 完整的错误消息
   - 调用栈信息
   - 输入文件内容

3. **复现步骤**：
   - 详细的操作步骤
   - 最小复现示例
   - 预期结果和实际结果

### Q: 如何调试CHTL代码？

A: 使用调试工具：

1. **语法检查**：
   ```bash
   chtl compile input.chtl --syntax-check
   ```

2. **AST输出**：
   ```bash
   chtl compile input.chtl --ast
   ```

3. **词法分析**：
   ```bash
   chtl compile input.chtl --tokens
   ```

4. **使用GDB**：
   ```bash
   gdb ./chtl
   (gdb) run compile input.chtl
   (gdb) break main
   (gdb) run
   ```

---

如果以上解答没有解决你的问题，请：

1. 查看[API文档](api.md)
2. 提交[Issue](https://github.com/chtl-project/chtl/issues)
3. 参与[讨论](https://github.com/chtl-project/chtl/discussions)
4. 联系开发团队