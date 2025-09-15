#ifndef CHTLJS_SPECIALNODE_H
#define CHTLJS_SPECIALNODE_H

#include "CHTLJSBaseNode.h"

namespace CHTLJS {

/**
 * ScriptLoader节点 - CHTL JS模块加载器
 */
class ScriptLoaderNode : public CHTLJSBaseNode {
public:
    ScriptLoaderNode();
    ~ScriptLoaderNode();
    
    void setModuleName(const std::string& name);
    const std::string& getModuleName() const;
    
    void setModulePath(const std::string& path);
    const std::string& getModulePath() const;
    
    void setCallback(std::shared_ptr<CHTLJSBaseNode> callback);
    std::shared_ptr<CHTLJSBaseNode> getCallback() const;
    
    std::string generateCode() const override;
    std::string toString() const override;

private:
    std::string module_name_;
    std::string module_path_;
    std::shared_ptr<CHTLJSBaseNode> callback_;
};

/**
 * Listen节点 - CHTL JS事件监听
 */
class ListenNode : public CHTLJSBaseNode {
public:
    ListenNode();
    ~ListenNode();
    
    void setSelector(const std::string& selector);
    const std::string& getSelector() const;
    
    void addEvent(const std::string& event, std::shared_ptr<CHTLJSBaseNode> handler);
    const std::map<std::string, std::shared_ptr<CHTLJSBaseNode>>& getEvents() const;
    
    std::string generateCode() const override;
    std::string toString() const override;

private:
    std::string selector_;
    std::map<std::string, std::shared_ptr<CHTLJSBaseNode>> events_;
};

/**
 * Animate节点 - CHTL JS动画系统
 */
class AnimateNode : public CHTLJSBaseNode {
public:
    AnimateNode();
    ~AnimateNode();
    
    void setTarget(const std::string& target);
    const std::string& getTarget() const;
    
    void setDuration(const std::string& duration);
    const std::string& getDuration() const;
    
    void setEasing(const std::string& easing);
    const std::string& getEasing() const;
    
    void addProperty(const std::string& property, const std::string& value);
    const std::map<std::string, std::string>& getProperties() const;
    
    void setCallback(std::shared_ptr<CHTLJSBaseNode> callback);
    std::shared_ptr<CHTLJSBaseNode> getCallback() const;
    
    std::string generateCode() const override;
    std::string toString() const override;

private:
    std::string target_;
    std::string duration_;
    std::string easing_;
    std::map<std::string, std::string> properties_;
    std::shared_ptr<CHTLJSBaseNode> callback_;
};

/**
 * Router节点 - CHTL JS路由系统
 */
class RouterNode : public CHTLJSBaseNode {
public:
    RouterNode();
    ~RouterNode();
    
    void addRoute(const std::string& path, std::shared_ptr<CHTLJSBaseNode> handler);
    const std::map<std::string, std::shared_ptr<CHTLJSBaseNode>>& getRoutes() const;
    
    void setDefaultRoute(std::shared_ptr<CHTLJSBaseNode> handler);
    std::shared_ptr<CHTLJSBaseNode> getDefaultRoute() const;
    
    std::string generateCode() const override;
    std::string toString() const override;

private:
    std::map<std::string, std::shared_ptr<CHTLJSBaseNode>> routes_;
    std::shared_ptr<CHTLJSBaseNode> default_route_;
};

/**
 * Vir节点 - CHTL JS虚拟对象
 */
class VirNode : public CHTLJSBaseNode {
public:
    VirNode();
    ~VirNode();
    
    void setObjectName(const std::string& name);
    const std::string& getObjectName() const;
    
    void addProperty(const std::string& name, std::shared_ptr<CHTLJSBaseNode> value);
    const std::map<std::string, std::shared_ptr<CHTLJSBaseNode>>& getProperties() const;
    
    void addMethod(const std::string& name, std::shared_ptr<CHTLJSBaseNode> method);
    const std::map<std::string, std::shared_ptr<CHTLJSBaseNode>>& getMethods() const;
    
    std::string generateCode() const override;
    std::string toString() const override;

private:
    std::string object_name_;
    std::map<std::string, std::shared_ptr<CHTLJSBaseNode>> properties_;
    std::map<std::string, std::shared_ptr<CHTLJSBaseNode>> methods_;
};

/**
 * UtilThen节点 - CHTL JS工具链式操作
 */
class UtilThenNode : public CHTLJSBaseNode {
public:
    UtilThenNode();
    ~UtilThenNode();
    
