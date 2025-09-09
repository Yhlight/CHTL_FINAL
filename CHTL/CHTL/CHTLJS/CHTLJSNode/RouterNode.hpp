#pragma once

#include "CHTLJSBaseNode.hpp"
#include <string>
#include <vector>
#include <map>

namespace CHTL {

/**
 * @brief 路由节点
 * 
 * 表示 router {} 语法
 */
class RouterNode : public CHTLJSBaseNode {
public:
    struct Route {
        std::string url;
        std::string selector;
    };
    
    RouterNode(size_t line = 0, size_t column = 0);
    
    // 设置根路径
    void setRoot(const std::string& root);
    
    // 获取根路径
    const std::string& getRoot() const { return root_; }
    
    // 设置路由模式
    void setMode(const std::string& mode);
    
    // 获取路由模式
    const std::string& getMode() const { return mode_; }
    
    // 添加路由
    void addRoute(const Route& route);
    
    // 添加路由
    void addRoute(const std::string& url, const std::string& selector);
    
    // 获取所有路由
    const std::vector<Route>& getRoutes() const { return routes_; }
    
    // 转换为 JavaScript 代码
    std::string toJavaScript() const override;
    
    // 转换为字符串
    std::string toString() const override;
    
private:
    std::string root_;
    std::string mode_;
    std::vector<Route> routes_;
    
    // 生成路由匹配代码
    std::string generateRouteMatcher() const;
    
    // 生成路由切换代码
    std::string generateRouteSwitcher() const;
};

} // namespace CHTL