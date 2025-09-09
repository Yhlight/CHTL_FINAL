#include "CHTLContext.h"
#include <fstream>
#include <sstream>
#include <iostream>

namespace CHTL {

CHTLContext::CHTLContext() 
    : optionCount(3)
    , disableNameGroup(false)
    , disableStyleAutoAddClass(false)
    , disableStyleAutoAddId(false)
    , disableScriptAutoAddClass(false)
    , disableScriptAutoAddId(false)
    , disableDefaultNamespace(false)
    , disableCustomOriginType(false)
{
    // Initialize default configuration
    configuration["INDEX_INITIAL_COUNT"] = "0";
    configuration["DEBUG_MODE"] = "false";
    configuration["DISABLE_STYLE_AUTO_ADD_CLASS"] = "false";
    configuration["DISABLE_STYLE_AUTO_ADD_ID"] = "false";
    configuration["DISABLE_SCRIPT_AUTO_ADD_CLASS"] = "false";
    configuration["DISABLE_SCRIPT_AUTO_ADD_ID"] = "false";
    configuration["DISABLE_DEFAULT_NAMESPACE"] = "false";
    configuration["DISABLE_CUSTOM_ORIGIN_TYPE"] = "false";
    
    // Initialize default keyword mappings
    nameGroups["CUSTOM_STYLE"] = {"@Style", "@style", "@CSS", "@Css", "@css"};
    nameGroups["CUSTOM_ELEMENT"] = {"@Element"};
    nameGroups["CUSTOM_VAR"] = {"@Var"};
    nameGroups["TEMPLATE_STYLE"] = {"@Style"};
    nameGroups["TEMPLATE_ELEMENT"] = {"@Element"};
    nameGroups["TEMPLATE_VAR"] = {"@Var"};
    nameGroups["ORIGIN_HTML"] = {"@Html"};
    nameGroups["ORIGIN_STYLE"] = {"@Style"};
    nameGroups["ORIGIN_JAVASCRIPT"] = {"@JavaScript"};
    nameGroups["IMPORT_HTML"] = {"@Html"};
    nameGroups["IMPORT_STYLE"] = {"@Style"};
    nameGroups["IMPORT_JAVASCRIPT"] = {"@JavaScript"};
    nameGroups["IMPORT_CHTL"] = {"@Chtl"};
    nameGroups["IMPORT_CRMOD"] = {"@CJmod"};
    nameGroups["KEYWORD_INHERIT"] = {"inherit"};
    nameGroups["KEYWORD_DELETE"] = {"delete"};
    nameGroups["KEYWORD_INSERT"] = {"insert"};
    nameGroups["KEYWORD_AFTER"] = {"after"};
    nameGroups["KEYWORD_BEFORE"] = {"before"};
    nameGroups["KEYWORD_REPLACE"] = {"replace"};
    nameGroups["KEYWORD_ATTOP"] = {"at top"};
    nameGroups["KEYWORD_ATBOTTOM"] = {"at bottom"};
    nameGroups["KEYWORD_FROM"] = {"from"};
    nameGroups["KEYWORD_AS"] = {"as"};
    nameGroups["KEYWORD_EXCEPT"] = {"except"};
    nameGroups["KEYWORD_USE"] = {"use"};
    nameGroups["KEYWORD_HTML5"] = {"html5"};
    nameGroups["KEYWORD_TEXT"] = {"text"};
    nameGroups["KEYWORD_STYLE"] = {"style"};
    nameGroups["KEYWORD_SCRIPT"] = {"script"};
    nameGroups["KEYWORD_CUSTOM"] = {"[Custom]"};
    nameGroups["KEYWORD_TEMPLATE"] = {"[Template]"};
    nameGroups["KEYWORD_ORIGIN"] = {"[Origin]"};
    nameGroups["KEYWORD_IMPORT"] = {"[Import]"};
    nameGroups["KEYWORD_NAMESPACE"] = {"[Namespace]"};
}

CHTLContext::~CHTLContext() = default;

void CHTLContext::setConfiguration(const std::map<std::string, std::string>& config) {
    for (const auto& pair : config) {
        configuration[pair.first] = pair.second;
    }
    
    // Update boolean flags
    disableNameGroup = getConfigBool("DISABLE_NAME_GROUP", false);
    disableStyleAutoAddClass = getConfigBool("DISABLE_STYLE_AUTO_ADD_CLASS", false);
    disableStyleAutoAddId = getConfigBool("DISABLE_STYLE_AUTO_ADD_ID", false);
    disableScriptAutoAddClass = getConfigBool("DISABLE_SCRIPT_AUTO_ADD_CLASS", false);
    disableScriptAutoAddId = getConfigBool("DISABLE_SCRIPT_AUTO_ADD_ID", false);
    disableDefaultNamespace = getConfigBool("DISABLE_DEFAULT_NAMESPACE", false);
    disableCustomOriginType = getConfigBool("DISABLE_CUSTOM_ORIGIN_TYPE", false);
    
    // Parse option count
    optionCount = std::stoi(getConfigValue("OPTION_COUNT", "3"));
}

void CHTLContext::loadConfiguration(const std::string& configFile) {
    std::ifstream file(configFile);
    if (!file.is_open()) {
        addError("Failed to open configuration file: " + configFile);
        return;
    }
    
    std::map<std::string, std::string> config;
    std::string line;
    while (std::getline(file, line)) {
        // Simple key=value parsing (would need more sophisticated parsing for real implementation)
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = line.substr(0, pos);
            std::string value = line.substr(pos + 1);
            // Remove quotes if present
            if (value.front() == '"' && value.back() == '"') {
                value = value.substr(1, value.length() - 2);
            }
            config[key] = value;
        }
    }
    
