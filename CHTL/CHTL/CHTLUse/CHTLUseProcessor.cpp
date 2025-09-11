#include "CHTLUseProcessor.h"
#include <algorithm>
#include <sstream>
#include <regex>
#include <cctype>

namespace CHTL {

CHTLUseProcessor::CHTLUseProcessor()
    : debugMode(false), strictMode(false), enableUses(true), enableValidation(true),
      enableWarnings(true), enableErrors(true), totalUses(0), activeUses(0),
      processedUses(0), validUses(0), errorUses(0), warningUses(0) {
    initializeUseTypes();
    initializeRuleFunctions();
}

void CHTLUseProcessor::initializeUseTypes() {
    useTypeMap["html5"] = UseNode::UseType::HTML5;
    useTypeMap["configuration"] = UseNode::UseType::CONFIGURATION;
    useTypeMap["module"] = UseNode::UseType::MODULE;
    useTypeMap["template"] = UseNode::UseType::TEMPLATE;
    useTypeMap["custom"] = UseNode::UseType::CUSTOM;
    
    useTypeNames[UseNode::UseType::HTML5] = "html5";
    useTypeNames[UseNode::UseType::CONFIGURATION] = "configuration";
    useTypeNames[UseNode::UseType::MODULE] = "module";
    useTypeNames[UseNode::UseType::TEMPLATE] = "template";
    useTypeNames[UseNode::UseType::CUSTOM] = "custom";
}

void CHTLUseProcessor::initializeRuleFunctions() {
    // 注册默认规则函数
    ruleFunctions["is_valid"] = [](const std::string& value) { return !value.empty(); };
    ruleFunctions["is_not_empty"] = [](const std::string& value) { return !value.empty(); };
    ruleFunctions["is_number"] = [](const std::string& value) { 
        return !value.empty() && std::all_of(value.begin(), value.end(), ::isdigit); 
    };
    ruleFunctions["is_alpha"] = [](const std::string& value) { 
        return !value.empty() && std::all_of(value.begin(), value.end(), ::isalpha); 
    };
    ruleFunctions["is_alnum"] = [](const std::string& value) { 
        return !value.empty() && std::all_of(value.begin(), value.end(), ::isalnum); 
    };
}

void CHTLUseProcessor::addUse(std::shared_ptr<UseNode> use) {
    if (!use) return;
    
    uses.push_back(use);
    useMap[use->getUseName()] = use;
    
    if (use->isActiveUse()) {
        activeUses++;
    }
    
    updateUseStatistics();
    updateUseMappings();
}

void CHTLUseProcessor::removeUse(const std::string& name) {
    auto it = useMap.find(name);
    if (it != useMap.end()) {
        auto use = it->second;
        useMap.erase(it);
        
        uses.erase(std::remove(uses.begin(), uses.end(), use), uses.end());
        
        if (use->isActiveUse()) {
            activeUses--;
        }
        
        updateUseStatistics();
        updateUseMappings();
    }
}

void CHTLUseProcessor::removeUse(std::shared_ptr<UseNode> use) {
    if (!use) return;
    
    removeUse(use->getUseName());
}

std::shared_ptr<UseNode> CHTLUseProcessor::getUse(const std::string& name) const {
    auto it = useMap.find(name);
    return (it != useMap.end()) ? it->second : nullptr;
}

std::vector<std::shared_ptr<UseNode>> CHTLUseProcessor::getUses() const {
    return uses;
}

std::vector<std::shared_ptr<UseNode>> CHTLUseProcessor::getUsesByType(UseNode::UseType type) const {
    std::vector<std::shared_ptr<UseNode>> result;
    for (const auto& use : uses) {
        if (use->getUseType() == type) {
            result.push_back(use);
        }
    }
    return result;
}

std::vector<std::shared_ptr<UseNode>> CHTLUseProcessor::getUsesByGroup(const std::string& group) const {
    auto it = useGroups.find(group);
    return (it != useGroups.end()) ? it->second : std::vector<std::shared_ptr<UseNode>>();
}

void CHTLUseProcessor::addUseToGroup(const std::string& groupName, std::shared_ptr<UseNode> use) {
    if (!use) return;
    
    useGroups[groupName].push_back(use);
}

void CHTLUseProcessor::removeUseFromGroup(const std::string& groupName, std::shared_ptr<UseNode> use) {
    auto it = useGroups.find(groupName);
    if (it != useGroups.end()) {
        auto& group = it->second;
        group.erase(std::remove(group.begin(), group.end(), use), group.end());
    }
}

std::vector<std::string> CHTLUseProcessor::getUseGroups() const {
    std::vector<std::string> groups;
    for (const auto& pair : useGroups) {
        groups.push_back(pair.first);
    }
    return groups;
}

void CHTLUseProcessor::registerUseType(const std::string& name, UseNode::UseType type) {
    useTypeMap[name] = type;
    useTypeNames[type] = name;
}

void CHTLUseProcessor::unregisterUseType(const std::string& name) {
    auto it = useTypeMap.find(name);
    if (it != useTypeMap.end()) {
        useTypeNames.erase(it->second);
        useTypeMap.erase(it);
    }
}

UseNode::UseType CHTLUseProcessor::getUseType(const std::string& name) const {
    auto it = useTypeMap.find(name);
    return (it != useTypeMap.end()) ? it->second : UseNode::UseType::HTML5;
}

std::string CHTLUseProcessor::getUseTypeName(UseNode::UseType type) const {
    auto it = useTypeNames.find(type);
    return (it != useTypeNames.end()) ? it->second : "unknown";
}

std::vector<std::string> CHTLUseProcessor::getRegisteredUseTypes() const {
    std::vector<std::string> types;
    for (const auto& pair : useTypeMap) {
        types.push_back(pair.first);
    }
    return types;
}

void CHTLUseProcessor::registerRuleFunction(const std::string& name, std::function<bool(const std::string&)> function) {
    ruleFunctions[name] = function;
}

void CHTLUseProcessor::unregisterRuleFunction(const std::string& name) {
    ruleFunctions.erase(name);
}

std::function<bool(const std::string&)> CHTLUseProcessor::getRuleFunction(const std::string& name) const {
    auto it = ruleFunctions.find(name);
    return (it != ruleFunctions.end()) ? it->second : nullptr;
}

std::vector<std::string> CHTLUseProcessor::getRegisteredRuleFunctions() const {
    std::vector<std::string> functions;
    for (const auto& pair : ruleFunctions) {
        functions.push_back(pair.first);
    }
    return functions;
}

void CHTLUseProcessor::setRuleValue(const std::string& name, const std::string& value) {
    ruleValues[name] = value;
}

std::string CHTLUseProcessor::getRuleValue(const std::string& name) const {
    auto it = ruleValues.find(name);
    return (it != ruleValues.end()) ? it->second : "";
}

bool CHTLUseProcessor::hasRuleValue(const std::string& name) const {
    return ruleValues.find(name) != ruleValues.end();
}

void CHTLUseProcessor::removeRuleValue(const std::string& name) {
    ruleValues.erase(name);
}

bool CHTLUseProcessor::processUses() {
    if (!enableUses) {
        return true;
    }
    
    bool success = true;
    
    for (auto& use : uses) {
        if (!processUse(use)) {
            success = false;
        }
    }
    
    return success;
}

bool CHTLUseProcessor::processUse(std::shared_ptr<UseNode> use) {
    if (!use || !use->isActiveUse()) {
        return true;
    }
    
    try {
        bool result = use->processUse();
        if (result) {
            processedUses++;
        }
        return result;
    } catch (const std::exception& e) {
        handleUseError("处理use时发生错误: " + std::string(e.what()));
        return false;
    }
}

bool CHTLUseProcessor::processUseGroup(const std::string& groupName) {
    auto groupUses = getUsesByGroup(groupName);
    bool success = true;
    
    for (auto& use : groupUses) {
        if (!processUse(use)) {
            success = false;
        }
    }
    
    return success;
}

bool CHTLUseProcessor::processUseType(UseNode::UseType type) {
    auto typeUses = getUsesByType(type);
    bool success = true;
    
    for (auto& use : typeUses) {
        if (!processUse(use)) {
            success = false;
        }
    }
    
    return success;
}

bool CHTLUseProcessor::validateUses() const {
    if (!enableValidation) {
        return true;
    }
    
    bool success = true;
    
    for (const auto& use : uses) {
        if (!validateUse(use)) {
            success = false;
        }
    }
    
    return success;
}

bool CHTLUseProcessor::validateUse(std::shared_ptr<UseNode> use) const {
    if (!use) {
        return false;
    }
    
    return use->validateUse();
}

bool CHTLUseProcessor::validateUseGroup(const std::string& groupName) const {
    auto groupUses = getUsesByGroup(groupName);
    bool success = true;
    
    for (const auto& use : groupUses) {
        if (!validateUse(use)) {
            success = false;
        }
    }
    
    return success;
}

bool CHTLUseProcessor::validateUseType(UseNode::UseType type) const {
    auto typeUses = getUsesByType(type);
    bool success = true;
    
    for (const auto& use : typeUses) {
        if (!validateUse(use)) {
            success = false;
        }
    }
    
    return success;
}

bool CHTLUseProcessor::applyUses() {
    if (!enableUses) {
        return true;
    }
    
    bool success = true;
    
    for (auto& use : uses) {
        if (!applyUse(use)) {
            success = false;
        }
    }
    
    return success;
}

bool CHTLUseProcessor::applyUse(std::shared_ptr<UseNode> use) {
    if (!use || !use->isProcessedUse()) {
        return false;
    }
    
    try {
        bool result = use->applyUse();
        if (result && use->isValidUse()) {
            validUses++;
        }
        return result;
    } catch (const std::exception& e) {
        handleUseError("应用use时发生错误: " + std::string(e.what()));
        return false;
    }
}

bool CHTLUseProcessor::applyUseGroup(const std::string& groupName) {
    auto groupUses = getUsesByGroup(groupName);
    bool success = true;
    
    for (auto& use : groupUses) {
        if (!applyUse(use)) {
            success = false;
        }
    }
    
    return success;
}

bool CHTLUseProcessor::applyUseType(UseNode::UseType type) {
    auto typeUses = getUsesByType(type);
    bool success = true;
    
    for (auto& use : typeUses) {
        if (!applyUse(use)) {
            success = false;
        }
    }
    
    return success;
}

std::shared_ptr<UseNode> CHTLUseProcessor::parseUse(const std::string& useText) {
    if (useText.empty()) {
        return nullptr;
    }
    
    // 解析use文本
    auto use = std::make_shared<UseNode>("parsed_use");
    
    // 简单的use解析
    std::istringstream iss(useText);
    std::string line;
    while (std::getline(iss, line)) {
        if (line.empty() || line[0] == '#') continue;
        
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = line.substr(0, pos);
            std::string value = line.substr(pos + 1);
            
            // 清理键值对
            key = std::regex_replace(key, std::regex("^\\s+|\\s+$"), "");
            value = std::regex_replace(value, std::regex("^\\s+|\\s+$"), "");
            
            use->setUseParameter(key, value);
        }
    }
    
