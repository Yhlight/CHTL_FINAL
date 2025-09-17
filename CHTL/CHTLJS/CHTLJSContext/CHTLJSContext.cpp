#include "CHTLJSContext.h"
#include <iostream>
#include <sstream>

namespace CHTLJS {

CHTLJSContext::CHTLJSContext() 
    : debugMode(false), minifyOutput(false) {
    initializeBuiltinFunctions();
    initializeCHTLJSFunctions();
}

void CHTLJSContext::initializeBuiltinFunctions() {
    // 基础JavaScript函数
    functions["console.log"] = [](const std::vector<std::string>& args) -> std::string {
        std::ostringstream oss;
        oss << "console.log(";
        for (size_t i = 0; i < args.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << args[i];
        }
        oss << ")";
        return oss.str();
    };
    
    functions["console.error"] = [](const std::vector<std::string>& args) -> std::string {
        std::ostringstream oss;
        oss << "console.error(";
        for (size_t i = 0; i < args.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << args[i];
        }
        oss << ")";
        return oss.str();
    };
    
    functions["console.warn"] = [](const std::vector<std::string>& args) -> std::string {
        std::ostringstream oss;
        oss << "console.warn(";
        for (size_t i = 0; i < args.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << args[i];
        }
        oss << ")";
        return oss.str();
    };
    
    functions["alert"] = [](const std::vector<std::string>& args) -> std::string {
        std::ostringstream oss;
        oss << "alert(";
        for (size_t i = 0; i < args.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << args[i];
        }
        oss << ")";
        return oss.str();
    };
    
    functions["parseInt"] = [](const std::vector<std::string>& args) -> std::string {
        if (args.empty()) return "parseInt()";
        return "parseInt(" + args[0] + ")";
    };
    
    functions["parseFloat"] = [](const std::vector<std::string>& args) -> std::string {
        if (args.empty()) return "parseFloat()";
        return "parseFloat(" + args[0] + ")";
    };
    
    functions["toString"] = [](const std::vector<std::string>& args) -> std::string {
        if (args.empty()) return "toString()";
        return args[0] + ".toString()";
    };
    
    functions["Math.abs"] = [](const std::vector<std::string>& args) -> std::string {
        if (args.empty()) return "Math.abs()";
        return "Math.abs(" + args[0] + ")";
    };
    
    functions["Math.floor"] = [](const std::vector<std::string>& args) -> std::string {
        if (args.empty()) return "Math.floor()";
        return "Math.floor(" + args[0] + ")";
    };
    
    functions["Math.ceil"] = [](const std::vector<std::string>& args) -> std::string {
        if (args.empty()) return "Math.ceil()";
        return "Math.ceil(" + args[0] + ")";
    };
    
    functions["Math.round"] = [](const std::vector<std::string>& args) -> std::string {
        if (args.empty()) return "Math.round()";
        return "Math.round(" + args[0] + ")";
    };
    
    functions["Math.random"] = [](const std::vector<std::string>& args) -> std::string {
        return "Math.random()";
    };
    
    functions["Math.max"] = [](const std::vector<std::string>& args) -> std::string {
        std::ostringstream oss;
        oss << "Math.max(";
        for (size_t i = 0; i < args.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << args[i];
        }
        oss << ")";
        return oss.str();
    };
    
    functions["Math.min"] = [](const std::vector<std::string>& args) -> std::string {
        std::ostringstream oss;
        oss << "Math.min(";
        for (size_t i = 0; i < args.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << args[i];
        }
        oss << ")";
        return oss.str();
    };
}

void CHTLJSContext::initializeCHTLJSFunctions() {
    // CHTL JS 特有函数
    functions["Listen"] = [](const std::vector<std::string>& args) -> std::string {
        std::ostringstream oss;
        oss << "Listen({";
        for (size_t i = 0; i < args.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << args[i];
        }
        oss << "})";
        return oss.str();
    };
    
    functions["Animate"] = [](const std::vector<std::string>& args) -> std::string {
        std::ostringstream oss;
        oss << "Animate({";
        for (size_t i = 0; i < args.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << args[i];
        }
        oss << "})";
        return oss.str();
    };
    
    functions["Router"] = [](const std::vector<std::string>& args) -> std::string {
        std::ostringstream oss;
        oss << "Router({";
        for (size_t i = 0; i < args.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << args[i];
        }
        oss << "})";
        return oss.str();
    };
    
    functions["ScriptLoader"] = [](const std::vector<std::string>& args) -> std::string {
        std::ostringstream oss;
        oss << "ScriptLoader({";
        for (size_t i = 0; i < args.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << args[i];
        }
        oss << "})";
        return oss.str();
    };
    
    functions["Delegate"] = [](const std::vector<std::string>& args) -> std::string {
        std::ostringstream oss;
        oss << "Delegate({";
        for (size_t i = 0; i < args.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << args[i];
        }
        oss << "})";
        return oss.str();
    };
    
    functions["printMylove"] = [](const std::vector<std::string>& args) -> std::string {
        std::ostringstream oss;
        oss << "printMylove({";
        for (size_t i = 0; i < args.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << args[i];
        }
        oss << "})";
        return oss.str();
    };
    
    functions["iNeverAway"] = [](const std::vector<std::string>& args) -> std::string {
        std::ostringstream oss;
        oss << "iNeverAway({";
        for (size_t i = 0; i < args.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << args[i];
        }
        oss << "})";
        return oss.str();
    };
}

void CHTLJSContext::setVariable(const std::string& name, const std::string& value) {
    variables[name] = value;
}

std::string CHTLJSContext::getVariable(const std::string& name) const {
    auto it = variables.find(name);
    if (it != variables.end()) {
        return it->second;
    }
    return "";
}

bool CHTLJSContext::hasVariable(const std::string& name) const {
    return variables.find(name) != variables.end();
}

void CHTLJSContext::removeVariable(const std::string& name) {
    variables.erase(name);
}

void CHTLJSContext::setFunction(const std::string& name, 
                                std::function<std::string(const std::vector<std::string>&)> func) {
    functions[name] = func;
}

std::function<std::string(const std::vector<std::string>&)> CHTLJSContext::getFunction(const std::string& name) const {
    auto it = functions.find(name);
    if (it != functions.end()) {
        return it->second;
    }
    return nullptr;
}

bool CHTLJSContext::hasFunction(const std::string& name) const {
    return functions.find(name) != functions.end();
}

void CHTLJSContext::removeFunction(const std::string& name) {
    functions.erase(name);
}

void CHTLJSContext::pushScope() {
    scopeStack.push_back(std::unordered_map<std::string, std::string>());
}

void CHTLJSContext::popScope() {
    if (!scopeStack.empty()) {
        scopeStack.pop_back();
    }
}

void CHTLJSContext::clearScope() {
    scopeStack.clear();
}

void CHTLJSContext::setVirtualObject(const std::string& name, const std::string& value) {
    virtualObjects[name] = value;
}

std::string CHTLJSContext::getVirtualObject(const std::string& name) const {
    auto it = virtualObjects.find(name);
    if (it != virtualObjects.end()) {
        return it->second;
    }
    return "";
}

bool CHTLJSContext::hasVirtualObject(const std::string& name) const {
    return virtualObjects.find(name) != virtualObjects.end();
}

void CHTLJSContext::setSelector(const std::string& name, const std::string& value) {
    selectors[name] = value;
}

std::string CHTLJSContext::getSelector(const std::string& name) const {
    auto it = selectors.find(name);
    if (it != virtualObjects.end()) {
        return it->second;
    }
    return "";
}

bool CHTLJSContext::hasSelector(const std::string& name) const {
    return selectors.find(name) != selectors.end();
}

void CHTLJSContext::setReactiveValue(const std::string& name, const std::string& value) {
    reactiveValues[name] = value;
}

std::string CHTLJSContext::getReactiveValue(const std::string& name) const {
    auto it = reactiveValues.find(name);
    if (it != reactiveValues.end()) {
        return it->second;
    }
    return "";
}

bool CHTLJSContext::hasReactiveValue(const std::string& name) const {
    return reactiveValues.find(name) != reactiveValues.end();
}

void CHTLJSContext::setDebugMode(bool debug) {
    debugMode = debug;
}

bool CHTLJSContext::isDebugMode() const {
    return debugMode;
}

void CHTLJSContext::setMinifyOutput(bool minify) {
    minifyOutput = minify;
}

bool CHTLJSContext::isMinifyOutput() const {
    return minifyOutput;
}

void CHTLJSContext::setOutputPath(const std::string& path) {
    outputPath = path;
}

std::string CHTLJSContext::getOutputPath() const {
    return outputPath;
}

void CHTLJSContext::clear() {
    variables.clear();
    functions.clear();
    scopeStack.clear();
    virtualObjects.clear();
    selectors.clear();
    reactiveValues.clear();
}

void CHTLJSContext::reset() {
    clear();
    initializeBuiltinFunctions();
    initializeCHTLJSFunctions();
}

std::vector<std::string> CHTLJSContext::getVariableNames() const {
    std::vector<std::string> names;
    for (const auto& pair : variables) {
        names.push_back(pair.first);
    }
    return names;
}

std::vector<std::string> CHTLJSContext::getFunctionNames() const {
    std::vector<std::string> names;
    for (const auto& pair : functions) {
        names.push_back(pair.first);
    }
    return names;
}

std::vector<std::string> CHTLJSContext::getVirtualObjectNames() const {
    std::vector<std::string> names;
    for (const auto& pair : virtualObjects) {
        names.push_back(pair.first);
    }
    return names;
}

std::vector<std::string> CHTLJSContext::getSelectorNames() const {
    std::vector<std::string> names;
    for (const auto& pair : selectors) {
        names.push_back(pair.first);
    }
    return names;
}

std::vector<std::string> CHTLJSContext::getReactiveValueNames() const {
    std::vector<std::string> names;
    for (const auto& pair : reactiveValues) {
        names.push_back(pair.first);
    }
    return names;
}

void CHTLJSContext::printContext() const {
    std::cout << "=== CHTL JS Context ===" << std::endl;
    printVariables();
    printFunctions();
    printVirtualObjects();
    printSelectors();
    printReactiveValues();
    std::cout << "=======================" << std::endl;
}

void CHTLJSContext::printVariables() const {
    std::cout << "Variables:" << std::endl;
    for (const auto& pair : variables) {
        std::cout << "  " << pair.first << " = " << pair.second << std::endl;
    }
}

void CHTLJSContext::printFunctions() const {
    std::cout << "Functions:" << std::endl;
    for (const auto& pair : functions) {
        std::cout << "  " << pair.first << std::endl;
    }
}

void CHTLJSContext::printVirtualObjects() const {
    std::cout << "Virtual Objects:" << std::endl;
    for (const auto& pair : virtualObjects) {
        std::cout << "  " << pair.first << " = " << pair.second << std::endl;
    }
}

void CHTLJSContext::printSelectors() const {
    std::cout << "Selectors:" << std::endl;
    for (const auto& pair : selectors) {
        std::cout << "  " << pair.first << " = " << pair.second << std::endl;
    }
}

void CHTLJSContext::printReactiveValues() const {
    std::cout << "Reactive Values:" << std::endl;
    for (const auto& pair : reactiveValues) {
        std::cout << "  " << pair.first << " = " << pair.second << std::endl;
    }
}

} // namespace CHTLJS