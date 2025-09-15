#include "CompilerDispatcher.h"
#include <fstream>
#include <sstream>
#include <iostream>

namespace CHTL {

CompilerDispatcher::CompilerDispatcher() 
    : debugMode(false), optimizationLevel(1), compiling(false) {
    scanner = std::make_unique<UnifiedScanner>();
}

CompilerDispatcher::~CompilerDispatcher() {
}

CompileResult CompilerDispatcher::compile(const std::string& source) {
    CompileResult result;
    compiling = true;
    clearMessages();
    
    try {
        // 使用统一扫描器分离代码片段
        std::vector<CodeFragment> fragments = scanner->scan(source);
        
        if (scanner->hasErrors()) {
            result.errors = scanner->getErrors();
            result.success = false;
            return result;
        }
        
        // 编译各个片段
        result = compileFragments(fragments);
        
        if (result.success) {
            // 合并代码
            result.html = mergeCode(result);
        }
        
    } catch (const std::exception& e) {
        addError("Compilation error: " + std::string(e.what()));
        result.success = false;
    }
    
    compiling = false;
    result.errors = lastErrors;
    result.warnings = lastWarnings;
    
    return result;
}

CompileResult CompilerDispatcher::compileFile(const std::string& filename) {
    CompileResult result;
    
    try {
        std::string source = readFile(filename);
        if (source.empty()) {
            addError("Failed to read file: " + filename);
            result.success = false;
            return result;
        }
        
        result = compile(source);
        
        if (result.success && !outputDirectory.empty()) {
            // 写入输出文件
            std::string baseName = filename.substr(0, filename.find_last_of('.'));
            writeFile(outputDirectory + "/" + baseName + ".html", result.html);
            writeFile(outputDirectory + "/" + baseName + ".css", result.css);
            writeFile(outputDirectory + "/" + baseName + ".js", result.javascript);
        }
        
    } catch (const std::exception& e) {
        addError("File compilation error: " + std::string(e.what()));
        result.success = false;
    }
    
    return result;
}

void CompilerDispatcher::setOutputDirectory(const std::string& directory) {
    outputDirectory = directory;
}

void CompilerDispatcher::setDebugMode(bool enabled) {
    debugMode = enabled;
}

void CompilerDispatcher::setOptimizationLevel(int level) {
    optimizationLevel = level;
}

bool CompilerDispatcher::isCompiling() const {
    return compiling;
}

std::vector<std::string> CompilerDispatcher::getLastErrors() const {
    return lastErrors;
}

std::vector<std::string> CompilerDispatcher::getLastWarnings() const {
    return lastWarnings;
}

CompileResult CompilerDispatcher::compileFragments(const std::vector<CodeFragment>& fragments) {
    CompileResult result;
    result.success = true;
    
    for (const auto& fragment : fragments) {
        try {
            switch (fragment.type) {
                case CodeType::CHTL:
                    result.html += compileCHTL(fragment.content);
                    break;
                case CodeType::CHTL_JS:
                    result.javascript += compileCHTLJS(fragment.content);
                    break;
                case CodeType::CSS:
                    result.css += compileCSS(fragment.content);
                    break;
                case CodeType::JS:
                    result.javascript += compileJS(fragment.content);
                    break;
                default:
                    addWarning("Unknown code type in fragment");
                    break;
            }
        } catch (const std::exception& e) {
            addError("Fragment compilation error: " + std::string(e.what()));
            result.success = false;
        }
    }
    
    return result;
}

std::string CompilerDispatcher::compileCHTL(const std::string& code) {
    // TODO: 实现CHTL编译器
    // 这里应该调用CHTL编译器的实际实现
    if (debugMode) {
        std::cout << "Compiling CHTL code: " << code.substr(0, 50) << "..." << std::endl;
    }
    
    // 临时实现：直接返回HTML
    return "<!-- CHTL compiled code -->\n" + code + "\n";
}

std::string CompilerDispatcher::compileCHTLJS(const std::string& code) {
    // TODO: 实现CHTL JS编译器
    // 这里应该调用CHTL JS编译器的实际实现
    if (debugMode) {
        std::cout << "Compiling CHTL JS code: " << code.substr(0, 50) << "..." << std::endl;
    }
    
    // 临时实现：直接返回JavaScript
    return "// CHTL JS compiled code\n" + code + "\n";
}

std::string CompilerDispatcher::compileCSS(const std::string& code) {
    // TODO: 实现CSS编译器
    // 这里应该调用CSS编译器的实际实现
    if (debugMode) {
        std::cout << "Compiling CSS code: " << code.substr(0, 50) << "..." << std::endl;
    }
    
    // 临时实现：直接返回CSS
    return "/* CSS compiled code */\n" + code + "\n";
}

std::string CompilerDispatcher::compileJS(const std::string& code) {
    // TODO: 实现JavaScript编译器
    // 这里应该调用JavaScript编译器的实际实现
    if (debugMode) {
        std::cout << "Compiling JS code: " << code.substr(0, 50) << "..." << std::endl;
    }
    
    // 临时实现：直接返回JavaScript
    return "// JavaScript compiled code\n" + code + "\n";
}

std::string CompilerDispatcher::mergeCode(const CompileResult& result) {
    std::ostringstream html;
    
    html << "<!DOCTYPE html>\n";
    html << "<html>\n";
    html << "<head>\n";
    html << "  <meta charset=\"UTF-8\">\n";
    html << "  <title>CHTL Generated Page</title>\n";
    
    if (!result.css.empty()) {
        html << "  <style>\n";
        html << result.css;
        html << "  </style>\n";
    }
    
    html << "</head>\n";
    html << "<body>\n";
    html << result.html;
    
    if (!result.javascript.empty()) {
        html << "  <script>\n";
        html << result.javascript;
        html << "  </script>\n";
    }
    
    html << "</body>\n";
    html << "</html>\n";
    
    return html.str();
}

void CompilerDispatcher::addError(const std::string& error) {
    lastErrors.push_back(error);
    if (debugMode) {
        std::cerr << "Error: " << error << std::endl;
    }
}

void CompilerDispatcher::addWarning(const std::string& warning) {
    lastWarnings.push_back(warning);
    if (debugMode) {
        std::cout << "Warning: " << warning << std::endl;
    }
}

void CompilerDispatcher::clearMessages() {
    lastErrors.clear();
    lastWarnings.clear();
}

bool CompilerDispatcher::writeFile(const std::string& filename, const std::string& content) {
    try {
        std::ofstream file(filename);
        if (!file.is_open()) {
            addError("Failed to open file for writing: " + filename);
            return false;
        }
        
        file << content;
        file.close();
        return true;
    } catch (const std::exception& e) {
        addError("Error writing file: " + std::string(e.what()));
        return false;
    }
}

std::string CompilerDispatcher::readFile(const std::string& filename) {
    try {
        std::ifstream file(filename);
        if (!file.is_open()) {
            addError("Failed to open file for reading: " + filename);
            return "";
        }
        
        std::ostringstream content;
        content << file.rdbuf();
        file.close();
        return content.str();
    } catch (const std::exception& e) {
        addError("Error reading file: " + std::string(e.what()));
        return "";
    }
}

} // namespace CHTL