    return use;
}

std::vector<std::shared_ptr<UseNode>> CHTLUseProcessor::parseUses(const std::string& usesText) {
    std::vector<std::shared_ptr<UseNode>> result;
    
    if (usesText.empty()) {
        return result;
    }
    
    // 简单的多use解析
    std::istringstream iss(usesText);
    std::string line;
    std::shared_ptr<UseNode> currentUse = nullptr;
    
    while (std::getline(iss, line)) {
        if (line.empty() || line[0] == '#') continue;
        
        if (line.find("use") == 0) {
            if (currentUse) {
                result.push_back(currentUse);
            }
            currentUse = std::make_shared<UseNode>("parsed_use");
        } else if (currentUse) {
            size_t pos = line.find('=');
            if (pos != std::string::npos) {
                std::string key = line.substr(0, pos);
                std::string value = line.substr(pos + 1);
                
                key = std::regex_replace(key, std::regex("^\\s+|\\s+$"), "");
                value = std::regex_replace(value, std::regex("^\\s+|\\s+$"), "");
                
                currentUse->setUseParameter(key, value);
            }
        }
    }
    
    if (currentUse) {
        result.push_back(currentUse);
    }
    
    return result;
}

std::string CHTLUseProcessor::parseUseParameter(const std::string& parameter) {
    if (parameter.empty()) {
        return "";
    }
    
    std::string result = parameter;
    result = std::regex_replace(result, std::regex("^\\s+|\\s+$"), "");
    return result;
}

