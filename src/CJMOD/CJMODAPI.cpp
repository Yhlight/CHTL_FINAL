#include "CHTL/CJMOD/CJMODAPI.h"
#include <iostream>
#include <sstream>
#include <algorithm>

namespace CHTL {

CJMODAPI::CJMODAPI()
    : m_debugMode(false)
    , m_totalFunctions(0)
    , m_functionCalls(0)
    , m_successfulCalls(0)
    , m_failedCalls(0)
{
    initializeDefaultFunctions();
}

CJMODAPI::~CJMODAPI() {
}

bool CJMODAPI::registerFunction(const CJMODAPIFunction& func) {
    if (m_debugMode) {
        std::cout << "[CJMODAPI] Registering function: " << func.name << std::endl;
    }
    
    // 检查函数是否已存在
    if (m_functionMap.find(func.name) != m_functionMap.end()) {
        if (m_debugMode) {
            std::cout << "[CJMODAPI] Function already exists: " << func.name << std::endl;
        }
        return false;
    }
    
    // 添加函数
    m_functions.push_back(func);
    m_functionMap[func.name] = m_functions.size() - 1;
    m_totalFunctions++;
    
    return true;
}

const CJMODAPIFunction* CJMODAPI::getFunction(const std::string& name) const {
    auto it = m_functionMap.find(name);
    if (it != m_functionMap.end()) {
        return &m_functions[it->second];
    }
    return nullptr;
}

bool CJMODAPI::hasFunction(const std::string& name) const {
    return m_functionMap.find(name) != m_functionMap.end();
}

const std::vector<CJMODAPIFunction>& CJMODAPI::getAllFunctions() const {
    return m_functions;
}

std::vector<CJMODAPIFunction> CJMODAPI::getFunctionsByType(CJMODAPIType type) const {
    std::vector<CJMODAPIFunction> result;
    for (const auto& func : m_functions) {
        if (func.type == type) {
            result.push_back(func);
        }
    }
    return result;
}

std::string CJMODAPI::callFunction(const std::string& name, const std::vector<std::string>& args) {
    m_functionCalls++;
    
    if (m_debugMode) {
        std::cout << "[CJMODAPI] Calling function: " << name << " with " << args.size() << " arguments" << std::endl;
    }
    
    const CJMODAPIFunction* func = getFunction(name);
    if (!func) {
        if (m_debugMode) {
            std::cout << "[CJMODAPI] Function not found: " << name << std::endl;
        }
        m_failedCalls++;
        return "";
    }
    
    // 验证参数
    if (!validateParameters(name, args)) {
        if (m_debugMode) {
            std::cout << "[CJMODAPI] Invalid parameters for function: " << name << std::endl;
        }
        m_failedCalls++;
        return "";
    }
    
    // 调用函数
    try {
        std::string result = func->handler(args);
        m_successfulCalls++;
        return result;
    } catch (const std::exception& e) {
        if (m_debugMode) {
            std::cout << "[CJMODAPI] Exception in function " << name << ": " << e.what() << std::endl;
        }
        m_failedCalls++;
        return "";
    }
}

bool CJMODAPI::validateParameters(const std::string& name, const std::vector<std::string>& args) const {
    const CJMODAPIFunction* func = getFunction(name);
    if (!func) {
        return false;
    }
    
    // 检查参数数量
    if (args.size() < func->parameters.size()) {
        // 检查是否有必需的参数缺失
        for (size_t i = args.size(); i < func->parameters.size(); ++i) {
            if (func->parameters[i].required) {
                return false;
            }
        }
    }
    
    // 检查参数类型（简化实现）
    for (size_t i = 0; i < std::min(args.size(), func->parameters.size()); ++i) {
        const auto& param = func->parameters[i];
        const std::string& arg = args[i];
        
        switch (param.type) {
            case CJMODAPIParamType::STRING:
                // 字符串类型总是有效
                break;
            case CJMODAPIParamType::NUMBER:
                // 检查是否为数字
                if (arg.empty() || !std::all_of(arg.begin(), arg.end(), [](char c) {
                    return std::isdigit(c) || c == '.' || c == '-' || c == '+';
                })) {
                    return false;
                }
                break;
            case CJMODAPIParamType::BOOLEAN:
                // 检查是否为布尔值
                if (arg != "true" && arg != "false" && arg != "1" && arg != "0") {
                    return false;
                }
                break;
            default:
                // 其他类型暂时跳过验证
                break;
        }
    }
    
    return true;
}

std::string CJMODAPI::getAPIDocumentation() const {
    std::ostringstream oss;
    oss << "CJMOD API Documentation\n";
    oss << "======================\n\n";
    
    for (const auto& func : m_functions) {
        oss << "Function: " << func.name << "\n";
        oss << "Type: " << getAPITypeName(func.type) << "\n";
        oss << "Description: " << func.description << "\n";
        oss << "Parameters:\n";
        
        for (const auto& param : func.parameters) {
            oss << "  - " << param.name << " (" << getParamTypeName(param.type) << ")";
            if (param.required) {
                oss << " [required]";
            } else {
                oss << " [optional]";
            }
            if (!param.defaultValue.empty()) {
                oss << " default: " << param.defaultValue;
            }
            if (!param.description.empty()) {
                oss << " - " << param.description;
            }
            oss << "\n";
        }
        
        if (!func.returnType.empty()) {
            oss << "Returns: " << func.returnType << "\n";
        }
        
        oss << "\n";
    }
    
    return oss.str();
}

std::string CJMODAPI::getAPIDocumentation(CJMODAPIType type) const {
    std::ostringstream oss;
    oss << "CJMOD API Documentation - " << getAPITypeName(type) << "\n";
    oss << "==========================================\n\n";
    
    auto functions = getFunctionsByType(type);
    for (const auto& func : functions) {
        oss << "Function: " << func.name << "\n";
        oss << "Description: " << func.description << "\n";
        oss << "Parameters:\n";
        
        for (const auto& param : func.parameters) {
            oss << "  - " << param.name << " (" << getParamTypeName(param.type) << ")";
            if (param.required) {
                oss << " [required]";
            } else {
                oss << " [optional]";
            }
            if (!param.defaultValue.empty()) {
                oss << " default: " << param.defaultValue;
            }
            if (!param.description.empty()) {
                oss << " - " << param.description;
            }
            oss << "\n";
        }
        
        if (!func.returnType.empty()) {
            oss << "Returns: " << func.returnType << "\n";
        }
        
        oss << "\n";
    }
    
    return oss.str();
}

void CJMODAPI::clear() {
    m_functions.clear();
    m_functionMap.clear();
    m_totalFunctions = 0;
    m_functionCalls = 0;
    m_successfulCalls = 0;
    m_failedCalls = 0;
}

void CJMODAPI::setDebugMode(bool debug) {
    m_debugMode = debug;
}

bool CJMODAPI::isDebugMode() const {
    return m_debugMode;
}

std::string CJMODAPI::getStatistics() const {
    std::ostringstream oss;
    oss << "CJMOD API Statistics:\n";
    oss << "  Total functions: " << m_totalFunctions << "\n";
    oss << "  Function calls: " << m_functionCalls << "\n";
    oss << "  Successful calls: " << m_successfulCalls << "\n";
    oss << "  Failed calls: " << m_failedCalls << "\n";
    oss << "  Success rate: " << (m_functionCalls > 0 ? (m_successfulCalls * 100.0 / m_functionCalls) : 0) << "%\n";
    return oss.str();
}

void CJMODAPI::initializeDefaultFunctions() {
    addVIRFunctions();
    addLISTENFunctions();
    addANIMATEFunctions();
    addROUTERFunctions();
    addDELEGATEFunctions();
    addFILELOADERFunctions();
}

void CJMODAPI::addVIRFunctions() {
    // vir element
    CJMODAPIFunction virElement("vir", CJMODAPIType::VIR, "Create virtual element");
    virElement.parameters = {
        {"element", CJMODAPIParamType::STRING, "", true, "Element name"},
        {"attributes", CJMODAPIParamType::OBJECT, "{}", false, "Element attributes"},
        {"children", CJMODAPIParamType::ARRAY, "[]", false, "Element children"}
    };
    virElement.returnType = "string";
    virElement.handler = [](const std::vector<std::string>& args) -> std::string {
        if (args.empty()) return "";
        std::ostringstream oss;
        oss << "<" << args[0];
        if (args.size() > 1 && !args[1].empty()) {
            oss << " " << args[1];
        }
        oss << ">";
        if (args.size() > 2 && !args[2].empty()) {
            oss << args[2];
        }
        oss << "</" << args[0] << ">";
        return oss.str();
    };
    registerFunction(virElement);
}

void CJMODAPI::addLISTENFunctions() {
    // listen event
    CJMODAPIFunction listenEvent("listen", CJMODAPIType::LISTEN, "Add event listener");
    listenEvent.parameters = {
        {"event", CJMODAPIParamType::STRING, "", true, "Event type"},
        {"handler", CJMODAPIParamType::FUNCTION, "", true, "Event handler function"},
        {"element", CJMODAPIParamType::STRING, "document", false, "Target element"}
    };
    listenEvent.returnType = "void";
    listenEvent.handler = [](const std::vector<std::string>& args) -> std::string {
        if (args.size() < 2) return "";
        std::ostringstream oss;
        oss << "addEventListener('" << args[0] << "', " << args[1] << ");";
        return oss.str();
    };
    registerFunction(listenEvent);
}

void CJMODAPI::addANIMATEFunctions() {
    // animate element
    CJMODAPIFunction animateElement("animate", CJMODAPIType::ANIMATE, "Animate element");
    animateElement.parameters = {
        {"element", CJMODAPIParamType::STRING, "", true, "Element selector"},
        {"properties", CJMODAPIParamType::OBJECT, "", true, "Animation properties"},
        {"duration", CJMODAPIParamType::NUMBER, "1000", false, "Animation duration in ms"},
        {"easing", CJMODAPIParamType::STRING, "ease", false, "Animation easing"}
    };
    animateElement.returnType = "void";
    animateElement.handler = [](const std::vector<std::string>& args) -> std::string {
        if (args.size() < 2) return "";
        std::ostringstream oss;
        oss << "animateElement('" << args[0] << "', " << args[1];
        if (args.size() > 2) {
            oss << ", " << args[2];
        }
        if (args.size() > 3) {
            oss << ", '" << args[3] << "'";
        }
        oss << ");";
        return oss.str();
    };
    registerFunction(animateElement);
}

void CJMODAPI::addROUTERFunctions() {
    // router navigate
    CJMODAPIFunction routerNavigate("navigate", CJMODAPIType::ROUTER, "Navigate to route");
    routerNavigate.parameters = {
        {"route", CJMODAPIParamType::STRING, "", true, "Route path"},
        {"params", CJMODAPIParamType::OBJECT, "{}", false, "Route parameters"},
        {"replace", CJMODAPIParamType::BOOLEAN, "false", false, "Replace current route"}
    };
    routerNavigate.returnType = "void";
    routerNavigate.handler = [](const std::vector<std::string>& args) -> std::string {
        if (args.empty()) return "";
        std::ostringstream oss;
        oss << "navigateToRoute('" << args[0] << "'";
        if (args.size() > 1) {
            oss << ", " << args[1];
        }
        if (args.size() > 2) {
            oss << ", " << args[2];
        }
        oss << ");";
        return oss.str();
    };
    registerFunction(routerNavigate);
}

void CJMODAPI::addDELEGATEFunctions() {
    // delegate event
    CJMODAPIFunction delegateEvent("delegate", CJMODAPIType::DELEGATE, "Delegate event handling");
    delegateEvent.parameters = {
        {"selector", CJMODAPIParamType::STRING, "", true, "CSS selector"},
        {"event", CJMODAPIParamType::STRING, "", true, "Event type"},
        {"handler", CJMODAPIParamType::FUNCTION, "", true, "Event handler function"},
        {"parent", CJMODAPIParamType::STRING, "document", false, "Parent element"}
    };
    delegateEvent.returnType = "void";
    delegateEvent.handler = [](const std::vector<std::string>& args) -> std::string {
        if (args.size() < 3) return "";
        std::ostringstream oss;
        oss << "delegateEvent('" << args[0] << "', '" << args[1] << "', " << args[2];
        if (args.size() > 3) {
            oss << ", '" << args[3] << "'";
        }
        oss << ");";
        return oss.str();
    };
    registerFunction(delegateEvent);
}

void CJMODAPI::addFILELOADERFunctions() {
    // load file
    CJMODAPIFunction loadFile("load", CJMODAPIType::FILELOADER, "Load file content");
    loadFile.parameters = {
        {"url", CJMODAPIParamType::STRING, "", true, "File URL"},
        {"type", CJMODAPIParamType::STRING, "text", false, "File type"},
        {"callback", CJMODAPIParamType::FUNCTION, "", false, "Callback function"}
    };
    loadFile.returnType = "Promise";
    loadFile.handler = [](const std::vector<std::string>& args) -> std::string {
        if (args.empty()) return "";
        std::ostringstream oss;
        oss << "loadFile('" << args[0] << "'";
        if (args.size() > 1) {
            oss << ", '" << args[1] << "'";
        }
        if (args.size() > 2) {
            oss << ", " << args[2];
        }
        oss << ");";
        return oss.str();
    };
    registerFunction(loadFile);
}

std::string CJMODAPI::getParamTypeName(CJMODAPIParamType type) {
    switch (type) {
        case CJMODAPIParamType::STRING:
            return "string";
        case CJMODAPIParamType::NUMBER:
            return "number";
        case CJMODAPIParamType::BOOLEAN:
            return "boolean";
        case CJMODAPIParamType::OBJECT:
            return "object";
        case CJMODAPIParamType::ARRAY:
            return "array";
        case CJMODAPIParamType::FUNCTION:
            return "function";
        case CJMODAPIParamType::UNKNOWN:
        default:
            return "unknown";
    }
}

std::string CJMODAPI::getAPITypeName(CJMODAPIType type) {
    switch (type) {
        case CJMODAPIType::VIR:
            return "VIR";
        case CJMODAPIType::LISTEN:
            return "LISTEN";
        case CJMODAPIType::ANIMATE:
            return "ANIMATE";
        case CJMODAPIType::ROUTER:
            return "ROUTER";
        case CJMODAPIType::DELEGATE:
            return "DELEGATE";
        case CJMODAPIType::FILELOADER:
            return "FILELOADER";
        case CJMODAPIType::UNKNOWN:
        default:
            return "UNKNOWN";
    }
}

} // namespace CHTL