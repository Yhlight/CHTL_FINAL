#include "CHTL/CompilerDispatcher/CompilerDispatcher.h"
#include <iostream>
#include <fstream>
#include <sstream>

namespace CHTL {

CompilerDispatcher::CompilerDispatcher(std::shared_ptr<CHTLContext> context)
    : m_context(context)
    , m_scanner(std::make_unique<UnifiedScanner>(context))
    , m_chtlParser(std::make_unique<CHTLParser>(context))
    , m_chtlGenerator(std::make_unique<CHTLGenerator>(context))
    , m_cjmodCompiler(std::make_unique<CJMODCompiler>(context))
    , m_codeMerger(std::make_unique<CodeMerger>(context))
    , m_debugMode(false)
    , m_compilationCount(0)
    , m_successfulCompilations(0)
    , m_failedCompilations(0)
    , m_chtlFragmentsProcessed(0)
    , m_chtlJSFragmentsProcessed(0)
    , m_cssFragmentsProcessed(0)
    , m_jsFragmentsProcessed(0)
{
}

CompilerDispatcher::~CompilerDispatcher() {
}

std::string CompilerDispatcher::compile(const std::string& sourceCode, const std::string& sourceFile) {
    m_compilationCount++;
    
    if (m_debugMode) {
        std::cout << "[CompilerDispatcher] Starting compilation..." << std::endl;
    }
    
    try {
        // 1. 使用统一扫描器分离代码片段
        auto fragments = m_scanner->scan(sourceCode);
        
        if (m_debugMode) {
            std::cout << "[CompilerDispatcher] Found " << fragments.size() << " code fragments" << std::endl;
        }
        
        // 2. 处理每个代码片段
        for (const auto& fragment : fragments) {
            CodeFragmentType type;
            
            // 根据片段类型确定处理方式
            type = fragment.type;
            
            if (m_debugMode) {
                std::cout << "[CompilerDispatcher] Processing fragment: type=" << static_cast<int>(type) 
                          << ", content=\"" << fragment.content.substr(0, 50) << "...\", line=" << fragment.line << std::endl;
            }
            
            // 添加片段到代码合并器
            m_codeMerger->addFragment(type, fragment.content, fragment.line, fragment.column, sourceFile);
        }
        
        // 3. 处理CHTL片段
        auto chtlFragments = m_codeMerger->getFragmentCount(CodeFragmentType::CHTL);
        for (size_t i = 0; i < chtlFragments; ++i) {
            // 获取CHTL片段
            auto fragment = m_codeMerger->getFragment(CodeFragmentType::CHTL, i);
            if (fragment) {
                // 解析CHTL片段
                auto document = m_chtlParser->parse(fragment->content);
                if (document) {
                    // 生成HTML
                    std::string html = m_chtlGenerator->generateHTML(document);
                    if (m_debugMode) {
                        std::cout << "[CompilerDispatcher] Generated HTML from CHTL: " << html.substr(0, 100) << "..." << std::endl;
                    }
                    // 将生成的HTML添加到代码合并器
                    m_codeMerger->addFragment(CodeFragmentType::HTML, html, fragment->line, fragment->column, fragment->sourceFile);
                }
                m_chtlFragmentsProcessed++;
            }
        }
        
        // 4. 处理CHTL JS片段
        auto cjmodFragments = m_codeMerger->getFragmentCount(CodeFragmentType::CHTL_JS);
        for (size_t i = 0; i < cjmodFragments; ++i) {
            // 这里应该从代码合并器中获取CHTL JS片段进行处理
            // 简化实现，直接处理所有CHTL JS片段
            m_chtlJSFragmentsProcessed++;
        }
        
        // 5. 处理CSS片段
        auto cssFragments = m_codeMerger->getFragmentCount(CodeFragmentType::CSS);
        for (size_t i = 0; i < cssFragments; ++i) {
            // 这里应该从代码合并器中获取CSS片段进行处理
            // 简化实现，直接处理所有CSS片段
            m_cssFragmentsProcessed++;
        }
        
        // 6. 处理JavaScript片段
        auto jsFragments = m_codeMerger->getFragmentCount(CodeFragmentType::JS);
        for (size_t i = 0; i < jsFragments; ++i) {
            // 这里应该从代码合并器中获取JavaScript片段进行处理
            // 简化实现，直接处理所有JavaScript片段
            m_jsFragmentsProcessed++;
        }
        
        // 7. 合并所有代码片段
        std::string result = m_codeMerger->merge();
        
        m_successfulCompilations++;
        return result;
        
    } catch (const std::exception& e) {
        addError("Compilation failed: " + std::string(e.what()));
        m_failedCompilations++;
        return "";
    }
}

std::string CompilerDispatcher::compileFile(const std::string& filePath) {
    if (m_debugMode) {
        std::cout << "[CompilerDispatcher] Compiling file: " << filePath << std::endl;
    }
    
    std::ifstream file(filePath);
    if (!file.is_open()) {
        addError("Cannot open file: " + filePath);
        return "";
    }
    
    std::ostringstream buffer;
    buffer << file.rdbuf();
    file.close();
    
    return compile(buffer.str(), filePath);
}

void CompilerDispatcher::reset() {
    m_compilationCount = 0;
    m_successfulCompilations = 0;
    m_failedCompilations = 0;
    m_chtlFragmentsProcessed = 0;
    m_chtlJSFragmentsProcessed = 0;
    m_cssFragmentsProcessed = 0;
    m_jsFragmentsProcessed = 0;
    
    if (m_codeMerger) {
        m_codeMerger->clear();
    }
}

void CompilerDispatcher::setDebugMode(bool debug) {
    m_debugMode = debug;
    
    if (m_scanner) {
        m_scanner->setDebugMode(debug);
    }
    if (m_chtlParser) {
        m_chtlParser->setDebugMode(debug);
    }
    if (m_chtlGenerator) {
        m_chtlGenerator->setDebugMode(debug);
    }
    if (m_cjmodCompiler) {
        m_cjmodCompiler->setDebugMode(debug);
    }
    if (m_codeMerger) {
        m_codeMerger->setDebugMode(debug);
    }
}

bool CompilerDispatcher::isDebugMode() const {
    return m_debugMode;
}

std::string CompilerDispatcher::getStatistics() const {
    std::ostringstream oss;
    oss << "CompilerDispatcher Statistics:\n";
    oss << "  Compilation count: " << m_compilationCount << "\n";
    oss << "  Successful: " << m_successfulCompilations << "\n";
    oss << "  Failed: " << m_failedCompilations << "\n";
    oss << "  Success rate: " << (m_compilationCount > 0 ? (m_successfulCompilations * 100.0 / m_compilationCount) : 0) << "%\n";
    oss << "  CHTL fragments processed: " << m_chtlFragmentsProcessed << "\n";
    oss << "  CHTL JS fragments processed: " << m_chtlJSFragmentsProcessed << "\n";
    oss << "  CSS fragments processed: " << m_cssFragmentsProcessed << "\n";
    oss << "  JavaScript fragments processed: " << m_jsFragmentsProcessed << "\n";
    
    if (m_codeMerger) {
        oss << "\n" << m_codeMerger->getStatistics();
    }
    
    return oss.str();
}

std::string CompilerDispatcher::processCHTLFragment(const CodeFragment& fragment) {
    if (m_debugMode) {
        std::cout << "[CompilerDispatcher] Processing CHTL fragment..." << std::endl;
    }
    
    // 这里应该实现CHTL片段的处理逻辑
    // 1. 使用CHTLParser解析
    // 2. 使用CHTLGenerator生成
    // 简化实现，直接返回内容
    return fragment.content;
}

std::string CompilerDispatcher::processCHTLJSFragment(const CodeFragment& fragment) {
    if (m_debugMode) {
        std::cout << "[CompilerDispatcher] Processing CHTL JS fragment..." << std::endl;
    }
    
    // 这里应该实现CHTL JS片段的处理逻辑
    // 1. 使用CJMODCompiler编译
    // 简化实现，直接返回内容
    return fragment.content;
}

std::string CompilerDispatcher::processCSSFragment(const CodeFragment& fragment) {
    if (m_debugMode) {
        std::cout << "[CompilerDispatcher] Processing CSS fragment..." << std::endl;
    }
    
    // 这里应该实现CSS片段的处理逻辑
    // 简化实现，直接返回内容
    return fragment.content;
}

std::string CompilerDispatcher::processJavaScriptFragment(const CodeFragment& fragment) {
    if (m_debugMode) {
        std::cout << "[CompilerDispatcher] Processing JavaScript fragment..." << std::endl;
    }
    
    // 这里应该实现JavaScript片段的处理逻辑
    // 简化实现，直接返回内容
    return fragment.content;
}

void CompilerDispatcher::addError(const std::string& message) {
    if (m_context) {
        m_context->addError("CompilerDispatcher Error: " + message);
    }
}

void CompilerDispatcher::addWarning(const std::string& message) {
    if (m_context) {
        m_context->addWarning("CompilerDispatcher Warning: " + message);
    }
}

} // namespace CHTL