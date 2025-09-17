#ifndef CHTLJSFUNCTIONNODE_H
#define CHTLJSFUNCTIONNODE_H

#include "CHTLJSBaseNode.h"
#include <map>

namespace CHTLJS {

/**
 * CHTL JS函数节点
 * 表示CHTL JS中的各种函数调用
 */
class CHTLJSFunctionNode : public CHTLJSBaseNode {
public:
    CHTLJSFunctionNode(NodeType type, const std::string& functionName);
    
    void setFunctionName(const std::string& name);
    const std::string& getFunctionName() const;
    
    void addParameter(const std::string& key, const std::string& value);
    void addParameter(const std::string& key, std::shared_ptr<CHTLJSBaseNode> value);
    const std::map<std::string, std::string>& getStringParameters() const;
    const std::map<std::string, std::shared_ptr<CHTLJSBaseNode>>& getNodeParameters() const;
    
    std::string generateCode() const override;
    std::string toString() const override;

private:
    std::string function_name_;
    std::map<std::string, std::string> string_parameters_;
    std::map<std::string, std::shared_ptr<CHTLJSBaseNode>> node_parameters_;
};

/**
 * ScriptLoader节点
 */
class ScriptLoaderNode : public CHTLJSFunctionNode {
public:
    ScriptLoaderNode();
    std::string generateCode() const override;
};

/**
 * Listen节点
 */
class ListenNode : public CHTLJSFunctionNode {
public:
    ListenNode();
    std::string generateCode() const override;
};

/**
 * Animate节点
 */
class AnimateNode : public CHTLJSFunctionNode {
public:
    AnimateNode();
    std::string generateCode() const override;
};

/**
 * Router节点
 */
class RouterNode : public CHTLJSFunctionNode {
public:
    RouterNode();
    std::string generateCode() const override;
};

/**
 * Vir节点
 */
class VirNode : public CHTLJSFunctionNode {
public:
    VirNode();
    std::string generateCode() const override;
};

/**
 * iNeverAway节点
 */
class INeverAwayNode : public CHTLJSFunctionNode {
public:
    INeverAwayNode();
    std::string generateCode() const override;
};

/**
 * UtilThen节点
 */
class UtilThenNode : public CHTLJSFunctionNode {
public:
    UtilThenNode();
    std::string generateCode() const override;
};

/**
 * PrintMylove节点
 */
class PrintMyloveNode : public CHTLJSFunctionNode {
public:
    PrintMyloveNode();
    std::string generateCode() const override;
};

} // namespace CHTLJS

#endif // CHTLJSFUNCTIONNODE_H