#ifndef CHTLJS_GENERATOR_H
#define CHTLJS_GENERATOR_H

#include "../CHTLJSParser/ASTNode.h"
#include <string>
#include <memory>
#include <unordered_map>
#include <vector>

namespace CHTLJS {

class CHTLJSGenerator {
private:
    std::string output;
    int indentLevel;
    bool minify;
    
    void indent();
    void newline();
    void addIndent();
    void removeIndent();
    
    std::string escapeString(const std::string& str) const;
    std::string formatNumber(const std::string& num) const;
    std::string formatBoolean(const std::string& boolVal) const;
    
    // 基础代码生成
    std::string generateProgram(std::shared_ptr<ASTNode> node);
    std::string generateStatement(std::shared_ptr<ASTNode> node);
    std::string generateExpression(std::shared_ptr<ASTNode> node);
    
    // 声明生成
    std::string generateVariableDeclaration(std::shared_ptr<ASTNode> node);
    std::string generateFunctionDeclaration(std::shared_ptr<ASTNode> node);
    std::string generateClassDeclaration(std::shared_ptr<ASTNode> node);
    
    // 表达式生成
    std::string generateIdentifier(std::shared_ptr<ASTNode> node);
    std::string generateLiteral(std::shared_ptr<ASTNode> node);
    std::string generateBinaryExpression(std::shared_ptr<ASTNode> node);
    std::string generateUnaryExpression(std::shared_ptr<ASTNode> node);
    std::string generateAssignmentExpression(std::shared_ptr<ASTNode> node);
    std::string generateCallExpression(std::shared_ptr<ASTNode> node);
    std::string generateMemberExpression(std::shared_ptr<ASTNode> node);
    std::string generateArrayExpression(std::shared_ptr<ASTNode> node);
    std::string generateObjectExpression(std::shared_ptr<ASTNode> node);
    std::string generateConditionalExpression(std::shared_ptr<ASTNode> node);
    
    // 语句生成
    std::string generateExpressionStatement(std::shared_ptr<ASTNode> node);
    std::string generateBlockStatement(std::shared_ptr<ASTNode> node);
    std::string generateIfStatement(std::shared_ptr<ASTNode> node);
    std::string generateWhileStatement(std::shared_ptr<ASTNode> node);
    std::string generateForStatement(std::shared_ptr<ASTNode> node);
    std::string generateReturnStatement(std::shared_ptr<ASTNode> node);
    std::string generateBreakStatement(std::shared_ptr<ASTNode> node);
    std::string generateContinueStatement(std::shared_ptr<ASTNode> node);
    std::string generateTryStatement(std::shared_ptr<ASTNode> node);
    std::string generateThrowStatement(std::shared_ptr<ASTNode> node);
    
    // CHTL JS 特有生成
    std::string generateVirDeclaration(std::shared_ptr<ASTNode> node);
    std::string generateListenExpression(std::shared_ptr<ASTNode> node);
    std::string generateAnimateExpression(std::shared_ptr<ASTNode> node);
    std::string generateRouterExpression(std::shared_ptr<ASTNode> node);
    std::string generateScriptLoaderExpression(std::shared_ptr<ASTNode> node);
    std::string generateDelegateExpression(std::shared_ptr<ASTNode> node);
    std::string generateSelectorExpression(std::shared_ptr<ASTNode> node);
    std::string generateReactiveValueExpression(std::shared_ptr<ASTNode> node);
    
    // 辅助生成
    std::string generateProperty(std::shared_ptr<ASTNode> node);
    std::string generateParameterList(const std::vector<std::string>& params);
    std::string generateArgumentList(const std::vector<std::shared_ptr<ASTNode>>& args);
    
    // 工具方法
    std::string getOperatorString(TokenType type) const;
    bool needsParentheses(std::shared_ptr<ASTNode> node, TokenType parentOp) const;
    int getOperatorPrecedence(TokenType type) const;
    
public:
    CHTLJSGenerator(bool minifyOutput = false);
    ~CHTLJSGenerator() = default;
    
    std::string generate(std::shared_ptr<ASTNode> ast);
    std::string generate(const std::string& source);
    
    void setMinify(bool minifyOutput);
    bool isMinify() const;
    
    void reset();
    std::string getOutput() const;
};

} // namespace CHTLJS

#endif // CHTLJS_GENERATOR_H