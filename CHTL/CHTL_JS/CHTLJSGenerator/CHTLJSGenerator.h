#ifndef CHTL_JS_GENERATOR_H
#define CHTL_JS_GENERATOR_H

#include "CHTLJSNode/CHTLJSBaseNode.h"
#include "CHTLJSLexer/CHTLJSToken.h"
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>

namespace CHTL_JS {

class CHTLJSGenerator {
private:
    // 生成选项
    bool debugMode;
    bool strictMode;
    bool minifyOutput;
    bool beautifyOutput;
    bool includeComments;
    bool includeSourceMaps;
    
    // 代码生成状态
    int indentLevel;
    std::string indentString;
    std::string currentScope;
    std::vector<std::string> scopeStack;
    
    // 变量和函数管理
    std::map<std::string, std::shared_ptr<CHTLJSBaseNode>> variables;
    std::map<std::string, std::shared_ptr<CHTLJSBaseNode>> functions;
    std::map<std::string, std::shared_ptr<CHTLJSBaseNode>> classes;
    std::map<std::string, std::shared_ptr<CHTLJSBaseNode>> modules;
    
    // 代码生成统计
    int generatedLines;
    int generatedCharacters;
    std::map<std::string, int> featureUsage;
    
    // 错误和警告
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    
    // 代码生成配置
    std::string outputFormat; // "javascript", "chtljs", "html", "css"
    std::string languageVersion;
    std::map<std::string, std::string> customOptions;

public:
    CHTLJSGenerator();
    virtual ~CHTLJSGenerator() = default;
    
    // 基本配置
    void setDebugMode(bool debug) { debugMode = debug; }
    void setStrictMode(bool strict) { strictMode = strict; }
    void setMinifyOutput(bool minify) { minifyOutput = minify; }
    void setBeautifyOutput(bool beautify) { beautifyOutput = beautify; }
    void setIncludeComments(bool include) { includeComments = include; }
    void setIncludeSourceMaps(bool include) { includeSourceMaps = include; }
    void setOutputFormat(const std::string& format) { outputFormat = format; }
    void setLanguageVersion(const std::string& version) { languageVersion = version; }
    void setCustomOption(const std::string& key, const std::string& value) { customOptions[key] = value; }
    
    // 主要生成方法
    std::string generate(std::shared_ptr<CHTLJSBaseNode> ast);
    std::string generateProgram(std::shared_ptr<CHTLJSBaseNode> program);
    std::string generateStatement(std::shared_ptr<CHTLJSBaseNode> statement);
    std::string generateExpression(std::shared_ptr<CHTLJSBaseNode> expression);
    std::string generateDeclaration(std::shared_ptr<CHTLJSBaseNode> declaration);
    
    // 语句生成
    std::string generateExpressionStatement(std::shared_ptr<CHTLJSBaseNode> statement);
    std::string generateBlockStatement(std::shared_ptr<CHTLJSBaseNode> statement);
    std::string generateIfStatement(std::shared_ptr<CHTLJSBaseNode> statement);
    std::string generateForStatement(std::shared_ptr<CHTLJSBaseNode> statement);
    std::string generateWhileStatement(std::shared_ptr<CHTLJSBaseNode> statement);
    std::string generateDoWhileStatement(std::shared_ptr<CHTLJSBaseNode> statement);
    std::string generateSwitchStatement(std::shared_ptr<CHTLJSBaseNode> statement);
    std::string generateCaseStatement(std::shared_ptr<CHTLJSBaseNode> statement);
    std::string generateDefaultStatement(std::shared_ptr<CHTLJSBaseNode> statement);
    std::string generateBreakStatement(std::shared_ptr<CHTLJSBaseNode> statement);
    std::string generateContinueStatement(std::shared_ptr<CHTLJSBaseNode> statement);
    std::string generateReturnStatement(std::shared_ptr<CHTLJSBaseNode> statement);
    std::string generateThrowStatement(std::shared_ptr<CHTLJSBaseNode> statement);
    std::string generateTryStatement(std::shared_ptr<CHTLJSBaseNode> statement);
    std::string generateCatchStatement(std::shared_ptr<CHTLJSBaseNode> statement);
    std::string generateFinallyStatement(std::shared_ptr<CHTLJSBaseNode> statement);
    
