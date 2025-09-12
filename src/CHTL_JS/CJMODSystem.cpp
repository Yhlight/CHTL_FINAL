#include "CJMODSystem.h"
#include <sstream>
#include <regex>
#include <algorithm>
#include <iostream>

namespace CHTL {

// AtomArg方法实现
void AtomArg::bind(std::function<std::string(const std::string&)> func) {
    binder = func;
}

void AtomArg::fillValue(const std::string& val) {
    value = val;
    filled = true;
}

void AtomArg::fillValue(int val) {
    value = std::to_string(val);
    filled = true;
}

std::string AtomArg::getValue() const {
    if (binder && !value.empty()) {
        return binder(value);
    }
    return value;
}

// Arg方法实现
Arg::Arg(const std::vector<std::string>& values) {
    for (const auto& val : values) {
        args.emplace_back(AtomArgType::PLACEHOLDER, val);
    }
}

void Arg::addArg(const AtomArg& arg) {
    args.push_back(arg);
}

void Arg::bind(const std::string& pattern, std::function<std::string(const std::string&)> func) {
    for (auto& arg : args) {
        if (arg.value == pattern) {
            arg.bind(func);
        }
    }
}

std::string Arg::match(const std::string& pattern, std::function<std::string(const std::string&)> func) {
    for (auto& arg : args) {
        if (arg.value == pattern) {
            matchCount[pattern]++;
            return func(arg.value);
        }
    }
    return "";
}

void Arg::fillValue(const Arg& result) {
    for (size_t i = 0; i < std::min(args.size(), result.args.size()); i++) {
        args[i].fillValue(result.args[i].getValue());
    }
}

void Arg::transform(const std::string& templatePattern) {
    // 转换模板为最终代码
    // 这里需要根据具体的转换规则来实现
}

void Arg::print() const {
    std::cout << "[";
    for (size_t i = 0; i < args.size(); i++) {
        if (i > 0) std::cout << ", ";
        std::cout << "\"" << args[i].getValue() << "\"";
    }
    std::cout << "]" << std::endl;
}

AtomArg& Arg::operator[](size_t index) {
    return args[index];
}

const AtomArg& Arg::operator[](size_t index) const {
    return args[index];
}

size_t Arg::size() const {
    return args.size();
}

// Syntax方法实现
Arg Syntax::analyze(const std::string& syntax) {
    Arg result;
    
    // 简单的语法分析，实际需要更复杂的解析逻辑
    std::regex pattern(R"(\$[?!_]*)");
    std::sregex_iterator iter(syntax.begin(), syntax.end(), pattern);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        std::string match = iter->str();
        AtomArgType type = AtomArgType::PLACEHOLDER;
        
        if (match == "$?") type = AtomArgType::OPTIONAL;
        else if (match == "$!") type = AtomArgType::REQUIRED;
        else if (match == "$_") type = AtomArgType::UNORDERED;
        else if (match == "...") type = AtomArgType::VARIADIC;
        else if (match.length() > 1) type = AtomArgType::COMBINED;
        
        result.addArg(AtomArg(type, match));
    }
    
    return result;
}

bool Syntax::isObject(const std::string& code) {
    return code.find("{") != std::string::npos && 
           code.find("}") != std::string::npos &&
           code.find(":") != std::string::npos;
}

bool Syntax::isFunction(const std::string& code) {
    return code.find("function") != std::string::npos ||
           code.find("=>") != std::string::npos;
}

bool Syntax::isArray(const std::string& code) {
    return code.find("[") != std::string::npos && 
           code.find("]") != std::string::npos;
}

bool Syntax::isCHTLJSFunction(const std::string& code) {
    return code.find("{") != std::string::npos && 
           code.find("}") != std::string::npos &&
           code.find(":") != std::string::npos &&
           !isObject(code) && !isArray(code);
}

// CJMODScanner方法实现
Arg CJMODScanner::scan(const Arg& args, const std::string& keyword) {
    Arg result;
    
    // 扫描逻辑
    for (const auto& arg : args.args) {
        if (arg.value.find(keyword) != std::string::npos) {
            result.addArg(arg);
        }
    }
    
    return result;
}

Arg CJMODScanner::dualPointerScan(const std::string& code, const std::string& keyword) {
    Arg result;
    
    size_t front = 0, back = 0;
    size_t codeLength = code.length();
    
    while (front < codeLength) {
        if (code.substr(front, keyword.length()) == keyword) {
            // 找到关键字，开始收集代码
            back = front;
            while (back < codeLength && code[back] != ';' && code[back] != '\n') {
                back++;
            }
            
            std::string fragment = code.substr(front, back - front);
            result.addArg(AtomArg(AtomArgType::PLACEHOLDER, fragment));
            front = back;
        } else {
            front++;
        }
    }
    
    return result;
}

