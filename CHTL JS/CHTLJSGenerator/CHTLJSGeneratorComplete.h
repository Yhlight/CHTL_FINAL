#ifndef CHTLJS_GENERATOR_COMPLETE_H
#define CHTLJS_GENERATOR_COMPLETE_H

#include "CHTLJSGenerator.h"
#include "../CHTLJSNode/CHTLJSBaseNode.h"
#include "../CHTLJSContext/CHTLJSContext.h"
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>

namespace CHTLJS {

/**
 * 完整的CHTL JS代码生成器
 * 将CHTL JS AST转换为JavaScript代码
 * 严格遵循CHTL.md中定义的CHTL JS语法规范
 * CHTL JS是独立的编程语言，编译为JavaScript
 */
class CHTLJSGeneratorComplete : public CHTLJSGenerator {
public:
    CHTLJSGeneratorComplete(std::shared_ptr<CHTLJSContext> context = nullptr);
    ~CHTLJSGeneratorComplete();
    
    // 重写基类方法
    std::string generate(std::shared_ptr<CHTLJSBaseNode> ast) override;
    std::string generateDocument(const std::vector<std::shared_ptr<CHTLJSBaseNode>>& statements) override;
    
    // CHTL JS特有语法生成
    std::string generateScriptLoaderSystem(std::shared_ptr<CHTLJSBaseNode> node);
    std::string generateListenSystem(std::shared_ptr<CHTLJSBaseNode> node);
    std::string generateAnimateSystem(std::shared_ptr<CHTLJSBaseNode> node);
    std::string generateRouterSystem(std::shared_ptr<CHTLJSBaseNode> node);
    std::string generateVirSystem(std::shared_ptr<CHTLJSBaseNode> node);
    std::string generateUtilThenSystem(std::shared_ptr<CHTLJSBaseNode> node);
    std::string generatePrintMyloveSystem(std::shared_ptr<CHTLJSBaseNode> node);
    std::string generateINeverAwaySystem(std::shared_ptr<CHTLJSBaseNode> node);
    
    // CHTL JS特殊表达式生成
    std::string generateResponsiveGetExpression(std::shared_ptr<CHTLJSBaseNode> node);
    std::string generateResponsiveSetExpression(std::shared_ptr<CHTLJSBaseNode> node);
    std::string generateCHTLSelectorExpression(std::shared_ptr<CHTLJSBaseNode> node);
    std::string generateCHTLResponsiveExpression(std::shared_ptr<CHTLJSBaseNode> node);
    std::string generateBindExpression(std::shared_ptr<CHTLJSBaseNode> node);
    std::string generateArrowExpression(std::shared_ptr<CHTLJSBaseNode> node);
    
    // CHTL JS运行时系统生成
    std::string generateReactiveVariableSystem();
    std::string generateVirtualObjectSystemCore();
    std::string generateEventSystemCore();
    std::string generateAnimationSystemCore();
    std::string generateRouterSystemCore();
    std::string generateModuleSystemCore();
    std::string generateUtilSystemCore();
    
    // 运行时支持函数生成
    std::string generateReactiveGetFunction();
    std::string generateReactiveSetFunction();
    std::string generateEventListenerFunction();
    std::string generateAnimationFunction();
    std::string generateRouterFunction();
    std::string generateVirtualObjectFunction();
    std::string generatePrintMyloveFunction();
    std::string generateINeverAwayFunction();
    std::string generateUtilThenFunction();
    
    // 选择器和DOM操作生成
    std::string generateCHTLSelectorFunction();
    std::string generateDOMQueryFunction();
    std::string generateDOMManipulationFunction();
    
    // 生成配置
    void setGenerateRuntime(bool generate);
    bool getGenerateRuntime() const;
    void setMinifyOutput(bool minify);
    bool getMinifyOutput() const;
    void setGenerateSourceMap(bool generate);
    bool getGenerateSourceMap() const;
    
    // 错误处理
    void setErrorHandler(std::function<void(const std::string&, int, int)> handler);
    void reportError(const std::string& message, int line = 0, int column = 0);
    
    // 统计信息
    size_t getGeneratedLinesCount() const;
    size_t getGeneratedCodeSize() const;
    std::string getGenerationStatistics() const;

private:
    std::shared_ptr<CHTLJSContext> context_;
    std::function<void(const std::string&, int, int)> error_handler_;
    
    // 生成配置
    bool generate_runtime_;
    bool minify_output_;
    bool generate_source_map_;
    
    // 统计信息
    size_t generated_lines_;
    size_t generated_code_size_;
    
    // 代码模板
    std::map<std::string, std::string> code_templates_;
    
    // 辅助生成方法
    std::string generateRuntimeSetup();
    std::string generateRuntimeCleanup();
    std::string generateImportStatements();
    std::string generateExportStatements();
    
    // CHTL JS特有功能实现
    std::string generateReactiveVariableImplementation();
    std::string generateVirtualObjectImplementation();
    std::string generateEventSystemImplementation();
    std::string generateAnimationSystemImplementation();
    std::string generateRouterSystemImplementation();
    std::string generateModuleSystemImplementation();
    
    // 代码优化
    std::string optimizeGeneratedCode(const std::string& code);
    std::string minifyCode(const std::string& code);
    std::string removeUnusedCode(const std::string& code);
    std::string optimizeVariableNames(const std::string& code);
    
    // 源码映射
    std::string generateSourceMap(const std::string& originalSource, const std::string& generatedCode);
    
    // 工具方法
    std::string generateVariableName(const std::string& prefix = "var");
    std::string generateFunctionName(const std::string& prefix = "func");
    std::string generateClassName(const std::string& prefix = "Class");
    
    // 代码验证
    bool validateGeneratedCode(const std::string& code);
    bool isValidJavaScript(const std::string& code);
    
    // 错误处理
    void logError(const std::string& message, int line = 0, int column = 0);
    void validateNode(std::shared_ptr<CHTLJSBaseNode> node);
    
    // 初始化
    void initializeCodeTemplates();
    void initializeDefaultSettings();
};

} // namespace CHTLJS

#endif // CHTLJS_GENERATOR_COMPLETE_H