#pragma once

#include "CHTL/Common.h"
#include "CHTL/BaseNode.h"
#include "CHTL/CHTLLexer.h"
#include <string>
#include <vector>
#include <memory>
#include <map>

namespace CHTL {

// 前向声明
class CHTLParser;
class CHTLGenerator;
class CHTLContext;
class CHTLState;

/**
 * CHTL编译器 - 处理CHTL语法
 */
class CHTLCompiler {
public:
    // 编译选项
    struct CompileOptions {
        bool enableTemplates = true;
        bool enableCustoms = true;
        bool enableImports = true;
        bool enableNamespaces = true;
        bool enableConfiguration = true;
        bool enableOriginEmbedding = true;
        bool enableStyleBlocks = true;
        bool enableScriptBlocks = true;
        bool enableAttributeOperations = true;
        bool enablePropertyReferences = true;
        bool enableConditionalExpressions = true;
        bool enableResponsiveValues = true;
        bool debugMode = false;
    };

    // 编译结果
    struct CompileResult {
        bool success;
        std::string htmlOutput;
        std::string cssOutput;
        std::string jsOutput;
        std::string errorMessage;
        std::vector<std::string> warnings;
        std::shared_ptr<BaseNode> ast;
        
        CompileResult() : success(false) {}
    };

public:
    CHTLCompiler();
    ~CHTLCompiler();

    /**
     * 编译CHTL代码
     * @param sourceCode 源代码
     * @param options 编译选项
     * @return 编译结果
     */
    CompileResult compile(const std::string& sourceCode, const CompileOptions& options = {});

    /**
     * 从AST生成代码
     * @param ast AST根节点
     * @param options 编译选项
     * @return 编译结果
     */
    CompileResult generateFromAST(std::shared_ptr<BaseNode> ast, const CompileOptions& options = {});

    /**
     * 设置上下文
     * @param context 上下文
     */
    void setContext(std::shared_ptr<CHTLContext> context);

    /**
     * 获取上下文
     * @return 上下文
     */
    std::shared_ptr<CHTLContext> getContext() const;

    /**
     * 注册模板
     * @param name 模板名称
     * @param templateNode 模板节点
     */
    void registerTemplate(const std::string& name, std::shared_ptr<BaseNode> templateNode);

    /**
     * 注册自定义元素
     * @param name 元素名称
     * @param customNode 自定义节点
     */
    void registerCustom(const std::string& name, std::shared_ptr<BaseNode> customNode);

    /**
     * 注册变量组
     * @param name 变量组名称
     * @param varNode 变量节点
     */
    void registerVariableGroup(const std::string& name, std::shared_ptr<BaseNode> varNode);

    /**
     * 解析模板引用
     * @param templateName 模板名称
     * @param arguments 参数
     * @return 解析后的节点
     */
    std::shared_ptr<BaseNode> resolveTemplate(const std::string& templateName, 
                                            const std::map<std::string, std::string>& arguments = {});

    /**
     * 解析自定义元素
     * @param elementName 元素名称
     * @param arguments 参数
     * @return 解析后的节点
     */
    std::shared_ptr<BaseNode> resolveCustom(const std::string& elementName, 
                                          const std::map<std::string, std::string>& arguments = {});

    /**
     * 解析变量组
     * @param varGroupName 变量组名称
     * @param arguments 参数
     * @return 解析后的值
     */
    std::string resolveVariableGroup(const std::string& varGroupName, 
                                   const std::map<std::string, std::string>& arguments = {});

    /**
     * 处理局部样式块
     * @param styleNode 样式节点
     * @param parentElement 父元素
     * @return 处理后的CSS代码
     */
    std::string processLocalStyleBlock(std::shared_ptr<StyleNode> styleNode, 
                                     std::shared_ptr<ElementNode> parentElement);

    /**
     * 处理属性运算
     * @param expression 表达式
     * @param context 上下文
     * @return 计算结果
     */
    std::string evaluateAttributeExpression(const std::string& expression, 
                                          const std::map<std::string, std::string>& context);

    /**
     * 处理属性条件表达式
     * @param expression 表达式
     * @param context 上下文
     * @return 计算结果
     */
    std::string evaluateConditionalExpression(const std::string& expression, 
                                            const std::map<std::string, std::string>& context);

    /**
     * 处理引用属性
     * @param reference 引用表达式
     * @param context 上下文
     * @return 引用的值
     */
    std::string resolvePropertyReference(const std::string& reference, 
                                       const std::map<std::string, std::string>& context);

    /**
     * 处理响应式值
     * @param expression 表达式
     * @param context 上下文
     * @return 处理后的表达式
     */
    std::string processResponsiveValue(const std::string& expression, 
                                     const std::map<std::string, std::string>& context);

private:
    // 初始化编译器组件
    void initializeComponents();
    
    // 词法分析
    std::vector<Token> tokenize(const std::string& sourceCode);
    
    // 语法分析
    std::shared_ptr<BaseNode> parse(const std::vector<Token>& tokens);
    
    // 语义分析
    void semanticAnalysis(std::shared_ptr<BaseNode> ast);
    
    // 代码生成
    CompileResult generateCode(std::shared_ptr<BaseNode> ast, const CompileOptions& options);
    
    // 模板处理
    void processTemplates(std::shared_ptr<BaseNode> ast);
    void processCustoms(std::shared_ptr<BaseNode> ast);
    void processImports(std::shared_ptr<BaseNode> ast);
    void processNamespaces(std::shared_ptr<BaseNode> ast);
    void processConfiguration(std::shared_ptr<BaseNode> ast);
    
    // 样式处理
    void processStyleBlocks(std::shared_ptr<BaseNode> ast);
    void processScriptBlocks(std::shared_ptr<BaseNode> ast);
    
    // 错误处理
    void handleError(const std::string& error, CompileResult& result);
    void handleWarning(const std::string& warning, CompileResult& result);

private:
    std::unique_ptr<CHTLLexer> lexer_;
    std::unique_ptr<CHTLParser> parser_;
    std::unique_ptr<CHTLGenerator> generator_;
    std::shared_ptr<CHTLContext> context_;
    std::unique_ptr<CHTLState> state_;
    
    // 注册表
    std::map<std::string, std::shared_ptr<BaseNode>> templates_;
    std::map<std::string, std::shared_ptr<BaseNode>> customs_;
    std::map<std::string, std::shared_ptr<BaseNode>> variableGroups_;
    
    // 编译选项
    CompileOptions currentOptions_;
};

} // namespace CHTL