std::vector<std::string> CHTLUseProcessor::parseUseArray(const std::string& array) {
    std::vector<std::string> result;
    
    if (array.empty()) {
        return result;
    }
    
    // 简单的数组解析
    std::string cleaned = std::regex_replace(array, std::regex("\\[|\\]"), "");
    std::istringstream iss(cleaned);
    std::string item;
    
    while (std::getline(iss, item, ',')) {
        item = std::regex_replace(item, std::regex("^\\s+|\\s+$"), "");
        if (!item.empty()) {
            result.push_back(item);
        }
    }
    
    return result;
}

std::map<std::string, std::string> CHTLUseProcessor::parseUseObject(const std::string& object) {
    std::map<std::string, std::string> result;
    
    if (object.empty()) {
        return result;
    }
    
    // 简单的对象解析
    std::string cleaned = std::regex_replace(object, std::regex("\\{|\\}"), "");
    std::istringstream iss(cleaned);
    std::string line;
    
    while (std::getline(iss, line, ',')) {
        size_t pos = line.find(':');
        if (pos != std::string::npos) {
            std::string key = line.substr(0, pos);
            std::string value = line.substr(pos + 1);
            
            key = std::regex_replace(key, std::regex("^\\s+|\\s+$"), "");
            value = std::regex_replace(value, std::regex("^\\s+|\\s+$"), "");
            
            result[key] = value;
        }
    }
    
    return result;
}

