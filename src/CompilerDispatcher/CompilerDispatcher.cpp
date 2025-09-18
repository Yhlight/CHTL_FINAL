#include "CompilerDispatcher.h"
#include "../CHTL/CHTLLexer.h"
#include "../CHTL/CHTLParser.h"
#include "../CHTL/CHTLGenerator.h"
#include "../Scanner/UnifiedScanner.h"
#include "../CodeMerger/CodeMerger.h"

namespace CHTL {

CompilerDispatcher::CompilerDispatcher() 
    : inlineCSS_(false), inlineJS_(false), minify_(false), debugMode_(false) {
}

CompileResult CompilerDispatcher::compile(const std::string& source) {
    CompileResult result;
    
    try {
        // 使用统一扫描器分离代码片段
        UnifiedScanner scanner;
        ScanResult scanResult = scanner.scan(source);
        
        // 词法分析
        CHTLLexer lexer(source);
        TokenList tokens = lexer.tokenize();
        
        // 语法分析
        CHTLParser parser(tokens);
        NodePtr root = parser.parse();
        
        if (!root) {
            result.errors.push_back("Failed to parse source");
            return result;
        }
        
        // 代码生成
        CHTLGenerator generator;
        GenerationOptions options;
        options.inlineCSS = inlineCSS_;
        options.inlineJS = inlineJS_;
        options.minify = minify_;
        options.debugMode = debugMode_;
        
        GenerationResult genResult = generator.generate(root, options);
        
        if (genResult.hasErrors()) {
            result.errors.insert(result.errors.end(), genResult.errors.begin(), genResult.errors.end());
        }
        
        if (genResult.hasWarnings()) {
            result.warnings.insert(result.warnings.end(), genResult.warnings.begin(), genResult.warnings.end());
        }
        
        result.html = genResult.html;
        result.css = genResult.css;
        result.js = genResult.js;
        
    } catch (const std::exception& e) {
        result.errors.push_back("Compilation error: " + std::string(e.what()));
    }
    
    return result;
}

CompileResult CompilerDispatcher::compileFile(const std::string& filename) {
    CompileResult result;
    
    try {
        std::ifstream file(filename);
        if (!file.is_open()) {
            result.errors.push_back("Cannot open file: " + filename);
            return result;
        }
        
        std::string source((std::istreambuf_iterator<char>(file)),
                          std::istreambuf_iterator<char>());
        file.close();
        
        return compile(source);
        
    } catch (const std::exception& e) {
        result.errors.push_back("File compilation error: " + std::string(e.what()));
    }
    
    return result;
}

void CompilerDispatcher::setErrorHandler(std::function<void(const std::string&)> handler) {
    errorHandler_ = handler;
}

void CompilerDispatcher::reportError(const std::string& message) {
    if (errorHandler_) {
        errorHandler_(message);
    }
}

} // namespace CHTL