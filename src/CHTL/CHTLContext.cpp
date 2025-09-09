#include "CHTLContext.h"
#include "CHTLLexer/Lexer.h"
#include "CHTLParser.h"
#include "CHTLGenerator.h"
#include "CHTLState.h"
#include <fstream>
#include <sstream>
#include <iostream>

namespace CHTL {

CHTLContext::CHTLContext() : m_debugMode(false) {
    initializeComponents();
}

CHTLContext::~CHTLContext() = default;

void CHTLContext::initializeComponents() {
    m_lexer = std::make_unique<CHTLLexer>();
    m_parser = std::make_unique<CHTLParser>();
    m_generator = std::make_unique<CHTLGenerator>();
    m_state = std::make_unique<CHTLState>();
}

bool CHTLContext::compile(const std::string& inputFile, const std::string& outputFile) {
    clearMessages();
    
    if (!validateInputFile(inputFile)) {
        return false;
    }
    
    // 读取输入文件
    std::ifstream file(inputFile);
    if (!file.is_open()) {
        addError("无法打开输入文件: " + inputFile);
        return false;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string source = buffer.str();
    file.close();
    
    // 编译源代码
    std::string result = compileString(source);
    if (result.empty() && !m_errors.empty()) {
        return false;
    }
    
    // 确定输出文件名
    std::string output = outputFile.empty() ? generateOutputFileName(inputFile) : outputFile;
    
    // 写入输出文件
    std::ofstream outFile(output);
    if (!outFile.is_open()) {
        addError("无法创建输出文件: " + output);
        return false;
    }
    
    outFile << result;
    outFile.close();
    
    return true;
}

std::string CHTLContext::compileString(const std::string& input) {
    clearMessages();
    
    try {
        // 词法分析
        auto tokens = m_lexer->tokenize(input);
        if (tokens->empty()) {
            addError("词法分析失败：没有生成任何令牌");
            return "";
        }
        
        // 语法分析
        auto ast = m_parser->parse(std::move(tokens));
        if (!ast) {
            addError("语法分析失败：无法生成AST");
            return "";
        }
        
        // 代码生成
        std::string html = m_generator->generateCompleteHTML(std::move(ast));
        return html;
        
    } catch (const std::exception& e) {
        addError("编译过程中发生异常: " + std::string(e.what()));
        return "";
    }
}

void CHTLContext::setOption(const std::string& option, const std::string& value) {
    m_options[option] = value;
}

std::string CHTLContext::getOption(const std::string& option) const {
    auto it = m_options.find(option);
    return it != m_options.end() ? it->second : "";
}

const std::vector<std::string>& CHTLContext::getErrors() const {
    return m_errors;
}

const std::vector<std::string>& CHTLContext::getWarnings() const {
    return m_warnings;
}

void CHTLContext::clearMessages() {
    m_errors.clear();
    m_warnings.clear();
}

void CHTLContext::setDebugMode(bool debug) {
    m_debugMode = debug;
    if (m_lexer) {
        m_lexer->setDebugMode(debug);
    }
}

CHTLState* CHTLContext::getState() const {
    return m_state.get();
}

void CHTLContext::addError(const std::string& error) {
    m_errors.push_back(error);
    if (m_debugMode) {
        std::cerr << "错误: " << error << std::endl;
    }
}

void CHTLContext::addWarning(const std::string& warning) {
    m_warnings.push_back(warning);
    if (m_debugMode) {
        std::cout << "警告: " << warning << std::endl;
    }
}

bool CHTLContext::validateInputFile(const std::string& inputFile) {
    std::ifstream file(inputFile);
    if (!file.is_open()) {
        addError("文件不存在或无法访问: " + inputFile);
        return false;
    }
    
    // 检查文件扩展名
    size_t dotPos = inputFile.find_last_of('.');
    if (dotPos == std::string::npos) {
        addError("文件缺少扩展名: " + inputFile);
        return false;
    }
    
    std::string extension = inputFile.substr(dotPos + 1);
    if (extension != "chtl") {
        addError("不支持的文件类型: " + extension + "，期望 .chtl 文件");
        return false;
    }
    
    file.close();
    return true;
}

std::string CHTLContext::generateOutputFileName(const std::string& inputFile) {
    size_t dotPos = inputFile.find_last_of('.');
    if (dotPos == std::string::npos) {
        return inputFile + ".html";
    }
    
    return inputFile.substr(0, dotPos) + ".html";
}

} // namespace CHTL