std::string CHTLUseProcessor::generateUses() const {
    std::ostringstream oss;
    
    for (const auto& use : uses) {
        oss << use->toUseString() << "\n";
    }
    
    return oss.str();
}

std::string CHTLUseProcessor::generateUse(std::shared_ptr<UseNode> use) const {
    if (!use) {
        return "";
    }
    
    return use->toUseString();
}

std::string CHTLUseProcessor::generateUseGroup(const std::string& groupName) const {
    auto groupUses = getUsesByGroup(groupName);
    std::ostringstream oss;
    
    for (const auto& use : groupUses) {
        oss << use->toUseString() << "\n";
    }
    
    return oss.str();
}

std::string CHTLUseProcessor::generateUseType(UseNode::UseType type) const {
    auto typeUses = getUsesByType(type);
    std::ostringstream oss;
    
    for (const auto& use : typeUses) {
        oss << use->toUseString() << "\n";
    }
    
    return oss.str();
}

std::string CHTLUseProcessor::toHTML() const {
    std::ostringstream oss;
    oss << "<!-- Use Processor -->\n";
    oss << "<!-- Total Uses: " << totalUses << " -->\n";
    oss << "<!-- Active Uses: " << activeUses << " -->\n";
    oss << "<!-- Processed Uses: " << processedUses << " -->\n";
    oss << "<!-- Valid Uses: " << validUses << " -->\n";
    
    for (const auto& use : uses) {
        oss << use->toHTML() << "\n";
    }
    
    return oss.str();
}

