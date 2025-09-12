# 贡献指南

感谢你对CHTL项目的关注！我们欢迎各种形式的贡献，包括代码、文档、测试、反馈等。

## 目录

- [如何贡献](#如何贡献)
- [开发环境设置](#开发环境设置)
- [代码规范](#代码规范)
- [提交流程](#提交流程)
- [测试要求](#测试要求)
- [文档要求](#文档要求)
- [问题报告](#问题报告)
- [功能请求](#功能请求)

## 如何贡献

### 贡献类型

1. **代码贡献**
   - 修复bug
   - 实现新功能
   - 性能优化
   - 代码重构

2. **文档贡献**
   - 完善API文档
   - 编写使用教程
   - 翻译文档
   - 更新README

3. **测试贡献**
   - 编写单元测试
   - 编写集成测试
   - 性能测试
   - 测试用例

4. **其他贡献**
   - 报告bug
   - 提出功能请求
   - 参与讨论
   - 帮助其他用户

## 开发环境设置

### 系统要求

- C++17 或更高版本
- CMake 3.10 或更高版本
- Git
- 现代C++编译器（GCC 7.0+, Clang 5.0+, MSVC 2017+）

### 环境配置

1. **克隆仓库**：
   ```bash
   git clone https://github.com/chtl-project/chtl.git
   cd chtl
   ```

2. **创建开发分支**：
   ```bash
   git checkout -b feature/your-feature-name
   ```

3. **构建项目**：
   ```bash
   mkdir build && cd build
   cmake ..
   make -j4
   ```

4. **运行测试**：
   ```bash
   make test
   ```

### VSCode扩展开发

1. **安装依赖**：
   ```bash
   cd vscode-extension
   npm install
   ```

2. **编译扩展**：
   ```bash
   npm run compile
   ```

3. **运行测试**：
   ```bash
   npm test
   ```

## 代码规范

### C++代码规范

1. **命名规范**：
   ```cpp
   // 类名使用PascalCase
   class CHTLParser {
   public:
       // 方法名使用camelCase
       void parseTokens();
       
       // 私有成员使用下划线前缀
   private:
       std::vector<Token> tokens_;
   };
   ```

2. **注释规范**：
   ```cpp
   /**
    * @brief 解析CHTL代码
    * @param source 源代码
    * @return 解析结果
    */
   ParseResult parse(const std::string& source);
   ```

3. **错误处理**：
   ```cpp
   // 使用异常处理
   try {
       auto result = parser.parse(source);
       return result;
   } catch (const std::exception& e) {
       addError("Parse error: " + std::string(e.what()));
       return nullptr;
   }
   ```

4. **内存管理**：
   ```cpp
   // 使用智能指针
   std::shared_ptr<CHTLNode> node = std::make_shared<CHTLNode>();
   
   // 避免裸指针
   // CHTLNode* node = new CHTLNode(); // 不推荐
   ```

### TypeScript代码规范

1. **命名规范**：
   ```typescript
   // 类名使用PascalCase
   class CHTLLanguageServer {
       // 方法名使用camelCase
       provideCompletionItems() {
           // 实现
       }
   }
   ```

2. **类型定义**：
   ```typescript
   interface CHTLModule {
       name: string;
       version: string;
       description: string;
   }
   ```

3. **错误处理**：
   ```typescript
   try {
       const result = await compileCHTL(source);
       return result;
   } catch (error) {
       console.error('Compilation error:', error);
       throw error;
   }
   ```

### 文件组织

1. **头文件**：
   - 放在`include/`目录下
   - 按模块组织
   - 包含必要的头文件保护

2. **源文件**：
   - 放在`src/`目录下
   - 与头文件对应
   - 实现头文件中声明的功能

3. **测试文件**：
   - 放在`test/`目录下
   - 使用描述性的文件名
   - 包含完整的测试用例

## 提交流程

### 1. 准备工作

1. **Fork仓库**：
   - 在GitHub上fork CHTL仓库
   - 克隆你的fork到本地

2. **同步主分支**：
   ```bash
   git remote add upstream https://github.com/chtl-project/chtl.git
   git fetch upstream
   git checkout main
   git merge upstream/main
   ```

### 2. 开发流程

1. **创建分支**：
   ```bash
   git checkout -b feature/your-feature-name
   ```

2. **编写代码**：
   - 遵循代码规范
   - 编写测试用例
   - 更新文档

3. **提交代码**：
   ```bash
   git add .
   git commit -m "feat: add new feature"
   ```

4. **推送分支**：
   ```bash
   git push origin feature/your-feature-name
   ```

### 3. 创建Pull Request

1. **填写PR模板**：
   - 描述更改内容
   - 说明测试情况
   - 关联相关Issue

2. **等待审查**：
   - 维护者会审查代码
   - 可能需要修改
   - 通过后合并

### 提交信息规范

使用[Conventional Commits](https://www.conventionalcommits.org/)规范：

```
<type>[optional scope]: <description>

[optional body]

[optional footer(s)]
```

类型说明：
- `feat`: 新功能
- `fix`: 修复bug
- `docs`: 文档更新
- `style`: 代码格式调整
- `refactor`: 代码重构
- `test`: 测试相关
- `chore`: 构建过程或辅助工具的变动

示例：
```
feat(parser): add support for nested templates

- Add TemplateNode class for template handling
- Update parser to recognize template syntax
- Add tests for template parsing

Closes #123
```

## 测试要求

### 单元测试

1. **测试覆盖率**：
   - 新代码测试覆盖率应达到80%以上
   - 关键功能测试覆盖率应达到90%以上

2. **测试用例**：
   ```cpp
   #include <gtest/gtest.h>
   #include "CHTL/CHTLParser.h"
   
   TEST(CHTLParserTest, ParseSimpleElement) {
       CHTL::CHTLParser parser;
       std::string source = "div { text { \"Hello\" } }";
       auto result = parser.parse(source);
       
       ASSERT_NE(result, nullptr);
       EXPECT_EQ(result->getType(), CHTL::NodeType::ELEMENT);
   }
   ```

3. **测试命名**：
   - 使用描述性的测试名称
   - 遵循`TestSuiteName_TestName_ExpectedBehavior`格式

### 集成测试

1. **端到端测试**：
   ```cpp
   TEST(CompilerIntegrationTest, CompileToHTML) {
       CHTL::CompilerDispatcher compiler;
       std::string source = "html { body { div { text { \"Hello\" } } } }";
       auto result = compiler.compile(source);
       
       EXPECT_TRUE(result.success);
       EXPECT_FALSE(result.html.empty());
   }
   ```

2. **性能测试**：
   ```cpp
   TEST(PerformanceTest, LargeFileCompilation) {
       // 测试大文件编译性能
       auto start = std::chrono::high_resolution_clock::now();
       // 编译大文件
       auto end = std::chrono::high_resolution_clock::now();
       
       auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
       EXPECT_LT(duration.count(), 1000); // 应在1秒内完成
   }
   ```

### 测试运行

```bash
# 运行所有测试
make test

# 运行特定测试
./build/bin/test_parser

# 运行VSCode扩展测试
cd vscode-extension
npm test
```

## 文档要求

### API文档

1. **头文件注释**：
   ```cpp
   /**
    * @brief CHTL语法分析器
    * 
    * 负责将CHTL词法单元解析为抽象语法树(AST)
    * 支持HTML元素、属性、样式和脚本的解析
    */
   class CHTLParser {
   public:
       /**
        * @brief 解析词法单元
        * @param tokens 词法单元列表
        * @return 解析后的AST根节点
        */
       std::shared_ptr<CHTLNode> parse(const std::vector<Token>& tokens);
   };
   ```

2. **README更新**：
   - 更新功能列表
   - 添加使用示例
   - 更新安装说明

### 用户文档

1. **使用教程**：
   - 编写详细的使用教程
   - 提供完整的示例代码
   - 包含常见问题解答

2. **API文档**：
   - 使用Doxygen生成API文档
   - 包含所有公共接口
   - 提供使用示例

## 问题报告

### Bug报告

使用GitHub Issues报告bug，包含以下信息：

1. **环境信息**：
   - 操作系统版本
   - 编译器版本
   - CHTL版本

2. **复现步骤**：
   - 详细的操作步骤
   - 输入文件内容
   - 预期结果和实际结果

3. **错误信息**：
   - 完整的错误消息
   - 调用栈信息
   - 日志文件

### 功能请求

1. **需求描述**：
   - 详细描述功能需求
   - 说明使用场景
   - 提供参考实现

2. **优先级**：
   - 高：核心功能
   - 中：重要功能
   - 低：可选功能

## 代码审查

### 审查要点

1. **功能正确性**：
   - 代码是否实现了预期功能
   - 是否有边界情况处理
   - 是否有错误处理

2. **代码质量**：
   - 是否遵循代码规范
   - 是否有代码重复
   - 是否有性能问题

3. **测试覆盖**：
   - 是否有足够的测试用例
   - 测试是否覆盖所有场景
   - 测试是否稳定可靠

### 审查流程

1. **自动检查**：
   - CI/CD自动运行测试
   - 代码质量检查
   - 格式检查

2. **人工审查**：
   - 维护者审查代码
   - 提供修改建议
   - 决定是否合并

## 发布流程

### 版本管理

使用[Semantic Versioning](https://semver.org/)：

- `MAJOR`: 不兼容的API修改
- `MINOR`: 向后兼容的功能新增
- `PATCH`: 向后兼容的问题修复

### 发布步骤

1. **准备发布**：
   - 更新版本号
   - 更新CHANGELOG
   - 运行完整测试

2. **创建标签**：
   ```bash
   git tag -a v1.0.0 -m "Release version 1.0.0"
   git push origin v1.0.0
   ```

3. **发布包**：
   - 创建发布包
   - 上传到GitHub Releases
   - 更新文档

## 社区参与

### 讨论渠道

1. **GitHub Discussions**：
   - 功能讨论
   - 使用问题
   - 开发计划

2. **Issue Tracker**：
   - Bug报告
   - 功能请求
   - 技术问题

3. **代码审查**：
   - 参与代码审查
   - 提供改进建议
   - 帮助新贡献者

### 贡献者认可

1. **贡献者列表**：
   - 在README中列出贡献者
   - 在CHANGELOG中记录贡献
   - 在发布说明中感谢贡献者

2. **特殊贡献**：
   - 长期贡献者
   - 重大功能贡献
   - 社区建设贡献

## 联系方式

- **项目维护者**：CHTL Team
- **邮箱**：chtl-team@example.com
- **GitHub**：https://github.com/chtl-project/chtl
- **讨论区**：https://github.com/chtl-project/chtl/discussions

---

感谢你的贡献！让我们一起建设更好的CHTL项目！