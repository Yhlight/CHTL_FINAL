#include "CHTLJSFunction.h"
#include "CJMODScanner.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <regex>

namespace CHTL {
namespace CJMOD {

// 静态成员初始化
std::unordered_map<std::string, std::shared_ptr<CHTLJSFunction>> CHTLJSFunction::functionRegistry;
std::unordered_map<std::string, bool> CHTLJSFunction::virtualObjectBindings;

CHTLJSFunction::CHTLJSFunction() : isBuilt(false) {}

CHTLJSFunction::CHTLJSFunction(const std::string& name, const std::string& template) 
    : isBuilt(false) {
    signature.name = name;
    this->template = template;
}

CHTLJSFunction::CHTLJSFunction(const FunctionSignature& signature, const std::string& template) 
    : signature(signature), template(template), isBuilt(false) {}

std::shared_ptr<CHTLJSFunction> CHTLJSFunction::CreateCHTLJSFunction(const std::string& template) {
    auto function = std::make_shared<CHTLJSFunction>();
    function->setTemplate(template);
    return function;
}

std::shared_ptr<CHTLJSFunction> CHTLJSFunction::CreateCHTLJSFunction(const std::string& name, const std::string& template) {
    auto function = std::make_shared<CHTLJSFunction>(name, template);
    return function;
}

bool CHTLJSFunction::build() {
    if (!validateSignature()) {
        return false;
    }
    
    if (!parseTemplate()) {
        return false;
    }
    
    // 生成函数代码
    std::string generatedJS = generateJavaScript();
    std::string generatedCHTLJS = generateCHTLJS();
    
    generatedFunctions.clear();
    generatedFunctions.push_back(generatedJS);
    generatedFunctions.push_back(generatedCHTLJS);
    
    isBuilt = true;
    return true;
}

bool CHTLJSFunction::build(const std::string& template) {
    setTemplate(template);
    return build();
}

bool CHTLJSFunction::rebuild() {
    isBuilt = false;
    return build();
}

void CHTLJSFunction::setSignature(const FunctionSignature& signature) {
    this->signature = signature;
    isBuilt = false;
}

FunctionSignature CHTLJSFunction::getSignature() const {
    return signature;
}

void CHTLJSFunction::setName(const std::string& name) {
    signature.name = name;
    isBuilt = false;
}

std::string CHTLJSFunction::getName() const {
    return signature.name;
}

void CHTLJSFunction::addParameter(const FunctionParameter& parameter) {
    signature.parameters.push_back(parameter);
    isBuilt = false;
}

void CHTLJSFunction::removeParameter(const std::string& parameterName) {
    signature.parameters.erase(
        std::remove_if(signature.parameters.begin(), signature.parameters.end(),
            [&parameterName](const FunctionParameter& param) {
                return param.name == parameterName;
            }),
        signature.parameters.end()
    );
    isBuilt = false;
}

void CHTLJSFunction::setParameters(const std::vector<FunctionParameter>& parameters) {
    signature.parameters = parameters;
    isBuilt = false;
}

std::vector<FunctionParameter> CHTLJSFunction::getParameters() const {
    return signature.parameters;
}

FunctionParameter CHTLJSFunction::getParameter(const std::string& name) const {
    for (const auto& param : signature.parameters) {
        if (param.name == name) {
            return param;
        }
    }
    return FunctionParameter();
}

void CHTLJSFunction::setTemplate(const std::string& template) {
    this->template = template;
    isBuilt = false;
}

std::string CHTLJSFunction::getTemplate() const {
    return template;
}

bool CHTLJSFunction::parseTemplate() {
    if (template.empty()) {
        return false;
    }
    
    // 解析模板中的函数名
    std::regex functionNameRegex(R"(\w+)");
    std::smatch match;
    if (std::regex_search(template, match, functionNameRegex)) {
        if (signature.name.empty()) {
            signature.name = match.str();
        }
    }
    
    // 解析模板中的参数
    parsedArgs = Syntax::analyze(template);
    if (!parsedArgs) {
        return false;
    }
    
    // 从模板中提取参数信息
    std::vector<std::string> placeholders = parsedArgs->getAllPlaceholders();
    for (const auto& placeholder : placeholders) {
        FunctionParameter param;
        param.name = placeholder;
        
        // 根据占位符类型设置参数属性
        if (placeholder.find("?") != std::string::npos) {
            param.isOptional = true;
        }
        if (placeholder.find("!") != std::string::npos) {
            param.isOptional = false;
        }
        if (placeholder.find("_") != std::string::npos) {
            param.isUnordered = true;
        }
        if (placeholder == "...") {
            param.isVariadic = true;
        }
        
        signature.parameters.push_back(param);
    }
    
    return true;
}

void CHTLJSFunction::bindVirtualObject(const std::string& objectName, const std::string& binding) {
    virtualObjectBindings[objectName] = binding;
}

void CHTLJSFunction::unbindVirtualObject(const std::string& objectName) {
    virtualObjectBindings.erase(objectName);
}

bool CHTLJSFunction::hasVirtualObjectBinding(const std::string& objectName) const {
    return virtualObjectBindings.find(objectName) != virtualObjectBindings.end();
}

std::string CHTLJSFunction::getVirtualObjectBinding(const std::string& objectName) const {
    auto it = virtualObjectBindings.find(objectName);
    if (it != virtualObjectBindings.end()) {
        return it->second;
    }
    return "";
}

std::unordered_map<std::string, std::string> CHTLJSFunction::getAllVirtualObjectBindings() const {
    return virtualObjectBindings;
}

void CHTLJSFunction::setSupportsVirtualObject(bool support) {
    signature.supportsVirtualObject = support;
}

void CHTLJSFunction::setSupportsUnorderedPairs(bool support) {
    signature.supportsUnorderedPairs = support;
}

void CHTLJSFunction::setSupportsOptionalPairs(bool support) {
    signature.supportsOptionalPairs = support;
}

void CHTLJSFunction::setSupportsUnquotedLiterals(bool support) {
    signature.supportsUnquotedLiterals = support;
}

bool CHTLJSFunction::getSupportsVirtualObject() const {
    return signature.supportsVirtualObject;
}

bool CHTLJSFunction::getSupportsUnorderedPairs() const {
    return signature.supportsUnorderedPairs;
}

bool CHTLJSFunction::getSupportsOptionalPairs() const {
    return signature.supportsOptionalPairs;
}

bool CHTLJSFunction::getSupportsUnquotedLiterals() const {
    return signature.supportsUnquotedLiterals;
}

std::string CHTLJSFunction::generateJavaScript() const {
    std::ostringstream js;
    
    js << "function " << signature.name << "(";
    
    // 生成参数列表
    std::string paramList = generateParameterList();
    js << paramList;
    
    js << ") {\n";
    
    // 生成函数体
    std::string functionBody = generateFunctionBody();
    js << functionBody;
    
    js << "}";
    
    return js.str();
}

std::string CHTLJSFunction::generateCHTLJS() const {
    std::ostringstream chtljs;
    
    chtljs << signature.name << " {\n";
    
    // 生成参数
    for (const auto& param : signature.parameters) {
        chtljs << "    " << param.name;
        if (param.isOptional) {
            chtljs << "?";
        }
        if (param.isUnordered) {
            chtljs << "_";
        }
        if (param.isVariadic) {
            chtljs << "...";
        }
        chtljs << ",\n";
    }
    
    chtljs << "}";
    
    return chtljs.str();
}

std::string CHTLJSFunction::generateFunctionCall(const std::vector<std::string>& arguments) const {
    std::ostringstream call;
    
    call << signature.name << "(";
    
    for (size_t i = 0; i < arguments.size(); ++i) {
        if (i > 0) {
            call << ", ";
        }
        call << arguments[i];
    }
    
    call << ")";
    
    return call.str();
}

std::string CHTLJSFunction::generateFunctionDeclaration() const {
    return generateJavaScript();
}

std::string CHTLJSFunction::call(const std::vector<std::string>& arguments) const {
    return generateFunctionCall(arguments);
}

std::string CHTLJSFunction::call(const std::unordered_map<std::string, std::string>& arguments) const {
    std::ostringstream call;
    
    call << signature.name << "({";
    
    bool first = true;
    for (const auto& pair : arguments) {
        if (!first) {
            call << ", ";
        }
        call << pair.first << ": " << pair.second;
        first = false;
    }
    
    call << "})";
    
    return call.str();
}

std::string CHTLJSFunction::call(const std::string& argumentString) const {
    std::ostringstream call;
    call << signature.name << "(" << argumentString << ")";
    return call.str();
}

bool CHTLJSFunction::validate() const {
    return validateSignature() && validateParameters();
}

bool CHTLJSFunction::validateArguments(const std::vector<std::string>& arguments) const {
    // 检查必需参数
    for (const auto& param : signature.parameters) {
        if (!param.isOptional && !param.isVariadic) {
            // 检查是否提供了必需参数
            // 这里需要更复杂的验证逻辑
        }
    }
    
    return true;
}

bool CHTLJSFunction::validateArguments(const std::unordered_map<std::string, std::string>& arguments) const {
    // 检查参数名是否匹配
    for (const auto& pair : arguments) {
        bool found = false;
        for (const auto& param : signature.parameters) {
            if (param.name == pair.first) {
                found = true;
                break;
            }
        }
        if (!found) {
            return false;
        }
    }
    
    return true;
}

std::vector<std::string> CHTLJSFunction::getValidationErrors() const {
    std::vector<std::string> errors;
    
    if (signature.name.empty()) {
        errors.push_back("Function name is empty");
    }
    
    if (template.empty()) {
        errors.push_back("Function template is empty");
    }
    
    for (const auto& param : signature.parameters) {
        if (param.name.empty()) {
            errors.push_back("Parameter name is empty");
        }
    }
    
    return errors;
}

bool CHTLJSFunction::bindVirtualObject(const std::string& functionName) {
    virtualObjectBindings[functionName] = true;
    return true;
}

bool CHTLJSFunction::unbindVirtualObject(const std::string& functionName) {
    virtualObjectBindings.erase(functionName);
    return true;
}

bool CHTLJSFunction::hasVirtualObjectBinding(const std::string& functionName) {
    return virtualObjectBindings.find(functionName) != virtualObjectBindings.end();
}

void CHTLJSFunction::registerFunction(const std::string& name, std::shared_ptr<CHTLJSFunction> function) {
    functionRegistry[name] = function;
}

void CHTLJSFunction::unregisterFunction(const std::string& name) {
    functionRegistry.erase(name);
}

std::shared_ptr<CHTLJSFunction> CHTLJSFunction::getFunction(const std::string& name) {
    auto it = functionRegistry.find(name);
    if (it != functionRegistry.end()) {
        return it->second;
    }
    return nullptr;
}

bool CHTLJSFunction::hasFunction(const std::string& name) {
    return functionRegistry.find(name) != functionRegistry.end();
}

std::vector<std::string> CHTLJSFunction::getAllFunctionNames() {
    std::vector<std::string> names;
    for (const auto& pair : functionRegistry) {
        names.push_back(pair.first);
    }
    return names;
}

void CHTLJSFunction::clearRegistry() {
    functionRegistry.clear();
    virtualObjectBindings.clear();
}

void CHTLJSFunction::printSignature() const {
    std::cout << "=== Function Signature ===" << std::endl;
    std::cout << "Name: " << signature.name << std::endl;
    std::cout << "Return Type: " << signature.returnType << std::endl;
    std::cout << "Supports Virtual Object: " << (signature.supportsVirtualObject ? "Yes" : "No") << std::endl;
    std::cout << "Supports Unordered Pairs: " << (signature.supportsUnorderedPairs ? "Yes" : "No") << std::endl;
    std::cout << "Supports Optional Pairs: " << (signature.supportsOptionalPairs ? "Yes" : "No") << std::endl;
    std::cout << "Supports Unquoted Literals: " << (signature.supportsUnquotedLiterals ? "Yes" : "No") << std::endl;
    std::cout << "=========================" << std::endl;
}

void CHTLJSFunction::printParameters() const {
    std::cout << "=== Function Parameters ===" << std::endl;
    for (const auto& param : signature.parameters) {
        std::cout << "Parameter: " << param.name << std::endl;
        std::cout << "  Type: " << param.type << std::endl;
        std::cout << "  Optional: " << (param.isOptional ? "Yes" : "No") << std::endl;
        std::cout << "  Unordered: " << (param.isUnordered ? "Yes" : "No") << std::endl;
        std::cout << "  Variadic: " << (param.isVariadic ? "Yes" : "No") << std::endl;
        if (!param.defaultValue.empty()) {
            std::cout << "  Default: " << param.defaultValue << std::endl;
        }
    }
    std::cout << "==========================" << std::endl;
}

void CHTLJSFunction::printTemplate() const {
    std::cout << "=== Function Template ===" << std::endl;
    std::cout << template << std::endl;
    std::cout << "========================" << std::endl;
}

void CHTLJSFunction::printVirtualObjectBindings() const {
    std::cout << "=== Virtual Object Bindings ===" << std::endl;
    for (const auto& pair : virtualObjectBindings) {
        std::cout << pair.first << " -> " << pair.second << std::endl;
    }
    std::cout << "===============================" << std::endl;
}

void CHTLJSFunction::printGeneratedFunctions() const {
    std::cout << "=== Generated Functions ===" << std::endl;
    for (size_t i = 0; i < generatedFunctions.size(); ++i) {
        std::cout << "Function " << i << ":" << std::endl;
        std::cout << generatedFunctions[i] << std::endl;
        std::cout << std::endl;
    }
    std::cout << "===========================" << std::endl;
}

void CHTLJSFunction::printStatistics() const {
    std::cout << "=== CHTL JS Function Statistics ===" << std::endl;
    std::cout << "Function name: " << signature.name << std::endl;
    std::cout << "Parameter count: " << signature.parameters.size() << std::endl;
    std::cout << "Template length: " << template.length() << std::endl;
    std::cout << "Virtual object bindings: " << virtualObjectBindings.size() << std::endl;
    std::cout << "Generated functions: " << generatedFunctions.size() << std::endl;
    std::cout << "Is built: " << (isBuilt ? "Yes" : "No") << std::endl;
    std::cout << "===================================" << std::endl;
}

void CHTLJSFunction::clear() {
    signature = FunctionSignature();
    template.clear();
    parsedArgs.reset();
    virtualObjectBindings.clear();
    generatedFunctions.clear();
    isBuilt = false;
}

void CHTLJSFunction::reset() {
    clear();
}

bool CHTLJSFunction::isEmpty() const {
    return signature.name.empty() && template.empty();
}

bool CHTLJSFunction::isBuilt() const {
    return isBuilt;
}

bool CHTLJSFunction::validateSignature() const {
    if (signature.name.empty()) {
        return false;
    }
    
    if (template.empty()) {
        return false;
    }
    
    return true;
}

bool CHTLJSFunction::validateParameters() const {
    for (const auto& param : signature.parameters) {
        if (param.name.empty()) {
            return false;
        }
    }
    
    return true;
}

std::string CHTLJSFunction::generateParameterList() const {
    std::ostringstream params;
    
    for (size_t i = 0; i < signature.parameters.size(); ++i) {
        if (i > 0) {
            params << ", ";
        }
        
        const auto& param = signature.parameters[i];
        params << param.name;
        
        if (param.isOptional && !param.defaultValue.empty()) {
            params << " = " << param.defaultValue;
        }
        
        if (param.isVariadic) {
            params << "...";
        }
    }
    
    return params.str();
}

std::string CHTLJSFunction::generateFunctionBody() const {
    std::ostringstream body;
    
    body << "    // Generated function body\n";
    
    if (signature.supportsVirtualObject) {
        body << generateVirtualObjectSupport();
    }
    
    if (signature.supportsUnorderedPairs) {
        body << generateUnorderedPairSupport();
    }
    
    if (signature.supportsOptionalPairs) {
        body << generateOptionalPairSupport();
    }
    
    if (signature.supportsUnquotedLiterals) {
        body << generateUnquotedLiteralSupport();
    }
    
    // 基本函数体
    body << "    // Function implementation\n";
    body << "    return arguments;\n";
    
    return body.str();
}

std::string CHTLJSFunction::generateVirtualObjectSupport() const {
    return "    // Virtual object support\n    // Vir object handling\n";
}

std::string CHTLJSFunction::generateUnorderedPairSupport() const {
    return "    // Unordered pair support\n    // Unordered key-value pair handling\n";
}

std::string CHTLJSFunction::generateOptionalPairSupport() const {
    return "    // Optional pair support\n    // Optional parameter handling\n";
}

std::string CHTLJSFunction::generateUnquotedLiteralSupport() const {
    return "    // Unquoted literal support\n    // Unquoted string handling\n";
}

} // namespace CJMOD
} // namespace CHTL