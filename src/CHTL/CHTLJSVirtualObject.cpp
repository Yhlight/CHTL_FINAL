#include "CHTL/CHTLJS.h"
#include <sstream>
#include <regex>
#include <algorithm>

namespace CHTL {

// CHTLJSVirtualObject 实现
CHTLJSVirtualObject::CHTLJSVirtualObject(std::shared_ptr<CHTLContext> context)
    : m_context(context) {
}

std::string CHTLJSVirtualObject::parseVirtualObject(const std::string& objectName, const std::string& function) const {
    if (!isValidVirtualObjectName(objectName)) {
        m_context->addError("Invalid virtual object name: " + objectName);
        return "";
    }
    
    auto properties = parseVirtualObjectProperties(function);
    return generateVirtualObjectCode(objectName, function);
}

std::unordered_map<std::string, std::string> CHTLJSVirtualObject::parseVirtualObjectProperties(const std::string& function) const {
    std::unordered_map<std::string, std::string> properties;
    
    // 解析虚对象属性
    std::regex propertyRegex(R"(\s*(\w+)\s*:\s*([^,;]+)(?:[,;]|$))");
    std::sregex_iterator iter(function.begin(), function.end(), propertyRegex);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        std::smatch match = *iter;
        std::string key = match[1].str();
        std::string value = match[2].str();
        
        // 去除前后空格
        value.erase(0, value.find_first_not_of(" \t"));
        value.erase(value.find_last_not_of(" \t") + 1);
        
        properties[key] = value;
    }
    
    return properties;
}

std::string CHTLJSVirtualObject::generateVirtualObjectCode(const std::string& objectName, const std::string& function) const {
    std::stringstream ss;
    
    ss << "const " << objectName << " = {\n";
    ss << "  // 虚对象元信息\n";
    ss << "  __meta: {\n";
    ss << "    name: '" << objectName << "',\n";
    ss << "    type: 'virtual',\n";
    ss << "    created: new Date().toISOString()\n";
    ss << "  },\n\n";
    
    ss << "  // 虚对象函数\n";
    ss << "  function: " << function << ",\n\n";
    
    ss << "  // 元信息访问方法\n";
    ss << "  getMeta: function() {\n";
    ss << "    return this.__meta;\n";
    ss << "  },\n\n";
    
    ss << "  // 函数元信息\n";
    ss << "  getFunctionMeta: function() {\n";
    ss << "    return {\n";
    ss << "      name: this.function.name || 'anonymous',\n";
    ss << "      length: this.function.length,\n";
    ss << "      toString: this.function.toString()\n";
    ss << "    };\n";
    ss << "  },\n\n";
    
    ss << "  // 执行函数\n";
    ss << "  execute: function(...args) {\n";
    ss << "    return this.function.apply(this, args);\n";
    ss << "  },\n\n";
    
    ss << "  // 绑定虚对象\n";
    ss << "  bindVirtualObject: function(virtualObject) {\n";
    ss << "    this.__virtualObject = virtualObject;\n";
    ss << "    return this;\n";
    ss << "  },\n\n";
    
    ss << "  // 获取绑定的虚对象\n";
    ss << "  getVirtualObject: function() {\n";
    ss << "    return this.__virtualObject;\n";
    ss << "  }\n";
    ss << "};\n";
    
    return ss.str();
}

std::string CHTLJSVirtualObject::generateVirtualObjectFunction(const std::unordered_map<std::string, std::string>& properties) const {
    std::stringstream ss;
    
    ss << "function virtualObjectFunction() {\n";
    ss << "  const self = this;\n\n";
    
    for (const auto& [key, value] : properties) {
        ss << "  self." << key << " = " << value << ";\n";
    }
    
    ss << "  return self;\n";
    ss << "}\n";
    
    return ss.str();
}

std::string CHTLJSVirtualObject::generateMetaAccessCode(const std::string& objectName) const {
    std::stringstream ss;
    
    ss << "// 访问 " << objectName << " 的元信息\n";
    ss << "const " << objectName << "Meta = " << objectName << ".getMeta();\n";
    ss << "console.log('Object name:', " << objectName << "Meta.name);\n";
    ss << "console.log('Object type:', " << objectName << "Meta.type);\n";
    ss << "console.log('Created at:', " << objectName << "Meta.created);\n";
    
    return ss.str();
}

std::string CHTLJSVirtualObject::generateFunctionMetaCode(const std::string& functionName) const {
    std::stringstream ss;
    
    ss << "// 访问 " << functionName << " 的函数元信息\n";
    ss << "const " << functionName << "FunctionMeta = " << functionName << ".getFunctionMeta();\n";
    ss << "console.log('Function name:', " << functionName << "FunctionMeta.name);\n";
    ss << "console.log('Function length:', " << functionName << "FunctionMeta.length);\n";
    ss << "console.log('Function source:', " << functionName << "FunctionMeta.toString);\n";
    
    return ss.str();
}

bool CHTLJSVirtualObject::isValidVirtualObjectName(const std::string& name) const {
    if (name.empty()) {
        return false;
    }
    
    // 检查名称是否符合 JavaScript 标识符规范
    std::regex identifierRegex(R"(^[a-zA-Z_$][a-zA-Z0-9_$]*$)");
    return std::regex_match(name, identifierRegex);
}

std::string CHTLJSVirtualObject::generateMetaPropertyAccess(const std::string& property) const {
    std::stringstream ss;
    
    ss << "// 访问元属性: " << property << "\n";
    ss << "const metaValue = virtualObject.__meta." << property << ";\n";
    ss << "console.log('Meta property " << property << ":', metaValue);\n";
    
    return ss.str();
}

} // namespace CHTL