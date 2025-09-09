#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include "CHTL.h"

namespace CHTL {

class CHTLContext {
private:
    std::map<std::string, std::string> configuration;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    std::map<std::string, std::unique_ptr<CHTLNode>> templates;
    std::map<std::string, std::unique_ptr<CHTLNode>> customDefinitions;
    std::map<std::string, std::unique_ptr<CHTLNode>> namespaces;
    std::map<std::string, std::string> keywordMappings;
    
    // Name configuration mappings
    std::map<std::string, std::vector<std::string>> nameGroups;
    int optionCount;
    bool disableNameGroup;
    
    // Style automation settings
    bool disableStyleAutoAddClass;
    bool disableStyleAutoAddId;
    bool disableScriptAutoAddClass;
    bool disableScriptAutoAddId;
    
    // Namespace settings
    bool disableDefaultNamespace;
    
    // Origin type settings
    bool disableCustomOriginType;
    std::map<std::string, std::string> originTypes;

public:
    CHTLContext();
    ~CHTLContext();
    
    // Configuration management
    void setConfiguration(const std::map<std::string, std::string>& config);
    void loadConfiguration(const std::string& configFile);
    std::string getConfigValue(const std::string& key, const std::string& defaultValue = "") const;
    bool getConfigBool(const std::string& key, bool defaultValue = false) const;
    
    // Name configuration
    void setKeywordMapping(const std::string& key, const std::vector<std::string>& values);
    std::string getKeywordMapping(const std::string& key) const;
    bool isKeywordValid(const std::string& key, const std::string& value) const;
    
    // Error and warning management
    void addError(const std::string& error);
    void addWarning(const std::string& warning);
    const std::vector<std::string>& getErrors() const;
    const std::vector<std::string>& getWarnings() const;
    void clearErrors();
    void clearWarnings();
    
    // Template management
    void addTemplate(const std::string& name, std::unique_ptr<CHTLNode> templateNode);
    CHTLNode* getTemplate(const std::string& name) const;
    bool hasTemplate(const std::string& name) const;
    
    // Custom definition management
    void addCustomDefinition(const std::string& name, std::unique_ptr<CHTLNode> customNode);
    CHTLNode* getCustomDefinition(const std::string& name) const;
    bool hasCustomDefinition(const std::string& name) const;
    
    // Namespace management
    void addNamespace(const std::string& name, std::unique_ptr<CHTLNode> namespaceNode);
    CHTLNode* getNamespace(const std::string& name) const;
    bool hasNamespace(const std::string& name) const;
    std::string resolveNamespace(const std::string& name) const;
    
    // Style automation settings
    bool shouldAutoAddClass() const;
    bool shouldAutoAddId() const;
    bool shouldAutoAddScriptClass() const;
    bool shouldAutoAddScriptId() const;
    
    // Origin type management
    void addOriginType(const std::string& name, const std::string& prefix);
    std::string getOriginTypePrefix(const std::string& name) const;
    bool isOriginTypeValid(const std::string& name) const;
    
    // Validation
    bool validateConfiguration() const;
    bool validateTemplates() const;
    bool validateCustomDefinitions() const;
    bool validateNamespaces() const;
    
    // Debug information
    void printDebugInfo() const;
};

} // namespace CHTL