std::string CJMODScanner::preprocessCode(const std::string& code, const std::string& keyword) {
    // 前置截取处理
    return code;
}

// CJMODGenerator方法实现
std::string CJMODGenerator::exportResult(const Arg& args) {
    std::stringstream js;
    
    for (const auto& arg : args.args) {
        js << arg.getValue() << " ";
    }
    
    return js.str();
}

std::string CJMODGenerator::generateFunction(const std::string& name, 
                                            const std::vector<std::string>& parameters, 
                                            const std::string& body) {
    std::stringstream js;
    
    js << "function " << name << "(";
    
    for (size_t i = 0; i < parameters.size(); i++) {
        if (i > 0) js << ", ";
        js << parameters[i];
    }
    
    js << ") {\n";
    js << body;
    js << "\n}";
    
    return js.str();
}

std::string CJMODGenerator::generateVariable(const std::string& name, 
                                            const std::string& value, 
                                            bool isConst) {
    return (isConst ? "const " : "let ") + name + " = " + value + ";";
}

// CHTLJSFunction方法实现
CHTLJSFunction CHTLJSFunction::createCHTLJSFunction(const std::string& syntax) {
    CHTLJSFunction func("", syntax);
    
    // 解析语法，提取函数名和参数
    std::regex nameRegex(R"((\w+)\s*\{)");
    std::smatch matches;
    
    if (std::regex_search(syntax, matches, nameRegex)) {
        func.name = matches[1].str();
    }
    
    return func;
}

void CHTLJSFunction::bindVirtualObject(const std::string& functionName) {
    // 绑定虚对象支持
}

std::string CHTLJSFunction::generateCode() const {
    std::stringstream js;
    
    js << "// CHTL JS Function: " << name << "\n";
    js << "function " << name << "(";
    
    for (size_t i = 0; i < parameters.size(); i++) {
        if (i > 0) js << ", ";
        js << parameters[i];
    }
    
    js << ") {\n";
    js << body;
    js << "\n}";
    
    return js.str();
}

bool CHTLJSFunction::validateSyntax() const {
    return !name.empty() && !syntax.empty();
}

// CJMODModule方法实现
void CJMODModule::addFunction(const CHTLJSFunction& func) {
    functions.push_back(func);
}

void CJMODModule::addExport(const std::string& name, const std::string& value) {
    exports[name] = value;
}

std::string CJMODModule::generateModuleCode() const {
    std::stringstream js;
    
    js << "// CJMOD Module: " << name << "\n";
    js << "// Version: " << version << "\n";
    js << "// Description: " << description << "\n\n";
    
    for (const auto& func : functions) {
        js << func.generateCode() << "\n\n";
    }
    
    return js.str();
}

bool CJMODModule::validateModule() const {
    return !name.empty() && !version.empty();
}

// CJMODSystem方法实现
CJMODSystem::CJMODSystem() {}

CJMODSystem::~CJMODSystem() {}

void CJMODSystem::registerModule(std::unique_ptr<CJMODModule> module) {
    modules[module->name] = std::move(module);
}

CJMODModule* CJMODSystem::getModule(const std::string& name) {
    auto it = modules.find(name);
    return it != modules.end() ? it->second.get() : nullptr;
}

const std::map<std::string, std::unique_ptr<CJMODModule>>& CJMODSystem::getAllModules() const {
    return modules;
}

std::string CJMODSystem::processImport(const std::string& moduleName) {
    auto module = getModule(moduleName);
    if (module) {
        return module->generateModuleCode();
    }
    return "";
}

std::string CJMODSystem::processFunctionCall(const std::string& functionName, 
                                            const std::map<std::string, std::string>& parameters) {
    // 处理函数调用
    return "// Function call: " + functionName;
}

bool CJMODSystem::validateModule(const std::string& moduleName) {
    auto module = getModule(moduleName);
    return module ? module->validateModule() : false;
}

void CJMODSystem::clear() {
    modules.clear();
}

std::map<std::string, std::string> CJMODSystem::parseModuleInfo(const std::string& info) {
    std::map<std::string, std::string> result;
    
    // 解析模块信息
    std::regex infoRegex(R"((\w+)\s*=\s*"([^"]*)")");
    std::sregex_iterator iter(info.begin(), info.end(), infoRegex);
    std::sregex_iterator end;
    
    for (auto it = iter; it != end; ++it) {
        result[it->str(1)] = it->str(2);
    }
    
    return result;
}

std::vector<std::string> CJMODSystem::resolveDependencies(const std::string& moduleName) {
    // 解析依赖
    return {};
}

bool CJMODSystem::detectCircularDependency(const std::string& moduleName) {
    // 检测循环依赖
    return false;
}

bool CJMODSystem::checkVersionCompatibility(const std::string& moduleName) {
    // 检查版本兼容性
    return true;
}

} // namespace CHTL