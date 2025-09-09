#pragma once

#include "CHTL/CHTLNode/BaseNode.h"
#include "CHTL/CHTLNode/ScriptNode.h"
#include <string>
#include <memory>
#include <sstream>

namespace CHTL {

class CHTLJSGenerator {
public:
    CHTLJSGenerator();
    std::string generate(std::shared_ptr<BaseNode> ast);
    
private:
    std::string generateNode(std::shared_ptr<BaseNode> node, int indent = 0);
    std::string generateScriptNode(std::shared_ptr<ScriptNode> scriptNode, int indent);
    std::string generateFileLoader(std::shared_ptr<ScriptNode> fileLoaderNode, int indent);
    std::string generateListen(std::shared_ptr<ScriptNode> listenNode, int indent);
    std::string generateDelegate(std::shared_ptr<ScriptNode> delegateNode, int indent);
    std::string generateAnimate(std::shared_ptr<ScriptNode> animateNode, int indent);
    std::string generateVir(std::shared_ptr<ScriptNode> virNode, int indent);
    std::string generateRouter(std::shared_ptr<ScriptNode> routerNode, int indent);
    std::string generateUtil(std::shared_ptr<ScriptNode> utilNode, int indent);
    std::string generateSelector(std::shared_ptr<ScriptNode> selectorNode, int indent);
    std::string generateExpression(std::shared_ptr<BaseNode> exprNode, int indent);
    std::string generateFunctionCall(std::shared_ptr<BaseNode> funcNode, int indent);
    std::string generateObjectLiteral(std::shared_ptr<BaseNode> objNode, int indent);
    std::string generateArrayLiteral(std::shared_ptr<BaseNode> arrNode, int indent);
    std::string generateConditionalExpression(std::shared_ptr<BaseNode> condNode, int indent);
    std::string generateBinaryExpression(std::shared_ptr<BaseNode> binNode, int indent);
    std::string generateUnaryExpression(std::shared_ptr<BaseNode> unaryNode, int indent);
    std::string generateProperty(std::shared_ptr<BaseNode> propNode, int indent);
    
    // 辅助方法
    std::string getIndent(int indent);
    std::string processSelectors(const std::string& content);
    std::string processArrows(const std::string& content);
    std::string generateSelectorCode(const std::string& selector);
    std::string generateEventHandlers(const std::map<std::string, std::string>& handlers);
    std::string generateAnimationConfig(const std::map<std::string, std::string>& config);
    std::string generateRouteConfig(const std::map<std::string, std::string>& routes);
    std::string generateVirtualObjectConfig(const std::map<std::string, std::string>& config);
    
    // 代码生成状态
    std::stringstream output;
    int currentIndent;
};

} // namespace CHTL