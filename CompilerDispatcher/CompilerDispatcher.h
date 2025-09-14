#pragma once

#include <string>
#include <vector>
#include <memory>
#include "Scanner/UnifiedScanner.h"

namespace CHTL {

class CHTLCompiler;
class CHTLJSCompiler;
class CSSCompiler;
class JSCompiler;
class CodeMerger;

class CompilerDispatcher {
public:
    CompilerDispatcher();
    ~CompilerDispatcher();

    // 主要编译函数
    void compile(const std::string& inputFile, const std::string& outputFile);
    
    // 配置
    void setDebugMode(bool enabled);
    void setDefaultStruct(bool enabled);
    
    // 模块路径配置
    void addModulePath(const std::string& path);
    void setOfficialModulePath(const std::string& path);

private:
    // 核心组件
    std::unique_ptr<UnifiedScanner> scanner_;
    std::unique_ptr<CHTLCompiler> chtlCompiler_;
    std::unique_ptr<CHTLJSCompiler> chtlJSCompiler_;
    std::unique_ptr<CSSCompiler> cssCompiler_;
    std::unique_ptr<JSCompiler> jsCompiler_;
    std::unique_ptr<CodeMerger> codeMerger_;
    
    // 配置
    bool debugMode_;
    bool defaultStruct_;
    std::vector<std::string> modulePaths_;
    std::string officialModulePath_;
    
    // 辅助函数
    std::string readFile(const std::string& filePath);
    void writeFile(const std::string& filePath, const std::string& content);
    std::string getFileExtension(const std::string& filePath);
    bool fileExists(const std::string& filePath);
    
    // 编译流程
    void processCHTLFragments(const std::vector<CodeFragment>& fragments);
    void processCHTLJSFragments(const std::vector<CodeFragment>& fragments);
    void processCSSFragments(const std::vector<CodeFragment>& fragments);
    void processJSFragments(const std::vector<CodeFragment>& fragments);
};

} // namespace CHTL