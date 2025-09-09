#include "CJMOD/CJMODAPI.h"
#include <sstream>
#include <algorithm>
#include <regex>

namespace CHTL {

// AtomArg 实现
AtomArg::AtomArg(AtomArgType type, const std::string& value)
    : m_type(type), m_value(value), m_hasValue(false)
{
}

bool AtomArg::isPlaceholder() const {
    return m_type == AtomArgType::PLACEHOLDER;
}

bool AtomArg::isOptional() const {
    return m_type == AtomArgType::OPTIONAL;
}

bool AtomArg::isRequired() const {
    return m_type == AtomArgType::REQUIRED;
}

bool AtomArg::isUnordered() const {
    return m_type == AtomArgType::UNORDERED;
}

bool AtomArg::isVariadic() const {
    return m_type == AtomArgType::VARIADIC;
}

void AtomArg::bind(std::function<std::string(const std::string&)> binder) {
    m_binder = binder;
}

void AtomArg::fillValue(const std::string& value) {
    m_value = value;
    m_hasValue = true;
}

void AtomArg::fillValue(int value) {
    m_value = std::to_string(value);
    m_hasValue = true;
}

std::string AtomArg::getBoundValue() const {
    if (m_binder && m_hasValue) {
        return m_binder(m_value);
    }
    return m_value;
}

// Arg 实现
Arg::Arg() : m_transformedResult("")
{
}

void Arg::addAtom(const AtomArg& atom) {
    m_atoms.push_back(atom);
}

void Arg::addAtom(AtomArgType type, const std::string& value) {
    m_atoms.emplace_back(type, value);
}

AtomArg& Arg::operator[](size_t index) {
    return m_atoms[index];
}

const AtomArg& Arg::operator[](size_t index) const {
    return m_atoms[index];
}

void Arg::bind(const std::string& key, std::function<std::string(const std::string&)> binder) {
    for (auto& atom : m_atoms) {
        if (atom.getValue() == key) {
            atom.bind(binder);
        }
    }
}

void Arg::fillValue(const Arg& result) {
    for (size_t i = 0; i < std::min(m_atoms.size(), result.size()); ++i) {
        m_atoms[i].fillValue(result[i].getValue());
    }
}

void Arg::transform(const std::string& template_str) {
    m_transformedResult = template_str;
    // 这里可以实现更复杂的模板替换逻辑
}

void Arg::print() const {
    std::cout << "[";
    for (size_t i = 0; i < m_atoms.size(); ++i) {
        if (i > 0) std::cout << ", ";
        std::cout << "\"" << m_atoms[i].getValue() << "\"";
    }
    std::cout << "]" << std::endl;
}

// Syntax 实现
Arg Syntax::analyze(const std::string& syntax) {
    Arg args;
    std::string current;
    
    for (size_t i = 0; i < syntax.length(); ++i) {
        char c = syntax[i];
        
        if (c == '$') {
            // 处理占位符
            if (!current.empty()) {
                args.addAtom(AtomArgType::PLACEHOLDER, current);
                current.clear();
            }
            
            // 检查占位符类型
            if (i + 1 < syntax.length()) {
                char next = syntax[i + 1];
                if (next == '?') {
                    args.addAtom(AtomArgType::OPTIONAL, "$?");
                    ++i; // 跳过下一个字符
                } else if (next == '!') {
                    args.addAtom(AtomArgType::REQUIRED, "$!");
                    ++i; // 跳过下一个字符
                } else if (next == '_') {
                    args.addAtom(AtomArgType::UNORDERED, "$_");
                    ++i; // 跳过下一个字符
                } else {
                    args.addAtom(AtomArgType::PLACEHOLDER, "$");
                }
            } else {
                args.addAtom(AtomArgType::PLACEHOLDER, "$");
            }
        } else if (c == '.' && i + 2 < syntax.length() && 
                   syntax[i + 1] == '.' && syntax[i + 2] == '.') {
            // 处理不定参数
            if (!current.empty()) {
                args.addAtom(AtomArgType::PLACEHOLDER, current);
                current.clear();
            }
            args.addAtom(AtomArgType::VARIADIC, "...");
            i += 2; // 跳过后续两个点
        } else {
            current += c;
        }
    }
    
    if (!current.empty()) {
        args.addAtom(AtomArgType::PLACEHOLDER, current);
    }
    
    return args;
}

bool Syntax::isObject(const std::string& code) {
    std::string trimmed = code;
    // 去除首尾空白
    trimmed.erase(0, trimmed.find_first_not_of(" \t\n\r"));
    trimmed.erase(trimmed.find_last_not_of(" \t\n\r") + 1);
    
    return hasValidObjectSyntax(trimmed);
}

bool Syntax::isFunction(const std::string& code) {
    std::string trimmed = code;
    trimmed.erase(0, trimmed.find_first_not_of(" \t\n\r"));
    trimmed.erase(trimmed.find_last_not_of(" \t\n\r") + 1);
    
    return hasValidFunctionSyntax(trimmed);
}

bool Syntax::isArray(const std::string& code) {
    std::string trimmed = code;
    trimmed.erase(0, trimmed.find_first_not_of(" \t\n\r"));
    trimmed.erase(trimmed.find_last_not_of(" \t\n\r") + 1);
    
    return hasValidArraySyntax(trimmed);
}

bool Syntax::isCHTLJSFunction(const std::string& code) {
    std::string trimmed = code;
    trimmed.erase(0, trimmed.find_first_not_of(" \t\n\r"));
    trimmed.erase(trimmed.find_last_not_of(" \t\n\r") + 1);
    
    return hasValidCHTLJSFunctionSyntax(trimmed);
}

bool Syntax::isBalanced(const std::string& code, char open, char close) {
    int count = 0;
    for (char c : code) {
        if (c == open) count++;
        else if (c == close) count--;
        if (count < 0) return false;
    }
    return count == 0;
}

bool Syntax::hasValidObjectSyntax(const std::string& code) {
    if (code.empty() || code[0] != '{' || code.back() != '}') {
        return false;
    }
    
    return isBalanced(code, '{', '}');
}

bool Syntax::hasValidFunctionSyntax(const std::string& code) {
    // 简单的函数语法检查
    return code.find("function") != std::string::npos && 
           code.find('(') != std::string::npos &&
           code.find(')') != std::string::npos;
}

bool Syntax::hasValidArraySyntax(const std::string& code) {
    if (code.empty() || code[0] != '[' || code.back() != ']') {
        return false;
    }
    
    return isBalanced(code, '[', ']');
}

bool Syntax::hasValidCHTLJSFunctionSyntax(const std::string& code) {
    // CHTL JS函数语法：functionName { key: value, ... }
    size_t bracePos = code.find('{');
    if (bracePos == std::string::npos) return false;
    
    std::string functionName = code.substr(0, bracePos);
    std::string body = code.substr(bracePos);
    
    // 检查函数名是否有效
    if (functionName.empty() || functionName.find_first_not_of(" \t\n\r") == std::string::npos) {
        return false;
    }
    
    // 检查函数体是否是有效的对象语法
    return hasValidObjectSyntax(body);
}

// CJMODScanner 实现
Arg CJMODScanner::scan(const Arg& args, const std::string& keyword) {
    // 这里实现扫描逻辑
    Arg result;
    // 简化实现，实际需要更复杂的扫描算法
    return result;
}

Arg CJMODScanner::scanCode(const std::string& code, const std::string& keyword) {
    return doublePointerScan(code, keyword);
}

std::vector<std::string> CJMODScanner::splitByKeyword(const std::string& code, const std::string& keyword) {
    std::vector<std::string> result;
    size_t start = 0;
    size_t pos = 0;
    
    while ((pos = code.find(keyword, start)) != std::string::npos) {
        if (pos > start) {
            result.push_back(code.substr(start, pos - start));
        }
        result.push_back(keyword);
        start = pos + keyword.length();
    }
    
    if (start < code.length()) {
        result.push_back(code.substr(start));
    }
    
    return result;
}

bool CJMODScanner::isKeywordMatch(const std::string& text, const std::string& keyword) {
    return text.find(keyword) != std::string::npos;
}

// CJMODGenerator 实现
std::string CJMODGenerator::exportResult(const Arg& args) {
    return args.getTransformedResult();
}

void CJMODGenerator::exportToFile(const Arg& args, const std::string& filePath) {
    std::string result = exportResult(args);
    // 这里应该写入文件，简化实现
}

std::string CJMODGenerator::processTemplate(const std::string& template_str, const Arg& args) {
    std::string result = template_str;
    // 实现模板处理逻辑
    return result;
}

// CHTLJSFunction 实现
std::unordered_map<std::string, std::shared_ptr<CHTLJSFunction>> CHTLJSFunction::s_functions;
std::unordered_map<std::string, bool> CHTLJSFunction::s_virtualObjectSupport;

CHTLJSFunction::CHTLJSFunction(const std::string& syntax)
    : m_syntax(syntax), m_supportsVirtualObject(false)
{
}

std::shared_ptr<CHTLJSFunction> CHTLJSFunction::CreateCHTLJSFunction(const std::string& syntax) {
    auto func = std::make_shared<CHTLJSFunction>(syntax);
    s_functions[syntax] = func;
    return func;
}

void CHTLJSFunction::bindVirtualObject(const std::string& functionName) {
    s_virtualObjectSupport[functionName] = true;
}

bool CHTLJSFunction::supportsVirtualObject(const std::string& functionName) {
    auto it = s_virtualObjectSupport.find(functionName);
    return it != s_virtualObjectSupport.end() && it->second;
}

std::string CHTLJSFunction::generateJS(const std::vector<std::string>& args) const {
    std::ostringstream oss;
    oss << "function(";
    for (size_t i = 0; i < args.size(); ++i) {
        if (i > 0) oss << ", ";
        oss << "arg" << i;
    }
    oss << ") { ";
    oss << processArguments(args);
    oss << " }";
    return oss.str();
}

std::string CHTLJSFunction::processArguments(const std::vector<std::string>& args) const {
    // 简化的参数处理
    return "// Generated from CHTL JS function";
}

// ScanAlgorithm 实现
std::vector<std::string> ScanAlgorithm::doublePointerScan(const std::string& code, const std::string& keyword) {
    std::vector<std::string> result;
    size_t left = 0, right = 0;
    
    while (right < code.length()) {
        // 检查是否遇到关键字
        if (isKeywordAtPosition(code, right, keyword)) {
            // 收集关键字前的部分
            if (right > left) {
                result.push_back(code.substr(left, right - left));
            }
            result.push_back(keyword);
            right += keyword.length();
            left = right;
        } else {
            right++;
        }
    }
    
    // 收集剩余部分
    if (left < code.length()) {
        result.push_back(code.substr(left));
    }
    
    return result;
}

std::string ScanAlgorithm::preExtract(const std::string& code, const std::string& keyword) {
    size_t keywordPos = findKeywordStart(code, keyword);
    if (keywordPos == std::string::npos) {
        return code;
    }
    
    return code.substr(0, keywordPos);
}

bool ScanAlgorithm::isKeywordAtPosition(const std::string& code, size_t pos, const std::string& keyword) {
    if (pos + keyword.length() > code.length()) {
        return false;
    }
    
    return code.substr(pos, keyword.length()) == keyword;
}

size_t ScanAlgorithm::findKeywordStart(const std::string& code, const std::string& keyword) {
    return code.find(keyword);
}

size_t ScanAlgorithm::findKeywordEnd(const std::string& code, size_t start, const std::string& keyword) {
    return start + keyword.length();
}

} // namespace CHTL