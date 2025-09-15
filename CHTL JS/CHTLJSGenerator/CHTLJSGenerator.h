#ifndef CHTLJSGENERATOR_H
#define CHTLJSGENERATOR_H

#include "../CHTLJSParser/ASTNode.h"
#include "../CHTLJSContext/CHTLJSContext.h"
#include <string>
#include <memory>

namespace CHTLJS {

/**
 * CHTL JS代码生成器
 * 将AST转换为JavaScript代码
 */
class CHTLJSGenerator {
public:
    CHTLJSGenerator();
    ~CHTLJSGenerator();

    // 代码生成
    std::string generate(const std::shared_ptr<ProgramNode>& program);
    std::string generate(const std::shared_ptr<ASTNode>& node);
    
    // 上下文管理
    void setContext(std::shared_ptr<CHTLJSContext> context);
    std::shared_ptr<CHTLJSContext> getContext() const;

private:
    std::shared_ptr<CHTLJSContext> context;
    
    // 节点代码生成
    std::string generateProgram(const std::shared_ptr<ProgramNode>& node);
    std::string generateIdentifier(const std::shared_ptr<IdentifierNode>& node);
    std::string generateLiteral(const std::shared_ptr<LiteralNode>& node);
    std::string generateObjectLiteral(const std::shared_ptr<ObjectLiteralNode>& node);
    std::string generateFunctionCall(const std::shared_ptr<FunctionCallNode>& node);
    std::string generateVirtualObject(const std::shared_ptr<VirtualObjectNode>& node);
    std::string generateListen(const std::shared_ptr<ListenNode>& node);
    std::string generateAnimate(const std::shared_ptr<AnimateNode>& node);
    std::string generateRouter(const std::shared_ptr<RouterNode>& node);
    std::string generateScriptLoader(const std::shared_ptr<ScriptLoaderNode>& node);
    std::string generateDelegate(const std::shared_ptr<DelegateNode>& node);
    std::string generateINeverAway(const std::shared_ptr<INeverAwayNode>& node);
    std::string generateUtilExpression(const std::shared_ptr<UtilExpressionNode>& node);
    std::string generateSelector(const std::shared_ptr<SelectorNode>& node);
    std::string generateEventBinding(const std::shared_ptr<EventBindingNode>& node);
    std::string generatePropertyAccess(const std::shared_ptr<PropertyAccessNode>& node);
    
    // 辅助方法
    std::string generateSelectorCode(const std::string& selector);
    std::string generateEventCode(const std::string& eventType, const std::string& handler);
    std::string generateAnimationCode(const std::shared_ptr<ObjectLiteralNode>& properties);
    std::string generateRouterCode(const std::shared_ptr<ObjectLiteralNode>& properties);
    std::string generateScriptLoaderCode(const std::shared_ptr<ObjectLiteralNode>& properties);
    std::string generateDelegateCode(const std::shared_ptr<ObjectLiteralNode>& properties);
    std::string generateINeverAwayCode(const std::shared_ptr<ObjectLiteralNode>& functions);
    std::string generateUtilCode(const std::shared_ptr<ASTNode>& condition,
                               const std::shared_ptr<ASTNode>& changeBlock,
                               const std::shared_ptr<ASTNode>& thenBlock);
    
    // 工具方法
    std::string escapeString(const std::string& str);
    std::string indent(int level);
    bool needsQuotes(const std::string& str);
};

} // namespace CHTLJS

#endif // CHTLJSGENERATOR_H