    setConfiguration(config);
}

std::string CHTLContext::getConfigValue(const std::string& key, const std::string& defaultValue) const {
    auto it = configuration.find(key);
    return (it != configuration.end()) ? it->second : defaultValue;
}

bool CHTLContext::getConfigBool(const std::string& key, bool defaultValue) const {
    std::string value = getConfigValue(key);
    if (value.empty()) return defaultValue;
    return (value == "true" || value == "1" || value == "yes");
}

void CHTLContext::setKeywordMapping(const std::string& key, const std::vector<std::string>& values) {
    if (values.size() > static_cast<size_t>(optionCount)) {
        addWarning("Keyword group " + key + " exceeds option count limit");
        return;
    }
    nameGroups[key] = values;
}

std::string CHTLContext::getKeywordMapping(const std::string& key) const {
    auto it = nameGroups.find(key);
    if (it != nameGroups.end() && !it->second.empty()) {
        return it->second[0]; // Return first option
    }
    return key; // Return original if not found
}

bool CHTLContext::isKeywordValid(const std::string& key, const std::string& value) const {
    auto it = nameGroups.find(key);
    if (it != nameGroups.end()) {
        for (const auto& option : it->second) {
            if (option == value) return true;
        }
    }
    return false;
}

void CHTLContext::addError(const std::string& error) {
    errors.push_back(error);
}

void CHTLContext::addWarning(const std::string& warning) {
    warnings.push_back(warning);
}

const std::vector<std::string>& CHTLContext::getErrors() const {
    return errors;
}

const std::vector<std::string>& CHTLContext::getWarnings() const {
    return warnings;
}

void CHTLContext::clearErrors() {
    errors.clear();
}

void CHTLContext::clearWarnings() {
    warnings.clear();
}

void CHTLContext::addTemplate(const std::string& name, std::unique_ptr<CHTLNode> templateNode) {
    templates[name] = std::move(templateNode);
}

CHTLNode* CHTLContext::getTemplate(const std::string& name) const {
    auto it = templates.find(name);
    return (it != templates.end()) ? it->second.get() : nullptr;
}

bool CHTLContext::hasTemplate(const std::string& name) const {
    return templates.find(name) != templates.end();
}

void CHTLContext::addCustomDefinition(const std::string& name, std::unique_ptr<CHTLNode> customNode) {
    customDefinitions[name] = std::move(customNode);
}

CHTLNode* CHTLContext::getCustomDefinition(const std::string& name) const {
    auto it = customDefinitions.find(name);
    return (it != customDefinitions.end()) ? it->second.get() : nullptr;
}

bool CHTLContext::hasCustomDefinition(const std::string& name) const {
    return customDefinitions.find(name) != customDefinitions.end();
}

void CHTLContext::addNamespace(const std::string& name, std::unique_ptr<CHTLNode> namespaceNode) {
    namespaces[name] = std::move(namespaceNode);
}

CHTLNode* CHTLContext::getNamespace(const std::string& name) const {
    auto it = namespaces.find(name);
    return (it != namespaces.end()) ? it->second.get() : nullptr;
}

bool CHTLContext::hasNamespace(const std::string& name) const {
    return namespaces.find(name) != namespaces.end();
}

std::string CHTLContext::resolveNamespace(const std::string& name) const {
    // Simple namespace resolution (would need more sophisticated logic for real implementation)
    return name;
}

bool CHTLContext::shouldAutoAddClass() const {
    return !disableStyleAutoAddClass;
}

bool CHTLContext::shouldAutoAddId() const {
    return !disableStyleAutoAddId;
}

bool CHTLContext::shouldAutoAddScriptClass() const {
    return !disableScriptAutoAddClass;
}

bool CHTLContext::shouldAutoAddScriptId() const {
    return !disableScriptAutoAddId;
}

void CHTLContext::addOriginType(const std::string& name, const std::string& prefix) {
    originTypes[name] = prefix;
}

std::string CHTLContext::getOriginTypePrefix(const std::string& name) const {
    auto it = originTypes.find(name);
    return (it != originTypes.end()) ? it->second : "";
}

bool CHTLContext::isOriginTypeValid(const std::string& name) const {
    return originTypes.find(name) != originTypes.end();
}

bool CHTLContext::validateConfiguration() const {
    // Basic validation logic
    return true;
}

bool CHTLContext::validateTemplates() const {
    // Template validation logic
    return true;
}

bool CHTLContext::validateCustomDefinitions() const {
    // Custom definition validation logic
    return true;
}

bool CHTLContext::validateNamespaces() const {
    // Namespace validation logic
    return true;
}

void CHTLContext::printDebugInfo() const {
    std::cout << "CHTL Context Debug Info:" << std::endl;
    std::cout << "  Configuration entries: " << configuration.size() << std::endl;
    std::cout << "  Templates: " << templates.size() << std::endl;
    std::cout << "  Custom definitions: " << customDefinitions.size() << std::endl;
    std::cout << "  Namespaces: " << namespaces.size() << std::endl;
    std::cout << "  Errors: " << errors.size() << std::endl;
    std::cout << "  Warnings: " << warnings.size() << std::endl;
}

} // namespace CHTL