std::string CHTLUseProcessor::toCSS() const {
    std::ostringstream oss;
    oss << "/* Use Processor */\n";
    oss << "/* Total Uses: " << totalUses << " */\n";
    oss << "/* Active Uses: " << activeUses << " */\n";
    oss << "/* Processed Uses: " << processedUses << " */\n";
    oss << "/* Valid Uses: " << validUses << " */\n";
    
    for (const auto& use : uses) {
        oss << use->toCSS() << "\n";
    }
    
    return oss.str();
}

std::string CHTLUseProcessor::toJavaScript() const {
    std::ostringstream oss;
    oss << "// Use Processor\n";
    oss << "// Total Uses: " << totalUses << "\n";
    oss << "// Active Uses: " << activeUses << "\n";
    oss << "// Processed Uses: " << processedUses << "\n";
    oss << "// Valid Uses: " << validUses << "\n";
    
    for (const auto& use : uses) {
        oss << use->toJavaScript() << "\n";
    }
    
    return oss.str();
}

std::string CHTLUseProcessor::toUseString() const {
    return generateUses();
}

std::string CHTLUseProcessor::formatUses() const {
    return generateUses();
}

std::string CHTLUseProcessor::minifyUses() const {
    std::string result = generateUses();
    
    // 移除多余的空格和换行
    result = std::regex_replace(result, std::regex("\\s+"), " ");
    result = std::regex_replace(result, std::regex("\\s*\\{\\s*"), "{");
    result = std::regex_replace(result, std::regex("\\s*\\}\\s*"), "}");
    result = std::regex_replace(result, std::regex("\\s*;\\s*"), ";");
    result = std::regex_replace(result, std::regex("\\s*:\\s*"), ":");
    
    return result;
}

std::string CHTLUseProcessor::beautifyUses() const {
    std::string result = generateUses();
    
    // 美化格式
    result = std::regex_replace(result, std::regex("\\{"), "{\n  ");
    result = std::regex_replace(result, std::regex(";"), ";\n");
    result = std::regex_replace(result, std::regex("\\}"), "\n}\n");
    
    return result;
}

