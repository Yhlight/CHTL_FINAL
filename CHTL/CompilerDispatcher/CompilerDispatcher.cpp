#include "CompilerDispatcher.h"
#include "CHTL/CHTLParser/CHTLParser.h"
#include "CHTL/CHTLGenerator/CHTLGenerator.h"
#include <fstream>
#include <sstream>
#include <iostream>

namespace CHTL {

CompilerDispatcher::CompilerDispatcher() 
    : m_scanner(std::make_unique<UnifiedScanner>()),
      m_defaultStruct(false), m_inlineMode(false), 
      m_inlineCSS(false), m_inlineJS(false) {
}

CompilerDispatcher::~CompilerDispatcher() {
}

std::string CompilerDispatcher::compile(const std::string& inputFile) {
    try {
        // 读取输入文件
        std::string code = readFile(inputFile);
        
        // 编译代码
        return compileCode(code);
        
    } catch (const std::exception& e) {
        throw std::runtime_error("编译文件失败: " + std::string(e.what()));
    }
}

std::string CompilerDispatcher::compileCode(const std::string& code) {
    try {
        // 使用统一扫描器分离代码片段
        auto scanResult = m_scanner->scan(code);
        
        // 处理编译选项
        std::string processedCode = processCompilationOptions(code);
        
        // 这里需要调用各个编译器处理不同类型的代码片段
        // 目前先返回一个简单的HTML结构
        
        std::string html = "<!DOCTYPE html>\n<html>\n<head>\n";
        html += "<meta charset=\"UTF-8\">\n";
        html += "<title>CHTL Generated</title>\n";
        
        // 处理CSS
        std::string css = "";
        for (const auto& fragment : scanResult.fragments) {
            if (fragment.type == UnifiedScanner::CodeType::CSS) {
                css += fragment.content + "\n";
            }
        }
        
        if (!css.empty()) {
            if (m_inlineCSS || m_inlineMode) {
                html += "<style>\n" + css + "\n</style>\n";
            } else {
                html += "<link rel=\"stylesheet\" href=\"style.css\">\n";
            }
        }
        
        html += "</head>\n<body>\n";
        
        // 处理HTML内容
        std::string bodyContent = "";
        for (const auto& fragment : scanResult.fragments) {
            if (fragment.type == UnifiedScanner::CodeType::CHTL) {
                // 使用CHTL编译器处理
                CHTLParser parser;
                CHTLGenerator generator;
                generator.setDefaultStructure(m_defaultStruct);
                generator.setInlineCSS(m_inlineCSS);
                generator.setInlineJavaScript(m_inlineJS);
                
                auto ast = parser.parse(fragment.content);
                bodyContent += generator.generateHTML(ast);
            }
        }
        
        html += bodyContent;
        
        // 处理JavaScript
        std::string js = "";
        for (const auto& fragment : scanResult.fragments) {
            if (fragment.type == UnifiedScanner::CodeType::JS || 
                fragment.type == UnifiedScanner::CodeType::CHTL_JS) {
                js += fragment.content + "\n";
            }
        }
        
        if (!js.empty()) {
            if (m_inlineJS || m_inlineMode) {
                html += "<script>\n" + js + "\n</script>\n";
            } else {
                html += "<script src=\"script.js\"></script>\n";
            }
        }
        
        html += "</body>\n</html>";
        
        // 如果需要默认结构，生成默认HTML结构
        if (m_defaultStruct) {
            html = generateDefaultStructure(html);
        }
        
        // 输出到文件
        if (!m_outputFile.empty()) {
            writeFile(m_outputFile, html);
        }
        
        return html;
        
    } catch (const std::exception& e) {
        throw std::runtime_error("编译代码失败: " + std::string(e.what()));
    }
}

void CompilerDispatcher::setDefaultStruct(bool enable) {
    m_defaultStruct = enable;
}

void CompilerDispatcher::setInlineMode(bool enable) {
    m_inlineMode = enable;
}

void CompilerDispatcher::setInlineCSS(bool enable) {
    m_inlineCSS = enable;
}

void CompilerDispatcher::setInlineJS(bool enable) {
    m_inlineJS = enable;
}

void CompilerDispatcher::setOutputFile(const std::string& filename) {
    m_outputFile = filename;
}

std::string CompilerDispatcher::readFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("无法打开文件: " + filename);
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

void CompilerDispatcher::writeFile(const std::string& filename, const std::string& content) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("无法写入文件: " + filename);
    }
    
    file << content;
    file.close();
}

std::string CompilerDispatcher::processCompilationOptions(const std::string& code) {
    // 处理编译选项，如use语句等
    std::string processedCode = code;
    
    // 这里可以添加各种编译选项的处理逻辑
    // 例如处理use语句、配置组等
    
    return processedCode;
}

std::string CompilerDispatcher::generateDefaultStructure(const std::string& content) {
    // 生成默认HTML结构
    std::string html = "<!DOCTYPE html>\n";
    html += "<html lang=\"zh-CN\">\n";
    html += "<head>\n";
    html += "    <meta charset=\"UTF-8\">\n";
    html += "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n";
    html += "    <title>CHTL Generated Page</title>\n";
    html += "</head>\n";
    html += "<body>\n";
    html += content;
    html += "\n</body>\n";
    html += "</html>";
    
    return html;
}

std::string CompilerDispatcher::inlineResources(const std::string& html, 
                                               const std::string& css, 
                                               const std::string& js) {
    std::string result = html;
    
    // 内联CSS
    if (m_inlineCSS || m_inlineMode) {
        size_t headPos = result.find("</head>");
        if (headPos != std::string::npos) {
            std::string cssTag = "<style>\n" + css + "\n</style>\n";
            result.insert(headPos, cssTag);
        }
    }
    
    // 内联JS
    if (m_inlineJS || m_inlineMode) {
        size_t bodyPos = result.find("</body>");
        if (bodyPos != std::string::npos) {
            std::string jsTag = "<script>\n" + js + "\n</script>\n";
            result.insert(bodyPos, jsTag);
        }
    }
    
    return result;
}

} // namespace CHTL