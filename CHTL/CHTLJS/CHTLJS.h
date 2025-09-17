#ifndef CHTLJS_H
#define CHTLJS_H

#include "CHTLJSContext/CHTLJSContext.h"
#include "CHTLJSLexer/CHTLJSLexer.h"
#include "CHTLJSParser/CHTLJSParser.h"
#include "CHTLJSGenerator/CHTLJSGenerator.h"
#include <string>
#include <memory>

namespace CHTLJS {

class CHTLJS {
private:
    std::unique_ptr<CHTLJSContext> context;
    std::unique_ptr<CHTLJSLexer> lexer;
    std::unique_ptr<CHTLJSParser> parser;
    std::unique_ptr<CHTLJSGenerator> generator;
    
    bool initialized;
    std::string lastError;
    
    void initialize();
    void cleanup();
    
public:
    CHTLJS();
    ~CHTLJS();
    
    // 主要编译方法
    std::string compile(const std::string& source);
    std::string compileFile(const std::string& filePath);
    
    // 配置方法
    void setDebugMode(bool debug);
    void setMinifyOutput(bool minify);
    void setOutputPath(const std::string& path);
    
    // 状态查询
    bool isInitialized() const;
    std::string getLastError() const;
    bool hasError() const;
    
    // 上下文访问
    CHTLJSContext* getContext() const;
    
    // 工具方法
    void reset();
    void clear();
    
    // 调试方法
    void printContext() const;
    void printTokens(const std::string& source) const;
    void printAST(const std::string& source) const;
};

} // namespace CHTLJS

#endif // CHTLJS_H