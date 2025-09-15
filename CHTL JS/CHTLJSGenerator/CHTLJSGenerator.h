#ifndef CHTLJS_GENERATOR_H
#define CHTLJS_GENERATOR_H

#include "../CHTLJSNode/CHTLJSBaseNode.h"
#include "../CHTLJSNode/CHTLJSSpecialNode.h"
#include "../CHTLJSContext/CHTLJSContext.h"
#include <string>
#include <memory>

namespace CHTLJS {

/**
 * CHTL JS代码生成器
 * 将CHTL JS AST转换为JavaScript代码
 * CHTL JS是独立的编程语言，不是JavaScript
 */
class CHTLJSGenerator {
public:
    CHTLJSGenerator(std::shared_ptr<CHTLJSContext> context);
    ~CHTLJSGenerator();
    
    // 主要生成方法
    std::string generate(std::shared_ptr<CHTLJSBaseNode> ast);
    std::string generateDocument(const std::vector<std::shared_ptr<CHTLJSBaseNode>>& statements);
    
    // 特殊节点生成
    std::string generateScriptLoader(std::shared_ptr<CHTLJSBaseNode> node);
    std::string generateListen(std::shared_ptr<CHTLJSBaseNode> node);
    std::string generateAnimate(std::shared_ptr<CHTLJSBaseNode> node);
    std::string generateRouter(std::shared_ptr<CHTLJSBaseNode> node);
    std::string generateVir(std::shared_ptr<CHTLJSBaseNode> node);
    std::string generateUtilThen(std::shared_ptr<CHTLJSBaseNode> node);
    std::string generatePrintMylove(std::shared_ptr<CHTLJSBaseNode> node);
    std::string generateINeverAway(std::shared_ptr<CHTLJSBaseNode> node);
    std::string generateResponsiveGet(std::shared_ptr<CHTLJSBaseNode> node);
    std::string generateResponsiveSet(std::shared_ptr<CHTLJSBaseNode> node);
    std::string generateCHTLSelector(std::shared_ptr<CHTLJSBaseNode> node);
    std::string generateCHTLResponsive(std::shared_ptr<CHTLJSBaseNode> node);
    
    // 传统节点生成
    std::string generateIdentifier(std::shared_ptr<CHTLJSBaseNode> node);
    std::string generateLiteral(std::shared_ptr<CHTLJSBaseNode> node);
    std::string generateBinaryExpression(std::shared_ptr<CHTLJSBaseNode> node);
    std::string generateUnaryExpression(std::shared_ptr<CHTLJSBaseNode> node);
    std::string generateAssignment(std::shared_ptr<CHTLJSBaseNode> node);
    std::string generateFunctionCall(std::shared_ptr<CHTLJSBaseNode> node);
    std::string generateBlock(std::shared_ptr<CHTLJSBaseNode> node);
    std::string generateIfStatement(std::shared_ptr<CHTLJSBaseNode> node);
    std::string generateWhileStatement(std::shared_ptr<CHTLJSBaseNode> node);
    std::string generateForStatement(std::shared_ptr<CHTLJSBaseNode> node);
    std::string generateReturnStatement(std::shared_ptr<CHTLJSBaseNode> node);
    std::string generateVariableDeclaration(std::shared_ptr<CHTLJSBaseNode> node);
    std::string generateFunctionDeclaration(std::shared_ptr<CHTLJSBaseNode> node);
    
    // 辅助方法
    void setIndentation(int level);
    int getIndentation() const;
    std::string getIndentString() const;
    void increaseIndentation();
    void decreaseIndentation();

private:
    std::shared_ptr<CHTLJSContext> context_;
    int indentation_level_;
    
    // 辅助生成方法
    std::string generateExpression(std::shared_ptr<CHTLJSBaseNode> node);
    std::string generateStatement(std::shared_ptr<CHTLJSBaseNode> node);
    std::string generateBlockContent(const std::vector<std::shared_ptr<CHTLJSBaseNode>>& statements);
    
    // CHTL JS特有功能生成
    std::string generateReactiveSystem();
    std::string generateVirtualObjectSystem();
    std::string generateEventSystem();
    std::string generateAnimationSystem();
    std::string generateRouterSystem();
    std::string generateModuleSystem();
    
    // 工具方法
    std::string escapeString(const std::string& str) const;
    std::string generateUniqueId() const;
    bool isCHTLJSSpecific(std::shared_ptr<CHTLJSBaseNode> node) const;
};

} // namespace CHTLJS

#endif // CHTLJS_GENERATOR_H