    // 表达式生成
    std::string generatePrimaryExpression(std::shared_ptr<CHTLJSBaseNode> expression);
    std::string generateBinaryExpression(std::shared_ptr<CHTLJSBaseNode> expression);
    std::string generateUnaryExpression(std::shared_ptr<CHTLJSBaseNode> expression);
    std::string generateTernaryExpression(std::shared_ptr<CHTLJSBaseNode> expression);
    std::string generateCallExpression(std::shared_ptr<CHTLJSBaseNode> expression);
    std::string generateMemberExpression(std::shared_ptr<CHTLJSBaseNode> expression);
    std::string generateArrayExpression(std::shared_ptr<CHTLJSBaseNode> expression);
    std::string generateObjectExpression(std::shared_ptr<CHTLJSBaseNode> expression);
    std::string generateFunctionExpression(std::shared_ptr<CHTLJSBaseNode> expression);
    std::string generateArrowFunctionExpression(std::shared_ptr<CHTLJSBaseNode> expression);
    
    // 声明生成
    std::string generateVariableDeclaration(std::shared_ptr<CHTLJSBaseNode> declaration);
    std::string generateFunctionDeclaration(std::shared_ptr<CHTLJSBaseNode> declaration);
    std::string generateClassDeclaration(std::shared_ptr<CHTLJSBaseNode> declaration);
    std::string generateInterfaceDeclaration(std::shared_ptr<CHTLJSBaseNode> declaration);
    std::string generateEnumDeclaration(std::shared_ptr<CHTLJSBaseNode> declaration);
    std::string generateNamespaceDeclaration(std::shared_ptr<CHTLJSBaseNode> declaration);
    std::string generateModuleDeclaration(std::shared_ptr<CHTLJSBaseNode> declaration);
    std::string generateImportDeclaration(std::shared_ptr<CHTLJSBaseNode> declaration);
    std::string generateExportDeclaration(std::shared_ptr<CHTLJSBaseNode> declaration);
    
    // CHTL JS特有语法生成
    std::string generateEnhancedSelector(std::shared_ptr<CHTLJSBaseNode> selector);
    std::string generateVirtualObject(std::shared_ptr<CHTLJSBaseNode> virtualObject);
    std::string generateListenExpression(std::shared_ptr<CHTLJSBaseNode> listen);
    std::string generateDelegateExpression(std::shared_ptr<CHTLJSBaseNode> delegate);
    std::string generateAnimateExpression(std::shared_ptr<CHTLJSBaseNode> animate);
    std::string generateRouterExpression(std::shared_ptr<CHTLJSBaseNode> router);
    std::string generateFileloaderExpression(std::shared_ptr<CHTLJSBaseNode> fileloader);
    std::string generateUtilExpression(std::shared_ptr<CHTLJSBaseNode> util);
    
    // 特殊语法生成
    std::string generateDeclarationSyntax(std::shared_ptr<CHTLJSBaseNode> declaration);
    std::string generateKeyValuePairs(std::shared_ptr<CHTLJSBaseNode> pairs);
    std::string generateUnorderedPairs(std::shared_ptr<CHTLJSBaseNode> pairs);
    std::string generateOptionalPairs(std::shared_ptr<CHTLJSBaseNode> pairs);
    std::string generateChainExpression(std::shared_ptr<CHTLJSBaseNode> chain);
    std::string generateUnquotedLiteral(std::shared_ptr<CHTLJSBaseNode> literal);
    
    // 错误处理
    void addError(const std::string& error);
    void addWarning(const std::string& warning);
    void clearErrors();
    void clearWarnings();
    const std::vector<std::string>& getErrors() const { return errors; }
    const std::vector<std::string>& getWarnings() const { return warnings; }
    bool hasErrors() const { return !errors.empty(); }
    bool hasWarnings() const { return !warnings.empty(); }
    
    // 状态管理
    void reset();
    void resetScope();
    void enterScope(const std::string& scope);
    void exitScope();
    std::string getCurrentScope() const { return currentScope; }
    
    // 调试信息
    std::string getDebugInfo() const;
    void printDebugInfo() const;
    
    // 统计信息
    int getGeneratedLines() const { return generatedLines; }
    int getGeneratedCharacters() const { return generatedCharacters; }
    std::map<std::string, int> getFeatureUsage() const { return featureUsage; }
    void resetStatistics();
    
    // 验证
    bool validateOutput(const std::string& output) const;
    std::vector<std::string> validateGeneratedCode(const std::string& code) const;

private:
    // 辅助方法
    std::string getIndent() const;
    void increaseIndent();
    void decreaseIndent();
    std::string formatCode(const std::string& code) const;
    std::string minifyCode(const std::string& code) const;
    std::string beautifyCode(const std::string& code) const;
    