    void setTarget(std::shared_ptr<CHTLJSBaseNode> target);
    std::shared_ptr<CHTLJSBaseNode> getTarget() const;
    
    void addOperation(const std::string& operation, std::shared_ptr<CHTLJSBaseNode> value);
    const std::map<std::string, std::shared_ptr<CHTLJSBaseNode>>& getOperations() const;
    
    std::string generateCode() const override;
    std::string toString() const override;

private:
    std::shared_ptr<CHTLJSBaseNode> target_;
    std::map<std::string, std::shared_ptr<CHTLJSBaseNode>> operations_;
};

/**
 * PrintMylove节点 - CHTL JS图像转ASCII
 */
class PrintMyloveNode : public CHTLJSBaseNode {
public:
    PrintMyloveNode();
    ~PrintMyloveNode();
    
    void setImagePath(const std::string& path);
    const std::string& getImagePath() const;
    
    void setWidth(int width);
    int getWidth() const;
    
    void setHeight(int height);
    int getHeight() const;
    
    void setCharSet(const std::string& charset);
    const std::string& getCharSet() const;
    
    std::string generateCode() const override;
    std::string toString() const override;

private:
    std::string image_path_;
    int width_;
    int height_;
    std::string charset_;
};

/**
 * INeverAway节点 - CHTL JS函数重载
 */
class INeverAwayNode : public CHTLJSBaseNode {
public:
    INeverAwayNode();
    ~INeverAwayNode();
    
    void setFunctionName(const std::string& name);
    const std::string& getFunctionName() const;
    
    void addOverload(const std::vector<std::string>& parameters, std::shared_ptr<CHTLJSBaseNode> body);
    const std::vector<std::pair<std::vector<std::string>, std::shared_ptr<CHTLJSBaseNode>>>& getOverloads() const;
    
    std::string generateCode() const override;
    std::string toString() const override;

private:
    std::string function_name_;
    std::vector<std::pair<std::vector<std::string>, std::shared_ptr<CHTLJSBaseNode>>> overloads_;
};

/**
 * ResponsiveGet节点 - CHTL JS响应式获取
 */
class ResponsiveGetNode : public CHTLJSBaseNode {
public:
    ResponsiveGetNode(const std::string& variableName);
    ~ResponsiveGetNode();
    
    void setVariableName(const std::string& name);
    const std::string& getVariableName() const;
    
    std::string generateCode() const override;
    std::string toString() const override;

private:
    std::string variable_name_;
};

/**
 * ResponsiveSet节点 - CHTL JS响应式设置
 */
class ResponsiveSetNode : public CHTLJSBaseNode {
public:
    ResponsiveSetNode(const std::string& variableName, std::shared_ptr<CHTLJSBaseNode> value);
    ~ResponsiveSetNode();
    
    void setVariableName(const std::string& name);
    const std::string& getVariableName() const;
    
    void setValue(std::shared_ptr<CHTLJSBaseNode> value);
    std::shared_ptr<CHTLJSBaseNode> getValue() const;
    
    std::string generateCode() const override;
    std::string toString() const override;

private:
    std::string variable_name_;
    std::shared_ptr<CHTLJSBaseNode> value_;
};

/**
 * CHTLSelector节点 - CHTL选择器
 */
class CHTLSelectorNode : public CHTLJSBaseNode {
public:
    CHTLSelectorNode(const std::string& selector);
    ~CHTLSelectorNode();
    
    void setSelector(const std::string& selector);
    const std::string& getSelector() const;
    
    std::string generateCode() const override;
    std::string toString() const override;

private:
    std::string selector_;
};

/**
 * CHTLResponsive节点 - CHTL响应式值
 */
class CHTLResponsiveNode : public CHTLJSBaseNode {
public:
    CHTLResponsiveNode(const std::string& variableName);
    ~CHTLResponsiveNode();
    
    void setVariableName(const std::string& name);
    const std::string& getVariableName() const;
    
    std::string generateCode() const override;
    std::string toString() const override;

private:
    std::string variable_name_;
};

} // namespace CHTLJS

#endif // CHTLJS_SPECIALNODE_H