std::string CHTLUseProcessor::compressUses() const {
    std::string result = generateUses();
    
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

std::string CHTLUseProcessor::decompressUses() const {
    std::string result = generateUses();
    
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

std::string CHTLUseProcessor::encodeUses() const {
    std::string result = generateUses();
    
    // 编码use
    for (char& c : result) {
        if (std::isalnum(c)) {
            c = static_cast<char>((c - 'a' + 13) % 26 + 'a');
        }
    }
    
    return result;
}

std::string CHTLUseProcessor::decodeUses() const {
    std::string result = generateUses();
    
    // 解码use
    for (char& c : result) {
        if (std::isalnum(c)) {
            c = static_cast<char>((c - 'a' + 13) % 26 + 'a');
        }
    }
    
    return result;
}

void CHTLUseProcessor::setUseSetting(const std::string& key, const std::string& value) {
    useSettings[key] = value;
}

std::string CHTLUseProcessor::getUseSetting(const std::string& key) const {
    auto it = useSettings.find(key);
    return (it != useSettings.end()) ? it->second : "";
}

bool CHTLUseProcessor::hasUseSetting(const std::string& key) const {
    return useSettings.find(key) != useSettings.end();
}

void CHTLUseProcessor::removeUseSetting(const std::string& key) {
    useSettings.erase(key);
}

void CHTLUseProcessor::setUseFlag(const std::string& key, bool value) {
    useFlags[key] = value;
}

bool CHTLUseProcessor::getUseFlag(const std::string& key) const {
    auto it = useFlags.find(key);
    return (it != useFlags.end()) ? it->second : false;
}

bool CHTLUseProcessor::hasUseFlag(const std::string& key) const {
    return useFlags.find(key) != useFlags.end();
}

void CHTLUseProcessor::removeUseFlag(const std::string& key) {
    useFlags.erase(key);
}

void CHTLUseProcessor::setUseLimit(const std::string& key, int value) {
    useLimits[key] = value;
}

int CHTLUseProcessor::getUseLimit(const std::string& key) const {
    auto it = useLimits.find(key);
    return (it != useLimits.end()) ? it->second : 0;
}

bool CHTLUseProcessor::hasUseLimit(const std::string& key) const {
    return useLimits.find(key) != useLimits.end();
}

void CHTLUseProcessor::removeUseLimit(const std::string& key) {
    useLimits.erase(key);
}

void CHTLUseProcessor::setUseDefault(const std::string& key, const std::string& value) {
    useDefaults[key] = value;
}

std::string CHTLUseProcessor::getUseDefault(const std::string& key) const {
    auto it = useDefaults.find(key);
    return (it != useDefaults.end()) ? it->second : "";
}

bool CHTLUseProcessor::hasUseDefault(const std::string& key) const {
    return useDefaults.find(key) != useDefaults.end();
}

void CHTLUseProcessor::removeUseDefault(const std::string& key) {
    useDefaults.erase(key);
}

void CHTLUseProcessor::setUsePriority(const std::string& name, int priority) {
    usePriorities[name] = priority;
}

int CHTLUseProcessor::getUsePriority(const std::string& name) const {
    auto it = usePriorities.find(name);
    return (it != usePriorities.end()) ? it->second : 0;
}

bool CHTLUseProcessor::hasUsePriority(const std::string& name) const {
    return usePriorities.find(name) != usePriorities.end();
}

void CHTLUseProcessor::removeUsePriority(const std::string& name) {
    usePriorities.erase(name);
}

std::vector<std::shared_ptr<UseNode>> CHTLUseProcessor::getUsesByPriority(int priority) const {
    auto it = priorityUses.find(priority);
    return (it != priorityUses.end()) ? it->second : std::vector<std::shared_ptr<UseNode>>();
}

std::vector<int> CHTLUseProcessor::getUsePriorities() const {
    std::vector<int> priorities;
    for (const auto& pair : priorityUses) {
        priorities.push_back(pair.first);
    }
    std::sort(priorities.begin(), priorities.end());
    return priorities;
}

void CHTLUseProcessor::addValidationError(const std::string& error) {
    validationErrors.push_back(error);
}

void CHTLUseProcessor::addValidationWarning(const std::string& warning) {
    validationWarnings.push_back(warning);
}

void CHTLUseProcessor::addUseError(const std::string& error) {
    useErrors.push_back(error);
}

void CHTLUseProcessor::clearValidationMessages() {
    validationErrors.clear();
    validationWarnings.clear();
}

void CHTLUseProcessor::clearUseErrors() {
    useErrors.clear();
}

void CHTLUseProcessor::reset() {
    resetUses();
    resetUseGroups();
    resetUseTypes();
    resetUseRules();
    resetUseSettings();
    resetUsePriorities();
}

void CHTLUseProcessor::resetUses() {
    uses.clear();
    useMap.clear();
    updateUseStatistics();
}

void CHTLUseProcessor::resetUseGroups() {
    useGroups.clear();
}

void CHTLUseProcessor::resetUseTypes() {
    useTypeMap.clear();
    useTypeNames.clear();
    initializeUseTypes();
}

void CHTLUseProcessor::resetUseRules() {
    ruleFunctions.clear();
    ruleValues.clear();
    initializeRuleFunctions();
}

void CHTLUseProcessor::resetUseSettings() {
    useSettings.clear();
    useFlags.clear();
    useLimits.clear();
    useDefaults.clear();
}

void CHTLUseProcessor::resetUsePriorities() {
    usePriorities.clear();
    priorityUses.clear();
}

void CHTLUseProcessor::clearUses() {
    uses.clear();
    useMap.clear();
    updateUseStatistics();
}

void CHTLUseProcessor::clearUseGroups() {
    useGroups.clear();
}

void CHTLUseProcessor::clearUseTypes() {
    useTypeMap.clear();
    useTypeNames.clear();
}

void CHTLUseProcessor::clearUseRules() {
    ruleFunctions.clear();
    ruleValues.clear();
}

void CHTLUseProcessor::clearUseSettings() {
    useSettings.clear();
    useFlags.clear();
    useLimits.clear();
    useDefaults.clear();
}

void CHTLUseProcessor::clearUsePriorities() {
    usePriorities.clear();
    priorityUses.clear();
}

void CHTLUseProcessor::optimizeUses() {
    for (auto& use : uses) {
        optimizeUse(use);
    }
}

void CHTLUseProcessor::optimizeUse(std::shared_ptr<UseNode> use) {
    if (!use) return;
    
    // 优化use
    use->processUse();
}

void CHTLUseProcessor::optimizeUseGroup(const std::string& groupName) {
    auto groupUses = getUsesByGroup(groupName);
    for (auto& use : groupUses) {
        optimizeUse(use);
    }
}

void CHTLUseProcessor::optimizeUseType(UseNode::UseType type) {
    auto typeUses = getUsesByType(type);
    for (auto& use : typeUses) {
        optimizeUse(use);
    }
}

void CHTLUseProcessor::optimizeUseRule(const std::string& rule) {
    // 优化use规则
}

std::string CHTLUseProcessor::getDebugInfo() const {
    std::ostringstream oss;
    oss << "CHTLUseProcessor Debug Info:\n";
    oss << "  Total Uses: " << totalUses << "\n";
    oss << "  Active Uses: " << activeUses << "\n";
    oss << "  Processed Uses: " << processedUses << "\n";
    oss << "  Valid Uses: " << validUses << "\n";
    oss << "  Error Uses: " << errorUses << "\n";
    oss << "  Warning Uses: " << warningUses << "\n";
    oss << "  Debug Mode: " << (debugMode ? "true" : "false") << "\n";
    oss << "  Strict Mode: " << (strictMode ? "true" : "false") << "\n";
    oss << "  Uses Enabled: " << (enableUses ? "true" : "false") << "\n";
    oss << "  Validation Enabled: " << (enableValidation ? "true" : "false") << "\n";
    oss << "  Warnings Enabled: " << (enableWarnings ? "true" : "false") << "\n";
    oss << "  Errors Enabled: " << (enableErrors ? "true" : "false") << "\n";
    
    if (!validationErrors.empty()) {
        oss << "  Validation Errors: ";
        for (size_t i = 0; i < validationErrors.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << validationErrors[i];
        }
        oss << "\n";
    }
    
    if (!validationWarnings.empty()) {
        oss << "  Validation Warnings: ";
        for (size_t i = 0; i < validationWarnings.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << validationWarnings[i];
        }
        oss << "\n";
    }
    
    if (!useErrors.empty()) {
        oss << "  Use Errors: ";
        for (size_t i = 0; i < useErrors.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << useErrors[i];
        }
        oss << "\n";
    }
    
    return oss.str();
}

void CHTLUseProcessor::updateUseStatistics() {
    totalUses = uses.size();
    activeUses = 0;
    processedUses = 0;
    validUses = 0;
    errorUses = 0;
    warningUses = 0;
    
    for (const auto& use : uses) {
        if (use->isActiveUse()) {
            activeUses++;
        }
        if (use->isProcessedUse()) {
            processedUses++;
        }
        if (use->isValidUse()) {
            validUses++;
        }
        if (!use->getUseErrors().empty()) {
            errorUses++;
        }
        if (!use->getUseWarnings().empty()) {
            warningUses++;
        }
    }
}

void CHTLUseProcessor::updateUseMappings() {
    // 更新use映射
    useGroups.clear();
    
    for (const auto& use : uses) {
        // 更新use组映射
        if (!use->getUseGroup().empty()) {
            useGroups[use->getUseGroup()].push_back(use);
        }
    }
}

void CHTLUseProcessor::updateUsePriorities() {
    priorityUses.clear();
    
    for (const auto& use : uses) {
        int priority = use->getPriority();
        priorityUses[priority].push_back(use);
    }
}

void CHTLUseProcessor::handleUseError(const std::string& error) {
    addUseError(error);
    if (enableErrors) {
        // 处理use错误
    }
}

void CHTLUseProcessor::handleUseWarning(const std::string& warning) {
    addValidationWarning(warning);
    if (enableWarnings) {
        // 处理use警告
    }
}

// 私有辅助方法实现
bool CHTLUseProcessor::processHTML5Uses() {
    return processUseType(UseNode::UseType::HTML5);
}

bool CHTLUseProcessor::processConfigurationUses() {
    return processUseType(UseNode::UseType::CONFIGURATION);
}

bool CHTLUseProcessor::processModuleUses() {
    return processUseType(UseNode::UseType::MODULE);
}

bool CHTLUseProcessor::processTemplateUses() {
    return processUseType(UseNode::UseType::TEMPLATE);
}

bool CHTLUseProcessor::processCustomUses() {
    return processUseType(UseNode::UseType::CUSTOM);
}

bool CHTLUseProcessor::validateHTML5Uses() const {
    return validateUseType(UseNode::UseType::HTML5);
}

bool CHTLUseProcessor::validateConfigurationUses() const {
    return validateUseType(UseNode::UseType::CONFIGURATION);
}

bool CHTLUseProcessor::validateModuleUses() const {
    return validateUseType(UseNode::UseType::MODULE);
}

bool CHTLUseProcessor::validateTemplateUses() const {
    return validateUseType(UseNode::UseType::TEMPLATE);
}

bool CHTLUseProcessor::validateCustomUses() const {
    return validateUseType(UseNode::UseType::CUSTOM);
}

bool CHTLUseProcessor::applyHTML5Uses() {
    return applyUseType(UseNode::UseType::HTML5);
}

bool CHTLUseProcessor::applyConfigurationUses() {
    return applyUseType(UseNode::UseType::CONFIGURATION);
}

bool CHTLUseProcessor::applyModuleUses() {
    return applyUseType(UseNode::UseType::MODULE);
}

bool CHTLUseProcessor::applyTemplateUses() {
    return applyUseType(UseNode::UseType::TEMPLATE);
}

bool CHTLUseProcessor::applyCustomUses() {
    return applyUseType(UseNode::UseType::CUSTOM);
}

std::shared_ptr<UseNode> CHTLUseProcessor::parseHTML5Use(const std::string& useText) {
    auto use = parseUse(useText);
    if (use) {
        use->setUseType(UseNode::UseType::HTML5);
    }
    return use;
}

std::shared_ptr<UseNode> CHTLUseProcessor::parseConfigurationUse(const std::string& useText) {
    auto use = parseUse(useText);
    if (use) {
        use->setUseType(UseNode::UseType::CONFIGURATION);
    }
    return use;
}

std::shared_ptr<UseNode> CHTLUseProcessor::parseModuleUse(const std::string& useText) {
    auto use = parseUse(useText);
    if (use) {
        use->setUseType(UseNode::UseType::MODULE);
    }
    return use;
}

std::shared_ptr<UseNode> CHTLUseProcessor::parseTemplateUse(const std::string& useText) {
    auto use = parseUse(useText);
    if (use) {
        use->setUseType(UseNode::UseType::TEMPLATE);
    }
    return use;
}

std::shared_ptr<UseNode> CHTLUseProcessor::parseCustomUse(const std::string& useText) {
    auto use = parseUse(useText);
    if (use) {
        use->setUseType(UseNode::UseType::CUSTOM);
    }
    return use;
}

std::string CHTLUseProcessor::generateHTML5Uses() const {
    return generateUseType(UseNode::UseType::HTML5);
}

std::string CHTLUseProcessor::generateConfigurationUses() const {
    return generateUseType(UseNode::UseType::CONFIGURATION);
}

std::string CHTLUseProcessor::generateModuleUses() const {
    return generateUseType(UseNode::UseType::MODULE);
}

std::string CHTLUseProcessor::generateTemplateUses() const {
    return generateUseType(UseNode::UseType::TEMPLATE);
}

std::string CHTLUseProcessor::generateCustomUses() const {
    return generateUseType(UseNode::UseType::CUSTOM);
}

} // namespace CHTL