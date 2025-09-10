#include "UseNode.h"
#include <algorithm>
#include <sstream>
#include <regex>
#include <cctype>

namespace CHTL {

UseNode::UseNode(const std::string& name, UseType type)
    : BaseNode(NodeType::USE, name, ""), useType(type), useName(name),
      isActive(true), isProcessed(false), isValid(true), priority(0), isRequired(false),
      isOptional(false), isInherited(false) {
}

std::string UseNode::getUseTypeName() const {
    switch (useType) {
        case UseType::HTML5: return "html5";
        case UseType::CONFIGURATION: return "configuration";
        case UseType::MODULE: return "module";
        case UseType::TEMPLATE: return "template";
        case UseType::CUSTOM: return "custom";
        default: return "unknown";
    }
}

void UseNode::setUseParameter(const std::string& key, const std::string& value) {
    useParameters[key] = value;
}

std::string UseNode::getUseParameter(const std::string& key) const {
    auto it = useParameters.find(key);
    return (it != useParameters.end()) ? it->second : "";
}

bool UseNode::hasUseParameter(const std::string& key) const {
    return useParameters.find(key) != useParameters.end();
}

void UseNode::removeUseParameter(const std::string& key) {
    useParameters.erase(key);
}

void UseNode::setUseArray(const std::string& key, const std::vector<std::string>& values) {
    useArrays[key] = values;
}

std::vector<std::string> UseNode::getUseArray(const std::string& key) const {
    auto it = useArrays.find(key);
    return (it != useArrays.end()) ? it->second : std::vector<std::string>();
}

bool UseNode::hasUseArray(const std::string& key) const {
    return useArrays.find(key) != useArrays.end();
}

void UseNode::removeUseArray(const std::string& key) {
    useArrays.erase(key);
}

void UseNode::addUseArrayItem(const std::string& key, const std::string& item) {
    useArrays[key].push_back(item);
}

void UseNode::removeUseArrayItem(const std::string& key, const std::string& item) {
    auto it = useArrays.find(key);
    if (it != useArrays.end()) {
        auto& array = it->second;
        array.erase(std::remove(array.begin(), array.end(), item), array.end());
    }
}

void UseNode::setUseObject(const std::string& key, const std::map<std::string, std::string>& object) {
    useObjects[key] = object;
}

std::map<std::string, std::string> UseNode::getUseObject(const std::string& key) const {
    auto it = useObjects.find(key);
    return (it != useObjects.end()) ? it->second : std::map<std::string, std::string>();
}

bool UseNode::hasUseObject(const std::string& key) const {
    return useObjects.find(key) != useObjects.end();
}

void UseNode::removeUseObject(const std::string& key) {
    useObjects.erase(key);
}

void UseNode::setUseObjectProperty(const std::string& key, const std::string& property, const std::string& value) {
    useObjects[key][property] = value;
}

std::string UseNode::getUseObjectProperty(const std::string& key, const std::string& property) const {
    auto it = useObjects.find(key);
    if (it != useObjects.end()) {
        auto propIt = it->second.find(property);
        return (propIt != it->second.end()) ? propIt->second : "";
    }
    return "";
}

void UseNode::removeUseObjectProperty(const std::string& key, const std::string& property) {
    auto it = useObjects.find(key);
    if (it != useObjects.end()) {
        it->second.erase(property);
    }
}

void UseNode::setUseOption(const std::string& key, bool value) {
    useOptions[key] = value;
}

bool UseNode::getUseOption(const std::string& key) const {
    auto it = useOptions.find(key);
    return (it != useOptions.end()) ? it->second : false;
}

bool UseNode::hasUseOption(const std::string& key) const {
    return useOptions.find(key) != useOptions.end();
}

void UseNode::removeUseOption(const std::string& key) {
    useOptions.erase(key);
}

void UseNode::setUseLimit(const std::string& key, int value) {
    useLimits[key] = value;
}

int UseNode::getUseLimit(const std::string& key) const {
    auto it = useLimits.find(key);
    return (it != useLimits.end()) ? it->second : 0;
}

bool UseNode::hasUseLimit(const std::string& key) const {
    return useLimits.find(key) != useLimits.end();
}

void UseNode::removeUseLimit(const std::string& key) {
    useLimits.erase(key);
}

void UseNode::setUseDefault(const std::string& key, const std::string& value) {
    useDefaults[key] = value;
}

std::string UseNode::getUseDefault(const std::string& key) const {
    auto it = useDefaults.find(key);
    return (it != useDefaults.end()) ? it->second : "";
}

bool UseNode::hasUseDefault(const std::string& key) const {
    return useDefaults.find(key) != useDefaults.end();
}

void UseNode::removeUseDefault(const std::string& key) {
    useDefaults.erase(key);
}

void UseNode::addUseRule(const std::string& rule) {
    if (std::find(useRules.begin(), useRules.end(), rule) == useRules.end()) {
        useRules.push_back(rule);
    }
}

void UseNode::removeUseRule(const std::string& rule) {
    useRules.erase(std::remove(useRules.begin(), useRules.end(), rule), useRules.end());
}

bool UseNode::hasUseRule(const std::string& rule) const {
    return std::find(useRules.begin(), useRules.end(), rule) != useRules.end();
}

void UseNode::setRuleValue(const std::string& rule, const std::string& value) {
    ruleValues[rule] = value;
}

std::string UseNode::getRuleValue(const std::string& rule) const {
    auto it = ruleValues.find(rule);
    return (it != ruleValues.end()) ? it->second : "";
}

bool UseNode::hasRuleValue(const std::string& rule) const {
    return ruleValues.find(rule) != ruleValues.end();
}

void UseNode::removeRuleValue(const std::string& rule) {
    ruleValues.erase(rule);
}

void UseNode::setRuleFunction(const std::string& rule, std::function<bool(const std::string&)> function) {
    ruleFunctions[rule] = function;
}

std::function<bool(const std::string&)> UseNode::getRuleFunction(const std::string& rule) const {
    auto it = ruleFunctions.find(rule);
    return (it != ruleFunctions.end()) ? it->second : nullptr;
}

bool UseNode::hasRuleFunction(const std::string& rule) const {
    return ruleFunctions.find(rule) != ruleFunctions.end();
}

void UseNode::removeRuleFunction(const std::string& rule) {
    ruleFunctions.erase(rule);
}

void UseNode::addUseError(const std::string& error) {
    useErrors.push_back(error);
}

void UseNode::addUseWarning(const std::string& warning) {
    useWarnings.push_back(warning);
}

void UseNode::clearUseMessages() {
    useErrors.clear();
    useWarnings.clear();
}

void UseNode::setMetadata(const std::string& key, const std::string& value) {
    metadata[key] = value;
}

std::string UseNode::getMetadata(const std::string& key) const {
    auto it = metadata.find(key);
    return (it != metadata.end()) ? it->second : "";
}

bool UseNode::hasMetadata(const std::string& key) const {
    return metadata.find(key) != metadata.end();
}

void UseNode::removeMetadata(const std::string& key) {
    metadata.erase(key);
}

bool UseNode::processUse() {
    if (!isActive) {
        return true;
    }
    
    try {
        processUseParameters();
        processUseOptions();
        processUseLimits();
        processUseDefaults();
        processUseRules();
        
        isProcessed = true;
        return true;
    } catch (const std::exception& e) {
        handleUseError("处理use时发生错误: " + std::string(e.what()));
        return false;
    }
}

bool UseNode::processHTML5Use() {
    if (useType != UseType::HTML5) {
        return false;
    }
    
    return processUse();
}

bool UseNode::processConfigurationUse() {
    if (useType != UseType::CONFIGURATION) {
        return false;
    }
    
    return processUse();
}

bool UseNode::processModuleUse() {
    if (useType != UseType::MODULE) {
        return false;
    }
    
    return processUse();
}

bool UseNode::processTemplateUse() {
    if (useType != UseType::TEMPLATE) {
        return false;
    }
    
    return processUse();
}

bool UseNode::processCustomUse() {
    if (useType != UseType::CUSTOM) {
        return false;
    }
    
    return processUse();
}

bool UseNode::validateUse() const {
    return validateParameters() && validateOptions() && validateLimits() && validateDefaults() && validateRules();
}

bool UseNode::validateParameters() const {
    for (const auto& param : useParameters) {
        if (param.first.empty() || param.second.empty()) {
            return false;
        }
    }
    return true;
}

bool UseNode::validateOptions() const {
    for (const auto& option : useOptions) {
        if (option.first.empty()) {
            return false;
        }
    }
    return true;
}

bool UseNode::validateLimits() const {
    for (const auto& limit : useLimits) {
        if (limit.first.empty() || limit.second < 0) {
            return false;
        }
    }
    return true;
}

bool UseNode::validateDefaults() const {
    for (const auto& def : useDefaults) {
        if (def.first.empty() || def.second.empty()) {
            return false;
        }
    }
    return true;
}

bool UseNode::validateRules() const {
    for (const auto& rule : useRules) {
        if (rule.empty()) {
            return false;
        }
    }
    return true;
}

bool UseNode::applyUse() {
    if (!isProcessed) {
        return false;
    }
    
    try {
        applyUseParameters();
        applyUseOptions();
        applyUseLimits();
        applyUseDefaults();
        applyUseRules();
        
        return true;
    } catch (const std::exception& e) {
        handleUseError("应用use时发生错误: " + std::string(e.what()));
        return false;
    }
}

bool UseNode::applyUseRules() {
    for (const auto& rule : useRules) {
        if (hasRuleFunction(rule)) {
            auto func = getRuleFunction(rule);
            if (func && !func(rule)) {
                return false;
            }
        }
    }
    return true;
}

bool UseNode::applyUseOptions() {
    for (const auto& option : useOptions) {
        // 应用use选项
    }
    return true;
}

bool UseNode::applyUseLimits() {
    for (const auto& limit : useLimits) {
        // 应用use限制
    }
    return true;
}

bool UseNode::applyUseDefaults() {
    for (const auto& def : useDefaults) {
        // 应用use默认值
    }
    return true;
}

std::string UseNode::toHTML() const {
    std::ostringstream oss;
    oss << "<!-- Use: " << useName << " -->\n";
    oss << "<!-- Type: " << getUseTypeName() << " -->\n";
    oss << "<!-- Group: " << useGroup << " -->\n";
    oss << "<!-- Version: " << useVersion << " -->\n";
    
    if (!useParameters.empty()) {
        oss << "<!-- Parameters: ";
        for (const auto& param : useParameters) {
            oss << param.first << "=" << param.second << " ";
        }
        oss << "-->\n";
    }
    
    if (!useRules.empty()) {
        oss << "<!-- Rules: ";
        for (const auto& rule : useRules) {
            oss << rule << " ";
        }
        oss << "-->\n";
    }
    
    return oss.str();
}

std::string UseNode::toCSS() const {
    std::ostringstream oss;
    oss << "/* Use: " << useName << " */\n";
    oss << "/* Type: " << getUseTypeName() << " */\n";
    oss << "/* Group: " << useGroup << " */\n";
    oss << "/* Version: " << useVersion << " */\n";
    
    if (!useParameters.empty()) {
        oss << "/* Parameters: ";
        for (const auto& param : useParameters) {
            oss << param.first << "=" << param.second << " ";
        }
        oss << "*/\n";
    }
    
    return oss.str();
}

std::string UseNode::toJavaScript() const {
    std::ostringstream oss;
    oss << "// Use: " << useName << "\n";
    oss << "// Type: " << getUseTypeName() << "\n";
    oss << "// Group: " << useGroup << "\n";
    oss << "// Version: " << useVersion << "\n";
    
    if (!useParameters.empty()) {
        oss << "// Parameters: ";
        for (const auto& param : useParameters) {
            oss << param.first << "=" << param.second << " ";
        }
        oss << "\n";
    }
    
    return oss.str();
}

std::string UseNode::toUseString() const {
    std::ostringstream oss;
    oss << "use " << useName << " {\n";
    oss << "  type: " << getUseTypeName() << ";\n";
    oss << "  group: " << useGroup << ";\n";
    oss << "  version: " << useVersion << ";\n";
    
    if (!useParameters.empty()) {
        oss << "  parameters: {\n";
        for (const auto& param : useParameters) {
            oss << "    " << param.first << ": \"" << param.second << "\";\n";
        }
        oss << "  }\n";
    }
    
    if (!useArrays.empty()) {
        oss << "  arrays: {\n";
        for (const auto& array : useArrays) {
            oss << "    " << array.first << ": [";
            for (size_t i = 0; i < array.second.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << "\"" << array.second[i] << "\"";
            }
            oss << "];\n";
        }
        oss << "  }\n";
    }
    
    if (!useObjects.empty()) {
        oss << "  objects: {\n";
        for (const auto& obj : useObjects) {
            oss << "    " << obj.first << ": {\n";
            for (const auto& prop : obj.second) {
                oss << "      " << prop.first << ": \"" << prop.second << "\";\n";
            }
            oss << "    }\n";
        }
        oss << "  }\n";
    }
    
    if (!useOptions.empty()) {
        oss << "  options: {\n";
        for (const auto& option : useOptions) {
            oss << "    " << option.first << ": " << (option.second ? "true" : "false") << ";\n";
        }
        oss << "  }\n";
    }
    
    if (!useLimits.empty()) {
        oss << "  limits: {\n";
        for (const auto& limit : useLimits) {
            oss << "    " << limit.first << ": " << limit.second << ";\n";
        }
        oss << "  }\n";
    }
    
    if (!useDefaults.empty()) {
        oss << "  defaults: {\n";
        for (const auto& def : useDefaults) {
            oss << "    " << def.first << ": \"" << def.second << "\";\n";
        }
        oss << "  }\n";
    }
    
    if (!useRules.empty()) {
        oss << "  rules: [";
        for (size_t i = 0; i < useRules.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << "\"" << useRules[i] << "\"";
        }
        oss << "];\n";
    }
    
    oss << "}\n";
    return oss.str();
}

std::string UseNode::formatUse() const {
    return toUseString();
}

std::string UseNode::minifyUse() const {
    std::string result = toUseString();
    
    // 移除多余的空格和换行
    result = std::regex_replace(result, std::regex("\\s+"), " ");
    result = std::regex_replace(result, std::regex("\\s*\\{\\s*"), "{");
    result = std::regex_replace(result, std::regex("\\s*\\}\\s*"), "}");
    result = std::regex_replace(result, std::regex("\\s*;\\s*"), ";");
    result = std::regex_replace(result, std::regex("\\s*:\\s*"), ":");
    
    return result;
}

std::string UseNode::beautifyUse() const {
    std::string result = toUseString();
    
    // 美化格式
    result = std::regex_replace(result, std::regex("\\{"), "{\n  ");
    result = std::regex_replace(result, std::regex(";"), ";\n");
    result = std::regex_replace(result, std::regex("\\}"), "\n}\n");
    
    return result;
}

std::string UseNode::compressUse() const {
    std::string result = toUseString();
    
    // 压缩use
    result = std::regex_replace(result, std::regex("\\s+"), "");
    result = std::regex_replace(result, std::regex("use"), "u");
    result = std::regex_replace(result, std::regex("parameters"), "params");
    result = std::regex_replace(result, std::regex("arrays"), "arr");
    result = std::regex_replace(result, std::regex("objects"), "obj");
    result = std::regex_replace(result, std::regex("options"), "opts");
    result = std::regex_replace(result, std::regex("limits"), "lim");
    result = std::regex_replace(result, std::regex("defaults"), "def");
    result = std::regex_replace(result, std::regex("rules"), "r");
    
    return result;
}

std::string UseNode::decompressUse() const {
    std::string result = toUseString();
    
    // 解压缩use
    result = std::regex_replace(result, std::regex("u"), "use");
    result = std::regex_replace(result, std::regex("params"), "parameters");
    result = std::regex_replace(result, std::regex("arr"), "arrays");
    result = std::regex_replace(result, std::regex("obj"), "objects");
    result = std::regex_replace(result, std::regex("opts"), "options");
    result = std::regex_replace(result, std::regex("lim"), "limits");
    result = std::regex_replace(result, std::regex("def"), "defaults");
    result = std::regex_replace(result, std::regex("r"), "rules");
    
    return result;
}

std::string UseNode::encodeUse() const {
    std::string result = toUseString();
    
    // 编码use
    for (char& c : result) {
        if (std::isalnum(c)) {
            c = static_cast<char>((c - 'a' + 13) % 26 + 'a');
        }
    }
    
    return result;
}

std::string UseNode::decodeUse() const {
    std::string result = toUseString();
    
    // 解码use
    for (char& c : result) {
        if (std::isalnum(c)) {
            c = static_cast<char>((c - 'a' + 13) % 26 + 'a');
        }
    }
    
    return result;
}

std::string UseNode::getDebugInfo() const {
    std::ostringstream oss;
    oss << "UseNode Debug Info:\n";
    oss << "  Name: " << useName << "\n";
    oss << "  Type: " << getUseTypeName() << "\n";
    oss << "  Group: " << useGroup << "\n";
    oss << "  Version: " << useVersion << "\n";
    oss << "  Active: " << (isActive ? "true" : "false") << "\n";
    oss << "  Processed: " << (isProcessed ? "true" : "false") << "\n";
    oss << "  Valid: " << (isValid ? "true" : "false") << "\n";
    oss << "  Priority: " << priority << "\n";
    oss << "  Required: " << (isRequired ? "true" : "false") << "\n";
    oss << "  Optional: " << (isOptional ? "true" : "false") << "\n";
    oss << "  Inherited: " << (isInherited ? "true" : "false") << "\n";
    
    if (!useParameters.empty()) {
        oss << "  Parameters: ";
        for (const auto& param : useParameters) {
            oss << param.first << "=" << param.second << " ";
        }
        oss << "\n";
    }
    
    if (!useRules.empty()) {
        oss << "  Rules: ";
        for (const auto& rule : useRules) {
            oss << rule << " ";
        }
        oss << "\n";
    }
    
    if (!useErrors.empty()) {
        oss << "  Errors: ";
        for (const auto& error : useErrors) {
            oss << error << " ";
        }
        oss << "\n";
    }
    
    if (!useWarnings.empty()) {
        oss << "  Warnings: ";
        for (const auto& warning : useWarnings) {
            oss << warning << " ";
        }
        oss << "\n";
    }
    
    return oss.str();
}

std::shared_ptr<BaseNode> UseNode::clone() const {
    auto cloned = std::make_shared<UseNode>(useName, useType);
    
    // 复制基本属性
    cloned->setValue(value);
    // cloned->setAttributes(attributes);
    
    // 复制use属性
    cloned->useGroup = useGroup;
    cloned->useVersion = useVersion;
    cloned->useParameters = useParameters;
    cloned->useArrays = useArrays;
    cloned->useObjects = useObjects;
    cloned->useOptions = useOptions;
    cloned->useLimits = useLimits;
    cloned->useDefaults = useDefaults;
    cloned->useRules = useRules;
    cloned->ruleValues = ruleValues;
    cloned->ruleFunctions = ruleFunctions;
    cloned->isActive = isActive;
    cloned->isProcessed = isProcessed;
    cloned->isValid = isValid;
    cloned->useErrors = useErrors;
    cloned->useWarnings = useWarnings;
    cloned->metadata = metadata;
    cloned->priority = priority;
    cloned->isRequired = isRequired;
    cloned->isOptional = isOptional;
    cloned->isInherited = isInherited;
    
    return cloned;
}

bool UseNode::operator==(const UseNode& other) const {
    return useName == other.useName &&
           useType == other.useType &&
           useGroup == other.useGroup &&
           useVersion == other.useVersion &&
           useParameters == other.useParameters &&
           useArrays == other.useArrays &&
           useObjects == other.useObjects &&
           useOptions == other.useOptions &&
           useLimits == other.useLimits &&
           useDefaults == other.useDefaults &&
           useRules == other.useRules &&
           ruleValues == other.ruleValues &&
           isActive == other.isActive &&
           isProcessed == other.isProcessed &&
           isValid == other.isValid &&
           priority == other.priority &&
           isRequired == other.isRequired &&
           isOptional == other.isOptional &&
           isInherited == other.isInherited;
}

bool UseNode::operator!=(const UseNode& other) const {
    return !(*this == other);
}

// 私有辅助方法实现
std::string UseNode::processUseParameter(const std::string& key, const std::string& value) const {
    if (key.empty() || value.empty()) {
        return "";
    }
    
    std::string result = value;
    
    // 处理use参数
    result = std::regex_replace(result, std::regex("\\s+"), " ");
    result = std::regex_replace(result, std::regex("^\\s+|\\s+$"), "");
    
    return result;
}

std::string UseNode::validateUseParameter(const std::string& key, const std::string& value) const {
    if (key.empty() || value.empty()) {
        return "";
    }
    
    // 验证use参数
    if (value.find("invalid") != std::string::npos) {
        return "";
    }
    
    return value;
}

std::string UseNode::sanitizeUseParameter(const std::string& key, const std::string& value) const {
    if (key.empty() || value.empty()) {
        return "";
    }
    
    std::string result = value;
    
    // 清理use参数
    result = std::regex_replace(result, std::regex("[^a-zA-Z0-9\\s\\-_=<>!&|()\\[\\]{}:;,\\.]"), "");
    
    return result;
}

std::string UseNode::parseUseRule(const std::string& rule) const {
    if (rule.empty()) {
        return "";
    }
    
    // 解析use规则
    return processUseParameter("rule", rule);
}

std::string UseNode::evaluateUseRule(const std::string& rule) const {
    if (rule.empty()) {
        return "";
    }
    
    // 评估use规则
    return processUseParameter("rule", rule);
}

std::string UseNode::optimizeUseRule(const std::string& rule) const {
    if (rule.empty()) {
        return "";
    }
    
    // 优化use规则
    return processUseParameter("rule", rule);
}

void UseNode::processUseParameters() {
    for (auto& param : useParameters) {
        param.second = processUseParameter(param.first, param.second);
    }
}

void UseNode::processUseOptions() {
    for (const auto& option : useOptions) {
        // 处理use选项
    }
}

void UseNode::processUseLimits() {
    for (const auto& limit : useLimits) {
        // 处理use限制
    }
}

void UseNode::processUseDefaults() {
    for (const auto& def : useDefaults) {
        // 处理use默认值
    }
}

void UseNode::processUseRules() {
    for (const auto& rule : useRules) {
        // 处理use规则
    }
}

bool UseNode::validateUseParameters() const {
    for (const auto& param : useParameters) {
        if (param.first.empty() || param.second.empty()) {
            return false;
        }
    }
    return true;
}

bool UseNode::validateUseOptions() const {
    for (const auto& option : useOptions) {
        if (option.first.empty()) {
            return false;
        }
    }
    return true;
}

bool UseNode::validateUseLimits() const {
    for (const auto& limit : useLimits) {
        if (limit.first.empty() || limit.second < 0) {
            return false;
        }
    }
    return true;
}

bool UseNode::validateUseDefaults() const {
    for (const auto& def : useDefaults) {
        if (def.first.empty() || def.second.empty()) {
            return false;
        }
    }
    return true;
}

bool UseNode::validateUseRules() const {
    for (const auto& rule : useRules) {
        if (rule.empty()) {
            return false;
        }
    }
    return true;
}

void UseNode::applyUseParameters() {
    for (const auto& param : useParameters) {
        // 应用use参数
    }
}


void UseNode::handleUseError(const std::string& error) {
    addUseError(error);
    isValid = false;
}

void UseNode::handleUseWarning(const std::string& warning) {
    addUseWarning(warning);
}

} // namespace CHTL