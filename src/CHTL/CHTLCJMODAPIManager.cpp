#include "CHTL/CHTLCJMODAPI.h"
#include <algorithm>
#include <regex>

namespace CHTL {

// CJMODAPIManager 实现
CJMODAPIManager::CJMODAPIManager(std::shared_ptr<CHTLContext> context)
    : m_context(context) {
    // 初始化默认组件
    m_syntaxAnalyzer = std::make_shared<SyntaxAnalyzer>(context);
    m_parameterBinder = std::make_shared<ParameterBinder>(context);
    m_cjmodScanner = std::make_shared<CJMODScanner>(context);
    m_cjmodGenerator = std::make_shared<CJMODGenerator>(context);
}

void CJMODAPIManager::registerSyntaxAnalyzer(std::shared_ptr<SyntaxAnalyzer> analyzer) {
    m_syntaxAnalyzer = analyzer;
}

void CJMODAPIManager::registerParameterBinder(std::shared_ptr<ParameterBinder> binder) {
    m_parameterBinder = binder;
}

void CJMODAPIManager::registerCJMODScanner(std::shared_ptr<CJMODScanner> scanner) {
    m_cjmodScanner = scanner;
}

void CJMODAPIManager::registerCJMODGenerator(std::shared_ptr<CJMODGenerator> generator) {
    m_cjmodGenerator = generator;
}

std::shared_ptr<SyntaxAnalyzer> CJMODAPIManager::getSyntaxAnalyzer() const {
    return m_syntaxAnalyzer;
}

std::shared_ptr<ParameterBinder> CJMODAPIManager::getParameterBinder() const {
    return m_parameterBinder;
}

std::shared_ptr<CJMODScanner> CJMODAPIManager::getCJMODScanner() const {
    return m_cjmodScanner;
}

std::shared_ptr<CJMODGenerator> CJMODAPIManager::getCJMODGenerator() const {
    return m_cjmodGenerator;
}

void CJMODAPIManager::registerCHTLJSFunction(const std::string& name, std::shared_ptr<CHTLJSFunction> function) {
    m_functions[name] = function;
}

std::shared_ptr<CHTLJSFunction> CJMODAPIManager::getCHTLJSFunction(const std::string& name) const {
    auto it = m_functions.find(name);
    return (it != m_functions.end()) ? it->second : nullptr;
}

bool CJMODAPIManager::hasCHTLJSFunction(const std::string& name) const {
    return m_functions.find(name) != m_functions.end();
}

void CJMODAPIManager::registerAtomArg(const std::string& placeholder, std::shared_ptr<AtomArg> atomArg) {
    m_atomArgs[placeholder] = atomArg;
}

std::shared_ptr<AtomArg> CJMODAPIManager::getAtomArg(const std::string& placeholder) const {
    auto it = m_atomArgs.find(placeholder);
    return (it != m_atomArgs.end()) ? it->second : nullptr;
}

bool CJMODAPIManager::hasAtomArg(const std::string& placeholder) const {
    return m_atomArgs.find(placeholder) != m_atomArgs.end();
}

std::vector<std::string> CJMODAPIManager::performDoublePointerScan(const std::string& code) const {
    return doublePointerScan(code);
}

std::vector<std::string> CJMODAPIManager::performPreTruncationScan(const std::string& code) const {
    return preTruncationScan(code);
}

std::vector<std::string> CJMODAPIManager::doublePointerScan(const std::string& code) const {
    std::vector<std::string> results;
    
    if (code.empty()) {
        return results;
    }
    
    // 双指针扫描算法
    size_t left = 0;
    size_t right = code.length() - 1;
    
    while (left < right) {
        // 从左边找到第一个有效字符
        while (left < right && std::isspace(code[left])) {
            left++;
        }
        
        // 从右边找到第一个有效字符
        while (left < right && std::isspace(code[right])) {
            right--;
        }
        
        if (left < right) {
            // 提取子字符串
            std::string substring = code.substr(left, right - left + 1);
            results.push_back(substring);
            
            // 移动指针
            left++;
            right--;
        }
    }
    
    return results;
}

std::vector<std::string> CJMODAPIManager::preTruncationScan(const std::string& code) const {
    std::vector<std::string> results;
    
    if (code.empty()) {
        return results;
    }
    
    // 预截断扫描算法
    size_t pos = 0;
    size_t length = code.length();
    
    while (pos < length) {
        // 找到下一个分隔符
        size_t nextPos = pos;
        while (nextPos < length && !std::isspace(code[nextPos]) && 
               code[nextPos] != ',' && code[nextPos] != ';' && 
               code[nextPos] != '{' && code[nextPos] != '}') {
            nextPos++;
        }
        
        if (nextPos > pos) {
            std::string substring = code.substr(pos, nextPos - pos);
            results.push_back(substring);
        }
        
        // 跳过分隔符
        while (nextPos < length && (std::isspace(code[nextPos]) || 
               code[nextPos] == ',' || code[nextPos] == ';' || 
               code[nextPos] == '{' || code[nextPos] == '}')) {
            nextPos++;
        }
        
        pos = nextPos;
    }
    
    return results;
}

} // namespace CHTL