    // 代码生成辅助
    std::string generateNode(std::shared_ptr<CHTLJSBaseNode> node);
    std::string generateChildren(std::shared_ptr<CHTLJSBaseNode> node);
    std::string generateAttributes(std::shared_ptr<CHTLJSBaseNode> node);
    std::string generateComments(std::shared_ptr<CHTLJSBaseNode> node);
    std::string generateDirectives(std::shared_ptr<CHTLJSBaseNode> node);
    
    // 类型转换
    std::string convertToJavaScript(std::shared_ptr<CHTLJSBaseNode> node) const;
    std::string convertToCHTLJS(std::shared_ptr<CHTLJSBaseNode> node) const;
    std::string convertToHTML(std::shared_ptr<CHTLJSBaseNode> node) const;
    std::string convertToCSS(std::shared_ptr<CHTLJSBaseNode> node) const;
    
    // 特殊语法处理
    std::string processEnhancedSelector(const std::string& selector) const;
    std::string processVirtualObject(const std::string& name, std::shared_ptr<CHTLJSBaseNode> value) const;
    std::string processListenExpression(std::shared_ptr<CHTLJSBaseNode> listen) const;
    std::string processDelegateExpression(std::shared_ptr<CHTLJSBaseNode> delegate) const;
    std::string processAnimateExpression(std::shared_ptr<CHTLJSBaseNode> animate) const;
    std::string processRouterExpression(std::shared_ptr<CHTLJSBaseNode> router) const;
    std::string processFileloaderExpression(std::shared_ptr<CHTLJSBaseNode> fileloader) const;
    std::string processUtilExpression(std::shared_ptr<CHTLJSBaseNode> util) const;
    
    // 声明式语法处理
    std::string processDeclarationSyntax(const std::string& keyword, std::shared_ptr<CHTLJSBaseNode> declaration) const;
    std::string processKeyValuePairs(std::vector<std::shared_ptr<CHTLJSBaseNode>> pairs) const;
    std::string processUnorderedPairs(std::vector<std::shared_ptr<CHTLJSBaseNode>> pairs) const;
    std::string processOptionalPairs(std::vector<std::shared_ptr<CHTLJSBaseNode>> pairs) const;
    std::string processChainExpression(std::vector<std::string> elements) const;
    
    // 无修饰字面量处理
    std::string processUnquotedLiteral(const std::string& literal) const;
    bool isValidUnquotedLiteral(const std::string& literal) const;
    
    // 链式语法处理
    std::string processChainSyntax(const std::vector<std::string>& elements) const;
    std::string processChainElement(const std::string& element) const;
    
    // 错误处理辅助
    void reportError(const std::string& message, std::shared_ptr<CHTLJSBaseNode> node);
    void reportWarning(const std::string& message, std::shared_ptr<CHTLJSBaseNode> node);
    std::string formatError(const std::string& message, std::shared_ptr<CHTLJSBaseNode> node) const;
    
    // 调试辅助
    std::string getNodeContext(std::shared_ptr<CHTLJSBaseNode> node) const;
    void printNodeContext(std::shared_ptr<CHTLJSBaseNode> node) const;
    
    // 验证辅助
    bool validateJavaScript(const std::string& code) const;
    bool validateCHTLJS(const std::string& code) const;
    bool validateHTML(const std::string& code) const;
    bool validateCSS(const std::string& code) const;
    
    // 统计辅助
    void updateStatistics(const std::string& feature);
    void updateLineCount(const std::string& code);
    void updateCharacterCount(const std::string& code);
    
    // 格式化辅助
    std::string formatIndent(int level) const;
    std::string formatLine(const std::string& line) const;
    std::string formatBlock(const std::string& block) const;
    std::string formatExpression(const std::string& expression) const;
    std::string formatStatement(const std::string& statement) const;
    std::string formatDeclaration(const std::string& declaration) const;
    
    // 压缩辅助
    std::string removeWhitespace(const std::string& code) const;
    std::string removeComments(const std::string& code) const;
    std::string removeEmptyLines(const std::string& code) const;
    std::string compressIdentifiers(const std::string& code) const;
    
    // 美化辅助
    std::string addIndentation(const std::string& code) const;
    std::string addLineBreaks(const std::string& code) const;
    std::string addSpacing(const std::string& code) const;
    std::string formatBrackets(const std::string& code) const;
    std::string formatOperators(const std::string& code) const;
};

} // namespace CHTL_JS

#endif // CHTL_JS_GENERATOR_H