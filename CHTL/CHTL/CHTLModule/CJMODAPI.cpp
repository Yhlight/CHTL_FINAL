#include "CJMODAPI.hpp"
#include <CHTL/CHTLJS/CHTLJSLexer/CHTLJSLexer.hpp>
#include <CHTL/CHTLJS/CHTLJSParser/CHTLJSParser.hpp>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <set>

namespace CHTL {

CJMODAPI::CJMODAPI() {}

bool CJMODAPI::parseModule(const std::string& moduleName, const std::string& content) {
    try {
        std::shared_ptr<CHTLJSBaseNode> ast;
        if (!parseCHTLJSContent(content, ast)) {
            return false;
        }
        
        moduleASTs_[moduleName] = ast;
        loadedModules_.insert(moduleName);
        
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

bool CJMODAPI::parseModuleFromFile(const std::string& moduleName, const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string content((std::istreambuf_iterator<char>(file)),
                       std::istreambuf_iterator<char>());
    file.close();
    
    return parseModule(moduleName, content);
}

std::vector<std::string> CJMODAPI::analyzeSyntax(const std::string& content) {
    std::vector<std::string> errors;
    
    try {
        std::istringstream stream(content);
        CHTLJSLexer lexer(stream);
        CHTLJSToken token;
        
        while (true) {
            token = lexer.nextToken();
            if (token.getType() == CHTLJSTokenType::ERROR) {
                errors.push_back("Lexical error: " + token.getValue());
            }
            if (token.getType() == CHTLJSTokenType::END_OF_FILE) {
                break;
            }
        }
    } catch (const std::exception& e) {
        errors.push_back("Syntax analysis error: " + std::string(e.what()));
    }
    
    return errors;
}

std::vector<std::string> CJMODAPI::getSyntaxErrors(const std::string& content) {
    return analyzeSyntax(content);
}

bool CJMODAPI::validateSyntax(const std::string& content) {
    auto errors = analyzeSyntax(content);
    return errors.empty();
}

bool CJMODAPI::bindParameter(const std::string& functionName, const std::string& parameterName, const std::string& value) {
    auto it = functions_.find(functionName);
    if (it == functions_.end()) {
        return false; // 函数不存在
    }
    
    boundParameters_[functionName][parameterName] = value;
    return true;
}

bool CJMODAPI::bindParameters(const std::string& functionName, const std::map<std::string, std::string>& parameters) {
    auto it = functions_.find(functionName);
    if (it == functions_.end()) {
        return false; // 函数不存在
    }
    
    for (const auto& [name, value] : parameters) {
        boundParameters_[functionName][name] = value;
    }
    
    return true;
}

std::map<std::string, std::string> CJMODAPI::getBoundParameters(const std::string& functionName) const {
    auto it = boundParameters_.find(functionName);
    if (it == boundParameters_.end()) {
        return {};
    }
    return it->second;
}

bool CJMODAPI::registerFunction(const CJMODFunction& function) {
    auto func = std::make_shared<CJMODFunction>(function);
    functions_[function.name] = func;
    return true;
}

bool CJMODAPI::unregisterFunction(const std::string& functionName) {
    auto it = functions_.find(functionName);
    if (it == functions_.end()) {
        return false;
    }
    
    functions_.erase(it);
    boundParameters_.erase(functionName);
    return true;
}

std::shared_ptr<CJMODFunction> CJMODAPI::getFunction(const std::string& functionName) const {
    auto it = functions_.find(functionName);
    if (it == functions_.end()) {
        return nullptr;
    }
    return it->second;
}

std::vector<std::string> CJMODAPI::getFunctionNames() const {
    std::vector<std::string> names;
    for (const auto& [name, func] : functions_) {
        names.push_back(name);
    }
    return names;
}

std::string CJMODAPI::generateCode(const std::string& functionName) const {
    auto it = functions_.find(functionName);
    if (it == functions_.end()) {
        return "";
    }
    
    return generateFunctionCode(*it->second);
}

std::string CJMODAPI::generateModuleCode(const std::string& moduleName) const {
    auto it = moduleASTs_.find(moduleName);
    if (it == moduleASTs_.end()) {
        return "";
    }
    
    // 生成模块代码
    std::ostringstream oss;
    oss << "// CHTL JS Module: " << moduleName << "\n";
    oss << "module.exports = {\n";
    
    // 生成导出
    auto exportIt = moduleExports_.find(moduleName);
    if (exportIt != moduleExports_.end()) {
        for (const auto& [name, value] : exportIt->second) {
            oss << "    " << name << ": " << value << ",\n";
        }
    }
    
    oss << "};\n";
    return oss.str();
}

std::string CJMODAPI::generateParameterBindingCode(const std::string& functionName) const {
    auto it = functions_.find(functionName);
    if (it == functions_.end()) {
        return "";
    }
    
    const auto& function = *it->second;
    std::ostringstream oss;
    
    oss << "function " << function.name << "(";
    
    // 生成参数列表
    for (size_t i = 0; i < function.parameters.size(); ++i) {
        if (i > 0) oss << ", ";
        oss << function.parameters[i].name;
    }
    
    oss << ") {\n";
    
    // 生成参数验证
    oss << generateParameterValidationCode(function);
    
    // 生成函数实现
    oss << function.implementation << "\n";
    
    oss << "}\n";
    return oss.str();
}

bool CJMODAPI::loadModule(const std::string& moduleName) {
    auto it = moduleASTs_.find(moduleName);
    if (it == moduleASTs_.end()) {
        return false; // 模块不存在
    }
    
    loadedModules_.insert(moduleName);
    return true;
}

bool CJMODAPI::unloadModule(const std::string& moduleName) {
    auto it = loadedModules_.find(moduleName);
    if (it == loadedModules_.end()) {
        return false; // 模块未加载
    }
    
    loadedModules_.erase(it);
    return true;
}

bool CJMODAPI::isModuleLoaded(const std::string& moduleName) const {
    return loadedModules_.find(moduleName) != loadedModules_.end();
}

std::vector<std::string> CJMODAPI::getLoadedModules() const {
    return std::vector<std::string>(loadedModules_.begin(), loadedModules_.end());
}

bool CJMODAPI::addDependency(const std::string& moduleName, const std::string& dependency) {
    moduleDependencies_[moduleName].push_back(dependency);
    return true;
}

std::vector<std::string> CJMODAPI::getDependencies(const std::string& moduleName) const {
    auto it = moduleDependencies_.find(moduleName);
    if (it == moduleDependencies_.end()) {
        return {};
    }
    return it->second;
}

bool CJMODAPI::resolveDependencies(const std::string& moduleName) {
    auto it = moduleDependencies_.find(moduleName);
    if (it == moduleDependencies_.end()) {
        return true; // 无依赖
    }
    
    for (const auto& dependency : it->second) {
        if (!isModuleLoaded(dependency)) {
            return false; // 依赖未加载
        }
    }
    
    return true;
}

bool CJMODAPI::addExport(const std::string& moduleName, const std::string& exportName, const std::string& value) {
    moduleExports_[moduleName][exportName] = value;
    return true;
}

std::string CJMODAPI::getExport(const std::string& moduleName, const std::string& exportName) const {
    auto it = moduleExports_.find(moduleName);
    if (it == moduleExports_.end()) {
        return "";
    }
    
    auto exportIt = it->second.find(exportName);
    if (exportIt == it->second.end()) {
        return "";
    }
    
    return exportIt->second;
}

std::map<std::string, std::string> CJMODAPI::getExports(const std::string& moduleName) const {
    auto it = moduleExports_.find(moduleName);
    if (it == moduleExports_.end()) {
        return {};
    }
    return it->second;
}

bool CJMODAPI::validateModule(const std::string& moduleName) const {
    auto it = moduleASTs_.find(moduleName);
    if (it == moduleASTs_.end()) {
        return false; // 模块不存在
    }
    
    if (!isModuleLoaded(moduleName)) {
        return false; // 模块未加载
    }
    
    return true;
}

bool CJMODAPI::testFunction(const std::string& functionName, const std::map<std::string, std::string>& testParameters) const {
    auto it = functions_.find(functionName);
    if (it == functions_.end()) {
        return false; // 函数不存在
    }
    
    return validateParameters(*it->second, testParameters);
}

std::string CJMODAPI::runFunction(const std::string& functionName, const std::map<std::string, std::string>& parameters) const {
    auto it = functions_.find(functionName);
    if (it == functions_.end()) {
        return ""; // 函数不存在
    }
    
    if (!validateParameters(*it->second, parameters)) {
        return ""; // 参数验证失败
    }
    
    // 这里应该执行实际的函数逻辑
    // 现在只是返回函数名作为占位符
    return "Function " + functionName + " executed with " + std::to_string(parameters.size()) + " parameters";
}

bool CJMODAPI::parseCHTLJSContent(const std::string& content, std::shared_ptr<CHTLJSBaseNode>& ast) {
    try {
        std::istringstream stream(content);
        CHTLJSLexer lexer(stream);
        CHTLJSParser parser(stream);
        ast = parser.parse();
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

std::string CJMODAPI::generateFunctionCode(const CJMODFunction& function) const {
    std::ostringstream oss;
    
    oss << "function " << function.name << "(";
    
    // 生成参数列表
    for (size_t i = 0; i < function.parameters.size(); ++i) {
        if (i > 0) oss << ", ";
        oss << function.parameters[i].name;
    }
    
    oss << ") {\n";
    
    // 生成函数实现
    oss << function.implementation << "\n";
    
    oss << "}\n";
    return oss.str();
}

std::string CJMODAPI::generateParameterValidationCode(const CJMODFunction& function) const {
    std::ostringstream oss;
    
    for (const auto& param : function.parameters) {
        oss << "    if (typeof " << param.name << " === 'undefined') {\n";
        if (param.isRequired) {
            oss << "        throw new Error('Required parameter " << param.name << " is missing');\n";
        } else {
            oss << "        " << param.name << " = " << param.defaultValue << ";\n";
        }
        oss << "    }\n";
    }
    
    return oss.str();
}

bool CJMODAPI::validateParameters(const CJMODFunction& function, const std::map<std::string, std::string>& parameters) const {
    for (const auto& param : function.parameters) {
        if (param.isRequired) {
            auto it = parameters.find(param.name);
            if (it == parameters.end()) {
                return false; // 必需参数缺失
            }
        }
    }
    
    return true;
}

std::string CJMODAPI::convertParameterType(const std::string& value, CJMODParameterType type) const {
    switch (type) {
        case CJMODParameterType::STRING:
            return "\"" + value + "\"";
        case CJMODParameterType::NUMBER:
            return value;
        case CJMODParameterType::BOOLEAN:
            return (value == "true" || value == "1") ? "true" : "false";
        case CJMODParameterType::OBJECT:
            return value;
        case CJMODParameterType::ARRAY:
            return value;
        case CJMODParameterType::FUNCTION:
            return value;
        default:
            return value;
    }
}

} // namespace CHTL