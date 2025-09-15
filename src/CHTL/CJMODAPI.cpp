#include "CHTL/CJMODAPI.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <regex>

namespace CHTL {

// Arg 实现
Arg::Arg() = default;

Arg::Arg(const std::string& value) : value_(value) {
}

Arg::~Arg() = default;

const std::string& Arg::getValue() const {
    return value_;
}

void Arg::setValue(const std::string& value) {
    value_ = value;
}

void Arg::bind(const std::string& pattern, std::function<std::string(const std::string&)> handler) {
    handlers_[pattern] = handler;
}

std::string Arg::match(const std::string& pattern, std::function<std::string(const std::string&)> handler) const {
    auto it = handlers_.find(pattern);
    if (it != handlers_.end()) {
        return it->second(value_);
    }
    return handler(value_);
}

void Arg::fillValue(const Arg& other) {
    value_ = other.value_;
    handlers_ = other.handlers_;
}

void Arg::fillValue(const std::vector<Arg>& others) {
    if (!others.empty()) {
        fillValue(others[0]);
    }
}

std::string Arg::transform(const std::string& template_str) const {
    return processTemplate(template_str);
}

void Arg::print() const {
    std::cout << "[" << value_ << "]" << std::endl;
}

std::string Arg::toString() const {
    return "[" + value_ + "]";
}

bool Arg::isValid() const {
    return !value_.empty();
}

std::vector<std::string> Arg::getValidationErrors() const {
    std::vector<std::string> errors;
    
    if (value_.empty()) {
        errors.push_back("Value cannot be empty");
    }
    
    return errors;
}

std::string Arg::processTemplate(const std::string& template_str) const {
    std::string result = template_str;
    
    // 简单的模板处理，实际实现可能需要更复杂的逻辑
    std::regex placeholderRegex(R"(\$\{([^}]+)\})");
    std::smatch match;
    
    while (std::regex_search(result, match, placeholderRegex)) {
        std::string placeholder = match[1].str();
        std::string replacement = value_;
        
        result.replace(match.position(), match.length(), replacement);
    }
    
    return result;
}

std::string Arg::replacePlaceholders(const std::string& str, const std::map<std::string, std::string>& replacements) const {
    std::string result = str;
    
    for (const auto& pair : replacements) {
        std::regex placeholderRegex("\\$\\{" + pair.first + "\\}");
        result = std::regex_replace(result, placeholderRegex, pair.second);
    }
    
    return result;
}

// AtomArg 实现
AtomArg::AtomArg() : is_atom_(false) {
}

AtomArg::AtomArg(const std::string& value, const std::string& type) 
    : Arg(value), type_(type), is_atom_(isAtomicValue(value)) {
}

AtomArg::~AtomArg() = default;

const std::string& AtomArg::getType() const {
    return type_;
}

void AtomArg::setType(const std::string& type) {
    type_ = type;
}

bool AtomArg::isAtom() const {
    return is_atom_;
}

std::string AtomArg::getAtomValue() const {
    if (is_atom_) {
        return extractAtomValue(getValue());
    }
    return getValue();
}

void AtomArg::setAtomValue(const std::string& value) {
    setValue(value);
    is_atom_ = isAtomicValue(value);
}

bool AtomArg::validateAtom() const {
    return is_atom_ && !getValue().empty();
}

std::vector<std::string> AtomArg::getAtomValidationErrors() const {
    std::vector<std::string> errors;
    
    if (!is_atom_) {
        errors.push_back("Not an atomic value");
    }
    
    if (getValue().empty()) {
        errors.push_back("Atom value cannot be empty");
    }
    
    return errors;
}

bool AtomArg::isAtomicValue(const std::string& value) const {
    // 简单的原子值检测，实际实现可能需要更复杂的逻辑
    return !value.empty() && value.find(' ') == std::string::npos;
}

std::string AtomArg::extractAtomValue(const std::string& value) const {
    // 简单的原子值提取，实际实现可能需要更复杂的逻辑
    return value;
}

// Syntax 实现
Syntax::Syntax() = default;

Syntax::~Syntax() = default;

Arg Syntax::analyze(const std::string& pattern) {
    std::vector<std::string> tokens = tokenizePattern(pattern);
    
    Arg result;
    for (const auto& token : tokens) {
        result.setValue(result.getValue() + token);
    }
    
    return result;
}

std::vector<Arg> Syntax::analyzeMultiple(const std::string& pattern) {
    std::vector<Arg> results;
    std::vector<std::string> tokens = tokenizePattern(pattern);
    
    for (const auto& token : tokens) {
        Arg arg(token);
        results.push_back(arg);
    }
    
    return results;
}

bool Syntax::matches(const std::string& input, const std::string& pattern) {
    std::regex patternRegex(pattern);
    return std::regex_match(input, patternRegex);
}

std::vector<std::string> Syntax::extractMatches(const std::string& input, const std::string& pattern) {
    std::vector<std::string> matches;
    std::regex patternRegex(pattern);
    std::sregex_iterator begin(input.begin(), input.end(), patternRegex);
    std::sregex_iterator end;
    
    for (auto it = begin; it != end; ++it) {
        matches.push_back(it->str());
    }
    
    return matches;
}

bool Syntax::isValidPattern(const std::string& pattern) {
    try {
        std::regex patternRegex(pattern);
        return true;
    } catch (const std::regex_error&) {
        return false;
    }
}

std::vector<std::string> Syntax::getPatternValidationErrors(const std::string& pattern) {
    std::vector<std::string> errors;
    
    try {
        std::regex patternRegex(pattern);
    } catch (const std::regex_error& e) {
        errors.push_back("Invalid regex pattern: " + std::string(e.what()));
    }
    
    return errors;
}

std::vector<std::string> Syntax::tokenizePattern(const std::string& pattern) {
    std::vector<std::string> tokens;
    std::string currentToken;
    
    for (char c : pattern) {
        if (isSpecialCharacter(c)) {
            if (!currentToken.empty()) {
                tokens.push_back(currentToken);
                currentToken.clear();
            }
            tokens.push_back(std::string(1, c));
        } else {
            currentToken += c;
        }
    }
    
    if (!currentToken.empty()) {
        tokens.push_back(currentToken);
    }
    
    return tokens;
}

bool Syntax::isSpecialCharacter(char c) {
    return c == '$' || c == '*' || c == '+' || c == '?' || c == '|' || c == '(' || c == ')' || c == '[' || c == ']' || c == '{' || c == '}';
}

std::string Syntax::escapeSpecialCharacters(const std::string& str) {
    std::string result;
    for (char c : str) {
        if (isSpecialCharacter(c)) {
            result += '\\';
        }
        result += c;
    }
    return result;
}

std::string Syntax::unescapeSpecialCharacters(const std::string& str) {
    std::string result;
    for (size_t i = 0; i < str.length(); ++i) {
        if (str[i] == '\\' && i + 1 < str.length() && isSpecialCharacter(str[i + 1])) {
            result += str[i + 1];
            ++i;
        } else {
            result += str[i];
        }
    }
    return result;
}

// CJMODScanner 实现
CJMODScanner::CJMODScanner() : scan_mode_("dual_pointer") {
}

CJMODScanner::~CJMODScanner() = default;

Arg CJMODScanner::scan(const Arg& args, const std::string& keyword) {
    std::string content = args.getValue();
    return dualPointerScan(content, keyword);
}

std::vector<Arg> CJMODScanner::scanMultiple(const std::vector<Arg>& args, const std::string& keyword) {
    std::vector<Arg> results;
    
    for (const auto& arg : args) {
        Arg result = scan(arg, keyword);
        if (result.isValid()) {
            results.push_back(result);
        }
    }
    
    return results;
}

Arg CJMODScanner::dualPointerScan(const std::string& content, const std::string& keyword) {
    auto bounds = findKeywordBounds(content, keyword);
    if (bounds.first != std::string::npos && bounds.second != std::string::npos) {
        std::string extracted = extractContentRange(content, bounds.first, bounds.second);
        return Arg(extracted);
    }
    
    return Arg();
}

std::vector<Arg> CJMODScanner::dualPointerScanMultiple(const std::string& content, const std::vector<std::string>& keywords) {
    std::vector<Arg> results;
    
    for (const auto& keyword : keywords) {
        Arg result = dualPointerScan(content, keyword);
        if (result.isValid()) {
            results.push_back(result);
        }
    }
    
    return results;
}

Arg CJMODScanner::preTruncateScan(const std::string& content, const std::string& keyword) {
    // 前置截取扫描实现
    size_t keywordPos = content.find(keyword);
    if (keywordPos != std::string::npos) {
        // 截取关键字前面的内容
        std::string truncated = content.substr(0, keywordPos);
        return Arg(truncated);
    }
    
    return Arg();
}

std::vector<Arg> CJMODScanner::preTruncateScanMultiple(const std::string& content, const std::vector<std::string>& keywords) {
    std::vector<Arg> results;
    
    for (const auto& keyword : keywords) {
        Arg result = preTruncateScan(content, keyword);
        if (result.isValid()) {
            results.push_back(result);
        }
    }
    
    return results;
}

void CJMODScanner::setScanMode(const std::string& mode) {
    scan_mode_ = mode;
}

void CJMODScanner::setScanOptions(const std::map<std::string, std::string>& options) {
    scan_options_ = options;
}

const std::string& CJMODScanner::getScanMode() const {
    return scan_mode_;
}

const std::map<std::string, std::string>& CJMODScanner::getScanOptions() const {
    return scan_options_;
}

std::pair<size_t, size_t> CJMODScanner::findKeywordBounds(const std::string& content, const std::string& keyword) {
    size_t start = content.find(keyword);
    if (start == std::string::npos) {
        return {std::string::npos, std::string::npos};
    }
    
    size_t end = start + keyword.length() - 1;
    return {start, end};
}

std::string CJMODScanner::extractContentRange(const std::string& content, size_t start, size_t end) {
    if (!isValidContentRange(content, start, end)) {
        return "";
    }
    
    return content.substr(start, end - start + 1);
}

bool CJMODScanner::isValidContentRange(const std::string& content, size_t start, size_t end) {
    return start < content.length() && end < content.length() && start <= end;
}

std::vector<std::pair<size_t, size_t>> CJMODScanner::findAllKeywordBounds(const std::string& content, const std::string& keyword) {
    std::vector<std::pair<size_t, size_t>> bounds;
    size_t pos = 0;
    
    while (pos < content.length()) {
        size_t start = content.find(keyword, pos);
        if (start == std::string::npos) {
            break;
        }
        
        size_t end = start + keyword.length() - 1;
        bounds.push_back({start, end});
        pos = start + 1;
    }
    
    return bounds;
}

// CJMODGenerator 实现
CJMODGenerator::CJMODGenerator() : generation_mode_("function") {
}

CJMODGenerator::~CJMODGenerator() = default;

std::string CJMODGenerator::exportResult(const Arg& args) {
    return args.toString();
}

std::string CJMODGenerator::exportResult(const std::vector<Arg>& args) {
    std::ostringstream oss;
    oss << "[";
    
    for (size_t i = 0; i < args.size(); ++i) {
        if (i > 0) {
            oss << ", ";
        }
        oss << args[i].toString();
    }
    
    oss << "]";
    return oss.str();
}

std::string CJMODGenerator::generateCode(const Arg& args, const std::string& template_str) {
    return processTemplate(template_str, args);
}

std::string CJMODGenerator::generateFunction(const Arg& args, const std::string& functionName) {
    std::string template_str = "function " + functionName + "() {\n    return " + args.getValue() + ";\n}";
    return generateCode(args, template_str);
}

std::string CJMODGenerator::generateClass(const Arg& args, const std::string& className) {
    std::string template_str = "class " + className + " {\n    constructor() {\n        " + args.getValue() + "\n    }\n}";
    return generateCode(args, template_str);
}

std::string CJMODGenerator::generateModule(const Arg& args, const std::string& moduleName) {
    std::string template_str = "module.exports = " + args.getValue() + ";";
    return generateCode(args, template_str);
}

void CJMODGenerator::setGenerationMode(const std::string& mode) {
    generation_mode_ = mode;
}

void CJMODGenerator::setGenerationOptions(const std::map<std::string, std::string>& options) {
    generation_options_ = options;
}

const std::string& CJMODGenerator::getGenerationMode() const {
    return generation_mode_;
}

const std::map<std::string, std::string>& CJMODGenerator::getGenerationOptions() const {
    return generation_options_;
}

std::string CJMODGenerator::processTemplate(const std::string& template_str, const Arg& args) {
    std::string result = template_str;
    
    // 简单的模板处理
    std::regex placeholderRegex(R"(\$\{([^}]+)\})");
    std::smatch match;
    
    while (std::regex_search(result, match, placeholderRegex)) {
        std::string placeholder = match[1].str();
        std::string replacement = args.getValue();
        
        result.replace(match.position(), match.length(), replacement);
    }
    
    return result;
}

std::string CJMODGenerator::processTemplate(const std::string& template_str, const std::vector<Arg>& args) {
    std::string result = template_str;
    
    // 处理多个参数
    for (size_t i = 0; i < args.size(); ++i) {
        std::string placeholder = "{" + std::to_string(i) + "}";
        std::string replacement = args[i].getValue();
        
        size_t pos = result.find(placeholder);
        if (pos != std::string::npos) {
            result.replace(pos, placeholder.length(), replacement);
        }
    }
    
    return result;
}

std::string CJMODGenerator::replacePlaceholders(const std::string& str, const std::map<std::string, std::string>& replacements) {
    std::string result = str;
    
    for (const auto& pair : replacements) {
        std::regex placeholderRegex("\\$\\{" + pair.first + "\\}");
        result = std::regex_replace(result, placeholderRegex, pair.second);
    }
    
    return result;
}

std::string CJMODGenerator::formatCode(const std::string& code) {
    // 简单的代码格式化
    std::string result = code;
    
    // 添加适当的缩进
    std::istringstream iss(result);
    std::ostringstream oss;
    std::string line;
    int indentLevel = 0;
    
    while (std::getline(iss, line)) {
        if (line.find('}') != std::string::npos) {
            indentLevel--;
        }
        
        for (int i = 0; i < indentLevel; ++i) {
            oss << "    ";
        }
        oss << line << "\n";
        
        if (line.find('{') != std::string::npos) {
            indentLevel++;
        }
    }
    
    return oss.str();
}

// CHTLJSFunction 实现
CHTLJSFunction::CHTLJSFunction() = default;

CHTLJSFunction::CHTLJSFunction(const std::string& name, const std::string& body) 
    : name_(name), body_(body) {
}

CHTLJSFunction::~CHTLJSFunction() = default;

const std::string& CHTLJSFunction::getName() const {
    return name_;
}

void CHTLJSFunction::setName(const std::string& name) {
    name_ = name;
}

const std::string& CHTLJSFunction::getBody() const {
    return body_;
}

void CHTLJSFunction::setBody(const std::string& body) {
    body_ = body;
}

std::string CHTLJSFunction::generateFunction() const {
    std::ostringstream oss;
    
    oss << "function " << formatFunctionName(name_) << "(";
    oss << formatParameters(parameters_) << ") {\n";
    oss << "    " << formatFunctionBody(body_) << "\n";
    oss << "}";
    
    return oss.str();
}

std::string CHTLJSFunction::generateFunctionCall(const std::vector<std::string>& args) const {
    std::ostringstream oss;
    
    oss << formatFunctionName(name_) << "(";
    for (size_t i = 0; i < args.size(); ++i) {
        if (i > 0) {
            oss << ", ";
        }
        oss << args[i];
    }
    oss << ")";
    
    return oss.str();
}

void CHTLJSFunction::addParameter(const std::string& param) {
    parameters_.push_back(param);
}

void CHTLJSFunction::removeParameter(const std::string& param) {
    parameters_.erase(
        std::remove(parameters_.begin(), parameters_.end(), param),
        parameters_.end()
    );
}

std::vector<std::string> CHTLJSFunction::getParameters() const {
    return parameters_;
}

bool CHTLJSFunction::validateFunction() const {
    return isValidFunctionName(name_) && isValidFunctionBody(body_);
}

std::vector<std::string> CHTLJSFunction::getValidationErrors() const {
    std::vector<std::string> errors;
    
    if (!isValidFunctionName(name_)) {
        errors.push_back("Invalid function name");
    }
    
    if (!isValidFunctionBody(body_)) {
        errors.push_back("Invalid function body");
    }
    
    return errors;
}

std::string CHTLJSFunction::formatFunctionName(const std::string& name) const {
    return name;
}

std::string CHTLJSFunction::formatFunctionBody(const std::string& body) const {
    return body;
}

std::string CHTLJSFunction::formatParameters(const std::vector<std::string>& params) const {
    std::ostringstream oss;
    
    for (size_t i = 0; i < params.size(); ++i) {
        if (i > 0) {
            oss << ", ";
        }
        oss << params[i];
    }
    
    return oss.str();
}

bool CHTLJSFunction::isValidFunctionName(const std::string& name) const {
    if (name.empty()) {
        return false;
    }
    
    // 检查函数名是否以字母或下划线开头
    if (!std::isalpha(name[0]) && name[0] != '_') {
        return false;
    }
    
    // 检查函数名是否只包含字母、数字和下划线
    for (char c : name) {
        if (!std::isalnum(c) && c != '_') {
            return false;
        }
    }
    
    return true;
}

bool CHTLJSFunction::isValidFunctionBody(const std::string& body) const {
    return !body.empty();
}

// CJMODAPIManager 实现
CJMODAPIManager::CJMODAPIManager() = default;

CJMODAPIManager::~CJMODAPIManager() = default;

void CJMODAPIManager::registerFunction(const std::string& name, std::shared_ptr<CHTLJSFunction> function) {
    if (function) {
        functions_[name] = function;
    }
}

void CJMODAPIManager::unregisterFunction(const std::string& name) {
    functions_.erase(name);
}

std::shared_ptr<CHTLJSFunction> CJMODAPIManager::getFunction(const std::string& name) const {
    auto it = functions_.find(name);
    if (it != functions_.end()) {
        return it->second;
    }
    return nullptr;
}

std::vector<std::string> CJMODAPIManager::getFunctionNames() const {
    std::vector<std::string> names;
    for (const auto& pair : functions_) {
        names.push_back(pair.first);
    }
    return names;
}

std::vector<std::shared_ptr<CHTLJSFunction>> CJMODAPIManager::getAllFunctions() const {
    std::vector<std::shared_ptr<CHTLJSFunction>> functions;
    for (const auto& pair : functions_) {
        functions.push_back(pair.second);
    }
    return functions;
}

bool CJMODAPIManager::hasFunction(const std::string& name) const {
    return functions_.find(name) != functions_.end();
}

Arg CJMODAPIManager::scanAndGenerate(const std::string& content, const std::string& pattern) {
    if (!scanner_) {
        return Arg();
    }
    
    Arg args = Syntax::analyze(pattern);
    Arg result = scanner_->scan(args, content);
    
    if (result.isValid() && generator_) {
        std::string generated = generator_->exportResult(result);
        result.setValue(generated);
    }
    
    return result;
}

std::string CJMODAPIManager::generateCode(const std::string& content, const std::string& template_str) {
    if (!generator_) {
        return content;
    }
    
    Arg args(content);
    return generator_->generateCode(args, template_str);
}

void CJMODAPIManager::setScanner(std::shared_ptr<CJMODScanner> scanner) {
    scanner_ = scanner;
}

void CJMODAPIManager::setGenerator(std::shared_ptr<CJMODGenerator> generator) {
    generator_ = generator;
}

std::shared_ptr<CJMODScanner> CJMODAPIManager::getScanner() const {
    return scanner_;
}

std::shared_ptr<CJMODGenerator> CJMODAPIManager::getGenerator() const {
    return generator_;
}

void CJMODAPIManager::clear() {
    clearFunctions();
    scanner_.reset();
    generator_.reset();
}

void CJMODAPIManager::clearFunctions() {
    functions_.clear();
}

std::string CJMODAPIManager::processContent(const std::string& content, const std::string& pattern) const {
    std::string result = content;
    
    // 应用注册的函数
    result = applyFunctions(result);
    
    return result;
}

std::string CJMODAPIManager::applyFunctions(const std::string& content) const {
    std::string result = content;
    
    for (const auto& pair : functions_) {
        const std::string& functionName = pair.first;
        const auto& function = pair.second;
        
        if (function) {
            std::string functionCall = function->generateFunctionCall({});
            // 简单的函数调用替换
            size_t pos = result.find(functionName);
            if (pos != std::string::npos) {
                result.replace(pos, functionName.length(), functionCall);
            }
        }
    }
    
    return result;
}

bool CJMODAPIManager::isFunctionCall(const std::string& content, size_t position) const {
    if (position >= content.length()) {
        return false;
    }
    
    // 简单的函数调用检测
    for (const auto& pair : functions_) {
        const std::string& functionName = pair.first;
        if (position + functionName.length() <= content.length() &&
            content.substr(position, functionName.length()) == functionName) {
            return true;
        }
    }
    
    return false;
}

std::string CJMODAPIManager::extractFunctionName(const std::string& content, size_t position) const {
    for (const auto& pair : functions_) {
        const std::string& functionName = pair.first;
        if (position + functionName.length() <= content.length() &&
            content.substr(position, functionName.length()) == functionName) {
            return functionName;
        }
    }
    
    return "";
}

} // namespace CHTL