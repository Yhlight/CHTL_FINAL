#ifndef COMPILER_DISPATCHER_H
#define COMPILER_DISPATCHER_H

#include <string>
#include <vector>
#include <memory>
#include "CHTLNode/BaseNode.h"

namespace CHTL {

struct CompileResult {
    std::string html;
    std::string css;
    std::string js;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    
    bool hasErrors() const { return !errors.empty(); }
    bool hasWarnings() const { return !warnings.empty(); }
};

class CompilerDispatcher {
public:
    CompilerDispatcher();
    ~CompilerDispatcher() = default;
    
    // 编译
    CompileResult compile(const std::string& source);
    CompileResult compileFile(const std::string& filename);
    
    // 配置
    void setInlineCSS(bool inlineCSS) { inlineCSS_ = inlineCSS; }
    void setInlineJS(bool inlineJS) { inlineJS_ = inlineJS; }
    void setMinify(bool minify) { minify_ = minify; }
    void setDebugMode(bool debug) { debugMode_ = debug; }
    
    // 错误处理
    void setErrorHandler(std::function<void(const std::string&)> handler);
    
private:
    bool inlineCSS_;
    bool inlineJS_;
    bool minify_;
    bool debugMode_;
    std::function<void(const std::string&)> errorHandler_;
    
    void reportError(const std::string& message);
};

} // namespace CHTL

#endif // COMPILER_DISPATCHER_H