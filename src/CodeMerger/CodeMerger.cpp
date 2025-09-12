#include "CHTL/CodeMerger/CodeMerger.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <regex>

namespace CHTL {

CodeMerger::CodeMerger(std::shared_ptr<CHTLContext> context)
    : m_context(context)
    , m_debugMode(false)
    , m_totalFragments(0)
    , m_processedFragments(0)
    , m_mergeCount(0)
    , m_successfulMerges(0)
    , m_failedMerges(0)
{
}

CodeMerger::~CodeMerger() {
}

void CodeMerger::addFragment(const CodeFragment& fragment) {
    if (m_debugMode) {
        std::cout << "[CodeMerger] Adding fragment: type=" << static_cast<int>(fragment.type) 
                  << " (" << getFragmentTypeName(fragment.type) << ") from " << fragment.sourceFile << ":" << fragment.line << std::endl;
    }
    
    m_fragments.push_back(fragment);
    m_fragmentsByType[fragment.type].push_back(&m_fragments.back());
    m_totalFragments++;
}

void CodeMerger::addFragment(CodeFragmentType type, const std::string& content, size_t line, size_t column, const std::string& sourceFile) {
    addFragment(CodeFragment(type, content, line, column, sourceFile));
}

std::string CodeMerger::merge() {
    m_mergeCount++;
    
    if (m_debugMode) {
        std::cout << "[CodeMerger] Starting merge process..." << std::endl;
    }
    
    try {
        std::ostringstream result;
        
        // 按类型合并代码片段
        std::vector<CodeFragmentType> types = {CodeFragmentType::CHTL, CodeFragmentType::CHTL_JS, CodeFragmentType::CSS, CodeFragmentType::JS, CodeFragmentType::HTML};
        
        for (auto type : types) {
            auto it = m_fragmentsByType.find(type);
            if (it != m_fragmentsByType.end()) {
                std::string merged = mergeByType(type);
                if (!merged.empty()) {
                    result << merged << "\n";
                }
            }
        }
        
        m_successfulMerges++;
        return result.str();
    } catch (const std::exception& e) {
        addError("Merge failed: " + std::string(e.what()));
        m_failedMerges++;
        return "";
    }
}

std::string CodeMerger::mergeByType(CodeFragmentType type) {
    auto it = m_fragmentsByType.find(type);
    if (it == m_fragmentsByType.end()) {
        return "";
    }
    
    std::ostringstream result;
    
    for (auto* fragment : it->second) {
        if (!fragment->processed) {
            std::string content = processPlaceholders(fragment->content);
            
            // 如果是HTML片段，需要解析并转换为HTML
            if (type == CodeFragmentType::HTML) {
                // 这里应该调用CHTLParser来解析HTML片段
                // 简化实现，直接输出内容
                result << content << "\n";
            } else {
                result << content << "\n";
            }
            
            fragment->processed = true;
            m_processedFragments++;
        }
    }
    
    return result.str();
}

std::string CodeMerger::generateHTML(bool includeDefaultStructure) {
    if (m_debugMode) {
        std::cout << "[CodeMerger] Generating HTML output..." << std::endl;
    }
    
    std::ostringstream html;
    
    if (includeDefaultStructure) {
        html << generateHTMLHead();
        html << generateHTMLBody();
        html << generateHTMLTail();
    } else {
        // 只输出CHTL和CHTL JS处理后的内容
        html << mergeByType(CodeFragmentType::CHTL);
        html << mergeByType(CodeFragmentType::CHTL_JS);
    }
    
    return html.str();
}

std::string CodeMerger::generateCSS() {
    if (m_debugMode) {
        std::cout << "[CodeMerger] Generating CSS output..." << std::endl;
    }
    
    return mergeByType(CodeFragmentType::CSS);
}

std::string CodeMerger::generateJavaScript() {
    if (m_debugMode) {
        std::cout << "[CodeMerger] Generating JavaScript output..." << std::endl;
    }
    
    return mergeByType(CodeFragmentType::JS);
}

void CodeMerger::clear() {
    m_fragments.clear();
    m_fragmentsByType.clear();
    m_totalFragments = 0;
    m_processedFragments = 0;
    m_mergeCount = 0;
    m_successfulMerges = 0;
    m_failedMerges = 0;
}

size_t CodeMerger::getFragmentCount() const {
    return m_totalFragments;
}

size_t CodeMerger::getFragmentCount(CodeFragmentType type) const {
    auto it = m_fragmentsByType.find(type);
    return it != m_fragmentsByType.end() ? it->second.size() : 0;
}

const CodeFragment* CodeMerger::getFragment(CodeFragmentType type, size_t index) const {
    auto it = m_fragmentsByType.find(type);
    if (it != m_fragmentsByType.end() && index < it->second.size()) {
        return it->second[index];
    }
    return nullptr;
}

void CodeMerger::setDebugMode(bool debug) {
    m_debugMode = debug;
}

bool CodeMerger::isDebugMode() const {
    return m_debugMode;
}

std::string CodeMerger::getStatistics() const {
    std::ostringstream oss;
    oss << "CodeMerger Statistics:\n";
    oss << "  Total fragments: " << m_totalFragments << "\n";
    oss << "  Processed fragments: " << m_processedFragments << "\n";
    oss << "  Merge count: " << m_mergeCount << "\n";
    oss << "  Successful merges: " << m_successfulMerges << "\n";
    oss << "  Failed merges: " << m_failedMerges << "\n";
    oss << "  Success rate: " << (m_mergeCount > 0 ? (m_successfulMerges * 100.0 / m_mergeCount) : 0) << "%\n";
    
    // 按类型统计
    oss << "  Fragment types:\n";
    for (const auto& pair : m_fragmentsByType) {
        oss << "    " << getFragmentTypeName(pair.first) << ": " << pair.second.size() << "\n";
    }
    
    return oss.str();
}

std::string CodeMerger::processPlaceholders(const std::string& content) {
    std::string result = content;
    
    // 处理占位符替换
    // 这里可以根据需要实现更复杂的占位符处理逻辑
    
    return result;
}

std::string CodeMerger::decodePlaceholders(const std::string& content) {
    std::string result = content;
    
    // 解码占位符
    // 将占位符替换回原始代码
    
    // 处理JS代码占位符
    std::regex jsPlaceholderRegex(R"(_JS_CODE_PLACEHOLDER_)");
    result = std::regex_replace(result, jsPlaceholderRegex, "");
    
    return result;
}

std::string CodeMerger::generateHTMLHead() {
    std::ostringstream head;
    head << "<!DOCTYPE html>\n";
    head << "<html>\n";
    head << "<head>\n";
    head << "  <meta charset=\"UTF-8\">\n";
    head << "  <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n";
    head << "  <title>CHTL Generated Page</title>\n";
    
    // 添加CSS
    std::string css = generateCSS();
    if (!css.empty()) {
        head << "  <style>\n";
        head << css << "\n";
        head << "  </style>\n";
    }
    
    head << "</head>\n";
    return head.str();
}

std::string CodeMerger::generateHTMLBody() {
    std::ostringstream body;
    body << "<body>\n";
    body << "  <!-- Generated by CHTL Compiler v1.0.0 -->\n";
    
    // 添加CHTL处理后的内容
    std::string chtlContent = mergeByType(CodeFragmentType::CHTL);
    if (!chtlContent.empty()) {
        body << chtlContent << "\n";
    }
    
    // 添加JavaScript
    std::string js = generateJavaScript();
    if (!js.empty()) {
        body << "  <script>\n";
        body << js << "\n";
        body << "  </script>\n";
    }
    
    body << "</body>\n";
    return body.str();
}

std::string CodeMerger::generateHTMLTail() {
    return "</html>\n";
}

std::string CodeMerger::getFragmentTypeName(CodeFragmentType type) {
    switch (type) {
        case CodeFragmentType::CHTL:
            return "CHTL";
        case CodeFragmentType::CHTL_JS:
            return "CHTL_JS";
        case CodeFragmentType::CSS:
            return "CSS";
        case CodeFragmentType::JS:
            return "JavaScript";
        case CodeFragmentType::HTML:
            return "HTML";
        case CodeFragmentType::UNKNOWN:
        default:
            return "Unknown";
    }
}

void CodeMerger::addError(const std::string& message) {
    if (m_context) {
        m_context->addError("CodeMerger Error: " + message);
    }
}

void CodeMerger::addWarning(const std::string& message) {
    if (m_context) {
        m_context->addWarning("CodeMerger Warning: " + message);
    }
}

} // namespace CHTL