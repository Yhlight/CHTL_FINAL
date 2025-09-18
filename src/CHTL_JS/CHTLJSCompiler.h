#ifndef CHTL_JS_COMPILER_H
#define CHTL_JS_COMPILER_H

#include "CHTLJSContext.h"
#include <string>
#include <vector>
#include <memory>
#include <functional>

namespace CHTL {

class CHTLJSCompiler {
public:
    CHTLJSCompiler();
    ~CHTLJSCompiler() = default;
    
    // 主要接口
    std::string compile(const std::string& source);
    std::string compileVir(const std::string& content);
    std::string compileListen(const std::string& content);
    std::string compileAnimate(const std::string& content);
    std::string compileRouter(const std::string& content);
    std::string compileScriptLoader(const std::string& content);
    std::string compileUtil(const std::string& content);
    
    // 上下文管理
    void setContext(std::shared_ptr<CHTLJSContext> context);
    std::shared_ptr<CHTLJSContext> getContext() const { return context_; }
    
    // 错误处理
    void setErrorHandler(std::function<void(const std::string&, size_t, size_t)> handler);
    
    // 配置
    void setEnableMinification(bool enable) { enableMinification_ = enable; }
    void setEnableSourceMap(bool enable) { enableSourceMap_ = enable; }
    
private:
    std::shared_ptr<CHTLJSContext> context_;
    std::function<void(const std::string&, size_t, size_t)> errorHandler_;
    bool enableMinification_;
    bool enableSourceMap_;
    
    // 编译辅助方法
    std::string compileExpression(const std::string& expression);
    std::string compileFunction(const std::string& function);
    std::string compileObject(const std::string& object);
    std::string compileArray(const std::string& array);
    std::string compileConditional(const std::string& conditional);
    std::string compileLoop(const std::string& loop);
    
    // 语法转换
    std::string convertCHTLJSToJS(const std::string& chtljs);
    std::string convertEnhancedSelector(const std::string& selector);
    std::string convertEnhancedListener(const std::string& listener);
    std::string convertEventBinding(const std::string& binding);
    std::string convertEventDelegation(const std::string& delegation);
    std::string convertAnimation(const std::string& animation);
    std::string convertVirtualObject(const std::string& virtualObject);
    std::string convertRouter(const std::string& router);
    
    // 占位符处理
    std::string processPlaceholders(const std::string& content);
    std::string replacePlaceholders(const std::string& content, const std::unordered_map<std::string, std::string>& placeholders);
    
    // 代码优化
    std::string optimizeCode(const std::string& code);
    std::string minifyCode(const std::string& code);
    
    // 错误报告
    void reportError(const std::string& message, size_t line = 0, size_t column = 0);
    
    // 辅助方法
    std::string extractBlockContent(const std::string& content, size_t start, size_t& end);
    std::string extractFunctionName(const std::string& function);
    std::string extractFunctionParameters(const std::string& function);
    std::string extractFunctionBody(const std::string& function);
    std::string extractObjectProperties(const std::string& object);
    std::string extractArrayElements(const std::string& array);
    
    // 语法验证
    bool isValidCHTLJSSyntax(const std::string& content);
    bool isValidFunction(const std::string& function);
    bool isValidObject(const std::string& object);
    bool isValidArray(const std::string& array);
    bool isValidExpression(const std::string& expression);
    
    // 代码生成
    std::string generateFunctionWrapper(const std::string& functionName, const std::string& parameters, const std::string& body);
    std::string generateObjectWrapper(const std::string& objectName, const std::string& properties);
    std::string generateArrayWrapper(const std::string& arrayName, const std::string& elements);
    std::string generateEventWrapper(const std::string& eventName, const std::string& handler);
    std::string generateAnimationWrapper(const std::string& animationName, const std::string& properties);
    std::string generateRouterWrapper(const std::string& routerName, const std::string& routes);
};

} // namespace CHTL

#endif // CHTL_JS_COMPILER_H