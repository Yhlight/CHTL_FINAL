#include "CHTL/CHTLCJMODAPI.h"
#include <fstream>
#include <sstream>
#include <regex>
#include <algorithm>

namespace CHTL {

// CJMODGenerator 实现
CJMODGenerator::CJMODGenerator(std::shared_ptr<CHTLContext> context)
    : m_context(context) {
}

std::shared_ptr<CodeGenerationResult> CJMODGenerator::generate(const std::string& template_, 
                                                               const std::unordered_map<std::string, std::any>& parameters) {
    auto result = std::make_shared<CodeGenerationResult>(true);
    
    try {
        std::string generatedCode = processTemplate(template_, parameters);
        result->setGeneratedCode(generatedCode);
    } catch (const std::exception& e) {
        result = std::make_shared<CodeGenerationResult>(false, e.what());
    }
    
    return result;
}

std::shared_ptr<CodeGenerationResult> CJMODGenerator::generateFromFile(const std::string& templateFile, 
                                                                       const std::unordered_map<std::string, std::any>& parameters) {
    auto result = std::make_shared<CodeGenerationResult>(true);
    
    try {
        std::string generatedCode = processTemplateFile(templateFile, parameters);
        result->setGeneratedCode(generatedCode);
    } catch (const std::exception& e) {
        result = std::make_shared<CodeGenerationResult>(false, e.what());
    }
    
    return result;
}

std::string CJMODGenerator::exportResult(const std::shared_ptr<CodeGenerationResult>& result) const {
    if (!result || !result->isSuccess()) {
        return "";
    }
    
    return result->getGeneratedCode();
}

bool CJMODGenerator::exportResultToFile(const std::shared_ptr<CodeGenerationResult>& result, const std::string& filePath) const {
    if (!result || !result->isSuccess()) {
        return false;
    }
    
    std::ofstream file(filePath);
    if (!file.is_open()) {
        m_context->addError("Cannot create file: " + filePath);
        return false;
    }
    
    file << result->getGeneratedCode();
    file.close();
    
    return true;
}

bool CJMODGenerator::exportResultToFiles(const std::shared_ptr<CodeGenerationResult>& result, const std::string& directoryPath) const {
    if (!result || !result->isSuccess()) {
        return false;
    }
    
    try {
        std::filesystem::create_directories(directoryPath);
        
        for (const auto& [filename, content] : result->getGeneratedFiles()) {
            std::string filePath = directoryPath + "/" + filename;
            std::ofstream file(filePath);
            if (file.is_open()) {
                file << content;
                file.close();
            }
        }
        
        return true;
    } catch (const std::exception& e) {
        m_context->addError("Error creating files: " + std::string(e.what()));
        return false;
    }
}

std::string CJMODGenerator::processTemplate(const std::string& template_, const std::unordered_map<std::string, std::any>& parameters) const {
    std::string result = template_;
    
    // 处理占位符
    result = replacePlaceholders(result, parameters);
    
    // 处理条件语句
    result = processConditionals(result, parameters);
    
    // 处理循环
    result = processLoops(result, parameters);
    
    // 处理包含
    result = processIncludes(result, parameters);
    
    return result;
}

std::string CJMODGenerator::processTemplateFile(const std::string& templateFile, const std::unordered_map<std::string, std::any>& parameters) const {
    std::ifstream file(templateFile);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open template file: " + templateFile);
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string template_ = buffer.str();
    
    return processTemplate(template_, parameters);
}

std::string CJMODGenerator::optimizeCode(const std::string& code) const {
    std::string result = code;
    
    // 去除多余的空行
    std::regex multipleNewlines(R"(\n\s*\n\s*\n)");
    result = std::regex_replace(result, multipleNewlines, "\n\n");
    
    // 去除多余的空格
    std::regex multipleSpaces(R"(\s{2,})");
    result = std::regex_replace(result, multipleSpaces, " ");
    
    // 优化分号
    std::regex semicolonSpaces(R"(\s*;\s*)");
    result = std::regex_replace(result, semicolonSpaces, ";");
    
    return result;
}

std::string CJMODGenerator::minifyCode(const std::string& code) const {
    std::string result = code;
    
    // 去除注释
    std::regex commentRegex(R"(//.*$|/\*[\s\S]*?\*/)");
    result = std::regex_replace(result, commentRegex, "");
    
    // 去除多余的空格和换行
    std::regex whitespaceRegex(R"(\s+)");
    result = std::regex_replace(result, whitespaceRegex, " ");
    
    // 去除分号前的空格
    std::regex semicolonRegex(R"(\s*;\s*)");
    result = std::regex_replace(result, semicolonRegex, ";");
    
    return result;
}

std::string CJMODGenerator::beautifyCode(const std::string& code) const {
    std::string result = code;
    
    // 添加适当的缩进
    std::stringstream ss(result);
    std::string line;
    std::stringstream output;
    int indentLevel = 0;
    
    while (std::getline(ss, line)) {
        // 计算缩进
        std::string indent(indentLevel * 2, ' ');
        output << indent << line << "\n";
        
        // 更新缩进级别
        for (char c : line) {
            if (c == '{') {
                indentLevel++;
            } else if (c == '}') {
                indentLevel--;
                if (indentLevel < 0) indentLevel = 0;
            }
        }
    }
    
    return output.str();
}

std::string CJMODGenerator::replacePlaceholders(const std::string& template_, const std::unordered_map<std::string, std::any>& parameters) const {
    std::string result = template_;
    
    for (const auto& [key, value] : parameters) {
        std::string placeholder = "${" + key + "}";
        std::string valueStr = formatValue(value);
        
        size_t pos = 0;
        while ((pos = result.find(placeholder, pos)) != std::string::npos) {
            result.replace(pos, placeholder.length(), valueStr);
            pos += valueStr.length();
        }
    }
    
    return result;
}

std::string CJMODGenerator::processConditionals(const std::string& template_, const std::unordered_map<std::string, std::any>& parameters) const {
    std::string result = template_;
    
    // 处理 if 条件
    std::regex ifRegex(R"(\{#if\s+(\w+)\}(.*?)\{#endif\})");
    std::smatch match;
    
    while (std::regex_search(result, match, ifRegex)) {
        std::string condition = match[1].str();
        std::string content = match[2].str();
        
        auto it = parameters.find(condition);
        if (it != parameters.end()) {
            std::string value = formatValue(it->second);
            if (!value.empty() && value != "false" && value != "0") {
                result.replace(match.position(), match.length(), content);
            } else {
                result.replace(match.position(), match.length(), "");
            }
        } else {
            result.replace(match.position(), match.length(), "");
        }
    }
    
    return result;
}

std::string CJMODGenerator::processLoops(const std::string& template_, const std::unordered_map<std::string, std::any>& parameters) const {
    std::string result = template_;
    
    // 处理 for 循环
    std::regex forRegex(R"(\{#for\s+(\w+)\s+in\s+(\w+)\}(.*?)\{#endfor\})");
    std::smatch match;
    
    while (std::regex_search(result, match, forRegex)) {
        std::string item = match[1].str();
        std::string array = match[2].str();
        std::string content = match[3].str();
        
        auto it = parameters.find(array);
        if (it != parameters.end()) {
            try {
                auto arrayValue = std::any_cast<std::vector<std::string>>(it->second);
                std::string loopContent;
                
                for (const auto& arrayItem : arrayValue) {
                    std::string itemContent = content;
                    std::string itemPlaceholder = "${" + item + "}";
                    
                    size_t pos = 0;
                    while ((pos = itemContent.find(itemPlaceholder, pos)) != std::string::npos) {
                        itemContent.replace(pos, itemPlaceholder.length(), arrayItem);
                        pos += arrayItem.length();
                    }
                    
                    loopContent += itemContent;
                }
                
                result.replace(match.position(), match.length(), loopContent);
            } catch (const std::bad_any_cast&) {
                result.replace(match.position(), match.length(), "");
            }
        } else {
            result.replace(match.position(), match.length(), "");
        }
    }
    
    return result;
}

std::string CJMODGenerator::processIncludes(const std::string& template_, const std::unordered_map<std::string, std::any>& parameters) const {
    std::string result = template_;
    
    // 处理 include 指令
    std::regex includeRegex(R"(\{#include\s+(\w+)\})");
    std::smatch match;
    
    while (std::regex_search(result, match, includeRegex)) {
        std::string includeName = match[1].str();
        
        auto it = parameters.find(includeName);
        if (it != parameters.end()) {
            std::string includeContent = formatValue(it->second);
            result.replace(match.position(), match.length(), includeContent);
        } else {
            result.replace(match.position(), match.length(), "");
        }
    }
    
    return result;
}

std::string CJMODGenerator::formatValue(const std::any& value) const {
    try {
        if (value.type() == typeid(std::string)) {
            return std::any_cast<std::string>(value);
        } else if (value.type() == typeid(int)) {
            return std::to_string(std::any_cast<int>(value));
        } else if (value.type() == typeid(double)) {
            return std::to_string(std::any_cast<double>(value));
        } else if (value.type() == typeid(bool)) {
            return std::any_cast<bool>(value) ? "true" : "false";
        } else {
            return "";
        }
    } catch (const std::bad_any_cast&) {
        return "";
    }
}

// AtomArg 实现
AtomArg::AtomArg(const std::string& placeholder, const std::string& type)
    : m_placeholder(placeholder), m_type(type), m_isRequired(true) {
}

bool AtomArg::isVariable() const {
    return m_placeholder.starts_with("$") && !m_placeholder.starts_with("$?") && !m_placeholder.starts_with("$!");
}

bool AtomArg::isOptional() const {
    return m_placeholder.starts_with("$?");
}

bool AtomArg::isRequired() const {
    return m_placeholder.starts_with("$") && !m_placeholder.starts_with("$?");
}

bool AtomArg::isWildcard() const {
    return m_placeholder == "...";
}

bool AtomArg::isSpecial() const {
    return m_placeholder.starts_with("$_") || m_placeholder.starts_with("$!");
}

// CHTLJSFunction 实现
CHTLJSFunction::CHTLJSFunction(const std::string& name, const std::string& signature, const std::string& body)
    : m_name(name), m_signature(signature), m_body(body) {
    m_parameters = parseParameters(signature);
}

std::string CHTLJSFunction::call(const std::vector<std::string>& arguments) const {
    return generateFunctionCall(arguments);
}

std::string CHTLJSFunction::bind(const std::unordered_map<std::string, std::string>& bindings) const {
    std::string result = m_body;
    
    for (const auto& [key, value] : bindings) {
        std::string placeholder = "${" + key + "}";
        size_t pos = 0;
        while ((pos = result.find(placeholder, pos)) != std::string::npos) {
            result.replace(pos, placeholder.length(), value);
            pos += value.length();
        }
    }
    
    return result;
}

std::string CHTLJSFunction::createVirtualObject() const {
    return generateVirtualObjectCode();
}

bool CHTLJSFunction::validate() const {
    return !m_name.empty() && !m_signature.empty() && !m_body.empty();
}

bool CHTLJSFunction::validateParameters(const std::vector<std::string>& arguments) const {
    return m_parameters.size() == arguments.size();
}

std::vector<std::string> CHTLJSFunction::parseParameters(const std::string& signature) const {
    std::vector<std::string> parameters;
    
    size_t start = signature.find('(');
    if (start == std::string::npos) {
        return parameters;
    }
    
    size_t end = signature.find(')', start);
    if (end == std::string::npos) {
        return parameters;
    }
    
    std::string paramsStr = signature.substr(start + 1, end - start - 1);
    std::stringstream ss(paramsStr);
    std::string param;
    
    while (std::getline(ss, param, ',')) {
        std::string trimmed = param;
        trimmed.erase(0, trimmed.find_first_not_of(" \t"));
        trimmed.erase(trimmed.find_last_not_of(" \t") + 1);
        
        if (!trimmed.empty()) {
            parameters.push_back(trimmed);
        }
    }
    
    return parameters;
}

std::string CHTLJSFunction::generateFunctionCall(const std::vector<std::string>& arguments) const {
    std::stringstream ss;
    ss << m_name << "(";
    
    for (size_t i = 0; i < arguments.size(); ++i) {
        if (i > 0) {
            ss << ", ";
        }
        ss << arguments[i];
    }
    
    ss << ")";
    return ss.str();
}

std::string CHTLJSFunction::generateVirtualObjectCode() const {
    std::stringstream ss;
    
    ss << "const " << m_name << " = {\n";
    ss << "  __meta: {\n";
    ss << "    name: '" << m_name << "',\n";
    ss << "    type: 'function',\n";
    ss << "    signature: '" << m_signature << "',\n";
    ss << "    created: new Date().toISOString()\n";
    ss << "  },\n";
    ss << "  function: " << m_body << ",\n";
    ss << "  call: function(...args) {\n";
    ss << "    return this.function.apply(this, args);\n";
    ss << "  },\n";
    ss << "  bind: function(bindings) {\n";
    ss << "    return this.function.bind(bindings);\n";
    ss << "  }\n";
    ss << "};\n";
    
    return ss.str();
}

} // namespace CHTL