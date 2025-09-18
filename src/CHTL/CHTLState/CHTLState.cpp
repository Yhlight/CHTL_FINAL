#include "CHTL/CHTLState.h"
#include "CHTL/BaseNode.h"
#include "CHTL/ElementNode.h"
#include "CHTL/TemplateNode.h"
#include "CHTL/CustomNode.h"
#include "CHTL/ImportNode.h"
#include "CHTL/NamespaceNode.h"
#include "CHTL/ConfigurationNode.h"
#include <iostream>
#include <algorithm>
#include <sstream>

namespace CHTL {

CHTLState::CHTLState() 
    : currentPhase_(CompilationPhase::INITIALIZATION),
      currentNamespace_(""),
      currentScope_(0),
      debugMode_(false) {
    initializeState();
}

CHTLState::~CHTLState() = default;

void CHTLState::initializeState() {
    // Initialize compilation phases
    phases_ = {
        CompilationPhase::INITIALIZATION,
        CompilationPhase::LEXING,
        CompilationPhase::PARSING,
        CompilationPhase::TEMPLATE_PROCESSING,
        CompilationPhase::CUSTOM_PROCESSING,
        CompilationPhase::IMPORT_PROCESSING,
        CompilationPhase::NAMESPACE_PROCESSING,
        CompilationPhase::CONFIGURATION_PROCESSING,
        CompilationPhase::GENERATION,
        CompilationPhase::FINALIZATION
    };
    
    // Initialize scope stack
    scopeStack_.push_back(0);
    
    // Initialize namespace stack
    namespaceStack_.push_back("");
    
    // Initialize template processing state
    templateProcessingState_ = TemplateProcessingState::NONE;
    
    // Initialize custom processing state
    customProcessingState_ = CustomProcessingState::NONE;
    
    // Initialize import processing state
    importProcessingState_ = ImportProcessingState::NONE;
    
    // Initialize namespace processing state
    namespaceProcessingState_ = NamespaceProcessingState::NONE;
    
    // Initialize configuration processing state
    configurationProcessingState_ = ConfigurationProcessingState::NONE;
}

void CHTLState::setCurrentPhase(CompilationPhase phase) {
    currentPhase_ = phase;
    
    if (debugMode_) {
        std::cout << "Changed to phase: " << getPhaseName(phase) << std::endl;
    }
}

CHTLState::CompilationPhase CHTLState::getCurrentPhase() const {
    return currentPhase_;
}

std::string CHTLState::getPhaseName(CompilationPhase phase) const {
    switch (phase) {
        case CompilationPhase::INITIALIZATION:
            return "Initialization";
        case CompilationPhase::LEXING:
            return "Lexing";
        case CompilationPhase::PARSING:
            return "Parsing";
        case CompilationPhase::TEMPLATE_PROCESSING:
            return "Template Processing";
        case CompilationPhase::CUSTOM_PROCESSING:
            return "Custom Processing";
        case CompilationPhase::IMPORT_PROCESSING:
            return "Import Processing";
        case CompilationPhase::NAMESPACE_PROCESSING:
            return "Namespace Processing";
        case CompilationPhase::CONFIGURATION_PROCESSING:
            return "Configuration Processing";
        case CompilationPhase::GENERATION:
            return "Generation";
        case CompilationPhase::FINALIZATION:
            return "Finalization";
        default:
            return "Unknown";
    }
}

void CHTLState::setDebugMode(bool enabled) {
    debugMode_ = enabled;
}

bool CHTLState::isDebugMode() const {
    return debugMode_;
}

void CHTLState::setCurrentNamespace(const std::string& namespaceName) {
    currentNamespace_ = namespaceName;
}

std::string CHTLState::getCurrentNamespace() const {
    return currentNamespace_;
}

void CHTLState::pushNamespace(const std::string& namespaceName) {
    namespaceStack_.push_back(namespaceName);
    currentNamespace_ = namespaceName;
}

void CHTLState::popNamespace() {
    if (namespaceStack_.size() > 1) {
        namespaceStack_.pop_back();
        currentNamespace_ = namespaceStack_.back();
    }
}

void CHTLState::setCurrentScope(size_t scope) {
    currentScope_ = scope;
}

size_t CHTLState::getCurrentScope() const {
    return currentScope_;
}

void CHTLState::pushScope() {
    scopeStack_.push_back(currentScope_);
    currentScope_ = scopeStack_.size() - 1;
}

void CHTLState::popScope() {
    if (scopeStack_.size() > 1) {
        scopeStack_.pop_back();
        currentScope_ = scopeStack_.back();
    }
}

void CHTLState::setTemplateProcessingState(TemplateProcessingState state) {
    templateProcessingState_ = state;
    
    if (debugMode_) {
        std::cout << "Template processing state: " << getTemplateProcessingStateName(state) << std::endl;
    }
}

CHTLState::TemplateProcessingState CHTLState::getTemplateProcessingState() const {
    return templateProcessingState_;
}

std::string CHTLState::getTemplateProcessingStateName(TemplateProcessingState state) const {
    switch (state) {
        case TemplateProcessingState::NONE:
            return "None";
        case TemplateProcessingState::PROCESSING_STYLE_TEMPLATES:
            return "Processing Style Templates";
        case TemplateProcessingState::PROCESSING_ELEMENT_TEMPLATES:
            return "Processing Element Templates";
        case TemplateProcessingState::PROCESSING_VAR_TEMPLATES:
            return "Processing Variable Templates";
        default:
            return "Unknown";
    }
}

void CHTLState::setCustomProcessingState(CustomProcessingState state) {
    customProcessingState_ = state;
    
    if (debugMode_) {
        std::cout << "Custom processing state: " << getCustomProcessingStateName(state) << std::endl;
    }
}

CHTLState::CustomProcessingState CHTLState::getCustomProcessingState() const {
    return customProcessingState_;
}

std::string CHTLState::getCustomProcessingStateName(CustomProcessingState state) const {
    switch (state) {
        case CustomProcessingState::NONE:
            return "None";
        case CustomProcessingState::PROCESSING_CUSTOM_STYLES:
            return "Processing Custom Styles";
        case CustomProcessingState::PROCESSING_CUSTOM_ELEMENTS:
            return "Processing Custom Elements";
        case CustomProcessingState::PROCESSING_CUSTOM_VARS:
            return "Processing Custom Variables";
        default:
            return "Unknown";
    }
}

void CHTLState::setImportProcessingState(ImportProcessingState state) {
    importProcessingState_ = state;
    
    if (debugMode_) {
        std::cout << "Import processing state: " << getImportProcessingStateName(state) << std::endl;
    }
}

CHTLState::ImportProcessingState CHTLState::getImportProcessingState() const {
    return importProcessingState_;
}

std::string CHTLState::getImportProcessingStateName(ImportProcessingState state) const {
    switch (state) {
        case ImportProcessingState::NONE:
            return "None";
        case ImportProcessingState::PROCESSING_HTML_IMPORTS:
            return "Processing HTML Imports";
        case ImportProcessingState::PROCESSING_CSS_IMPORTS:
            return "Processing CSS Imports";
        case ImportProcessingState::PROCESSING_JS_IMPORTS:
            return "Processing JS Imports";
        case ImportProcessingState::PROCESSING_CHTL_IMPORTS:
            return "Processing CHTL Imports";
        case ImportProcessingState::PROCESSING_CJMOD_IMPORTS:
            return "Processing CJMOD Imports";
        default:
            return "Unknown";
    }
}

void CHTLState::setNamespaceProcessingState(NamespaceProcessingState state) {
    namespaceProcessingState_ = state;
    
    if (debugMode_) {
        std::cout << "Namespace processing state: " << getNamespaceProcessingStateName(state) << std::endl;
    }
}

CHTLState::NamespaceProcessingState CHTLState::getNamespaceProcessingState() const {
    return namespaceProcessingState_;
}

std::string CHTLState::getNamespaceProcessingStateName(NamespaceProcessingState state) const {
    switch (state) {
        case NamespaceProcessingState::NONE:
            return "None";
        case NamespaceProcessingState::PROCESSING_NAMESPACES:
            return "Processing Namespaces";
        case NamespaceProcessingState::PROCESSING_NAMESPACE_IMPORTS:
            return "Processing Namespace Imports";
        default:
            return "Unknown";
    }
}

void CHTLState::setConfigurationProcessingState(ConfigurationProcessingState state) {
    configurationProcessingState_ = state;
    
    if (debugMode_) {
        std::cout << "Configuration processing state: " << getConfigurationProcessingStateName(state) << std::endl;
    }
}

CHTLState::ConfigurationProcessingState CHTLState::getConfigurationProcessingState() const {
    return configurationProcessingState_;
}

std::string CHTLState::getConfigurationProcessingStateName(ConfigurationProcessingState state) const {
    switch (state) {
        case ConfigurationProcessingState::NONE:
            return "None";
        case ConfigurationProcessingState::PROCESSING_CONFIGURATIONS:
            return "Processing Configurations";
        case ConfigurationProcessingState::PROCESSING_CONFIGURATION_IMPORTS:
            return "Processing Configuration Imports";
        default:
            return "Unknown";
    }
}

void CHTLState::addProcessedTemplate(const std::string& templateName) {
    processedTemplates_.insert(templateName);
    
    if (debugMode_) {
        std::cout << "Processed template: " << templateName << std::endl;
    }
}

bool CHTLState::isTemplateProcessed(const std::string& templateName) const {
    return processedTemplates_.find(templateName) != processedTemplates_.end();
}

void CHTLState::addProcessedCustom(const std::string& customName) {
    processedCustoms_.insert(customName);
    
    if (debugMode_) {
        std::cout << "Processed custom: " << customName << std::endl;
    }
}

bool CHTLState::isCustomProcessed(const std::string& customName) const {
    return processedCustoms_.find(customName) != processedCustoms_.end();
}

void CHTLState::addProcessedImport(const std::string& importName) {
    processedImports_.insert(importName);
    
    if (debugMode_) {
        std::cout << "Processed import: " << importName << std::endl;
    }
}

bool CHTLState::isImportProcessed(const std::string& importName) const {
    return processedImports_.find(importName) != processedImports_.end();
}

void CHTLState::addProcessedNamespace(const std::string& namespaceName) {
    processedNamespaces_.insert(namespaceName);
    
    if (debugMode_) {
        std::cout << "Processed namespace: " << namespaceName << std::endl;
    }
}

bool CHTLState::isNamespaceProcessed(const std::string& namespaceName) const {
    return processedNamespaces_.find(namespaceName) != processedNamespaces_.end();
}

void CHTLState::addProcessedConfiguration(const std::string& configName) {
    processedConfigurations_.insert(configName);
    
    if (debugMode_) {
        std::cout << "Processed configuration: " << configName << std::endl;
    }
}

bool CHTLState::isConfigurationProcessed(const std::string& configName) const {
    return processedConfigurations_.find(configName) != processedConfigurations_.end();
}

void CHTLState::addError(const std::string& error) {
    errors_.push_back(error);
    
    if (debugMode_) {
        std::cerr << "Error: " << error << std::endl;
    }
}

void CHTLState::addWarning(const std::string& warning) {
    warnings_.push_back(warning);
    
    if (debugMode_) {
        std::cout << "Warning: " << warning << std::endl;
    }
}

std::vector<std::string> CHTLState::getErrors() const {
    return errors_;
}

std::vector<std::string> CHTLState::getWarnings() const {
    return warnings_;
}

void CHTLState::clearErrors() {
    errors_.clear();
}

void CHTLState::clearWarnings() {
    warnings_.clear();
}

void CHTLState::addVariable(const std::string& name, const std::string& value) {
    variables_[name] = value;
    
    if (debugMode_) {
        std::cout << "Added variable: " << name << " = " << value << std::endl;
    }
}

std::string CHTLState::getVariable(const std::string& name) const {
    auto it = variables_.find(name);
    if (it != variables_.end()) {
        return it->second;
    }
    return "";
}

bool CHTLState::hasVariable(const std::string& name) const {
    return variables_.find(name) != variables_.end();
}

void CHTLState::removeVariable(const std::string& name) {
    auto it = variables_.find(name);
    if (it != variables_.end()) {
        variables_.erase(it);
        
        if (debugMode_) {
            std::cout << "Removed variable: " << name << std::endl;
        }
    }
}

void CHTLState::setVariables(const std::map<std::string, std::string>& vars) {
    variables_ = vars;
}

std::map<std::string, std::string> CHTLState::getVariables() const {
    return variables_;
}

void CHTLState::addScopeVariable(const std::string& name, const std::string& value) {
    scopeVariables_[currentScope_][name] = value;
    
    if (debugMode_) {
        std::cout << "Added scope variable: " << name << " = " << value << " (scope: " << currentScope_ << ")" << std::endl;
    }
}

std::string CHTLState::getScopeVariable(const std::string& name) const {
    auto scopeIt = scopeVariables_.find(currentScope_);
    if (scopeIt != scopeVariables_.end()) {
        auto varIt = scopeIt->second.find(name);
        if (varIt != scopeIt->second.end()) {
            return varIt->second;
        }
    }
    return "";
}

bool CHTLState::hasScopeVariable(const std::string& name) const {
    auto scopeIt = scopeVariables_.find(currentScope_);
    if (scopeIt != scopeVariables_.end()) {
        return scopeIt->second.find(name) != scopeIt->second.end();
    }
    return false;
}

void CHTLState::removeScopeVariable(const std::string& name) {
    auto scopeIt = scopeVariables_.find(currentScope_);
    if (scopeIt != scopeVariables_.end()) {
        auto varIt = scopeIt->second.find(name);
        if (varIt != scopeIt->second.end()) {
            scopeIt->second.erase(varIt);
            
            if (debugMode_) {
                std::cout << "Removed scope variable: " << name << " (scope: " << currentScope_ << ")" << std::endl;
            }
        }
    }
}

void CHTLState::clearScopeVariables() {
    auto scopeIt = scopeVariables_.find(currentScope_);
    if (scopeIt != scopeVariables_.end()) {
        scopeIt->second.clear();
        
        if (debugMode_) {
            std::cout << "Cleared scope variables for scope: " << currentScope_ << std::endl;
        }
    }
}

void CHTLState::addPhaseData(const std::string& key, const std::string& value) {
    phaseData_[currentPhase_][key] = value;
    
    if (debugMode_) {
        std::cout << "Added phase data: " << key << " = " << value << " (phase: " << getPhaseName(currentPhase_) << ")" << std::endl;
    }
}

std::string CHTLState::getPhaseData(const std::string& key) const {
    auto phaseIt = phaseData_.find(currentPhase_);
    if (phaseIt != phaseData_.end()) {
        auto dataIt = phaseIt->second.find(key);
        if (dataIt != phaseIt->second.end()) {
            return dataIt->second;
        }
    }
    return "";
}

bool CHTLState::hasPhaseData(const std::string& key) const {
    auto phaseIt = phaseData_.find(currentPhase_);
    if (phaseIt != phaseData_.end()) {
        return phaseIt->second.find(key) != phaseIt->second.end();
    }
    return false;
}

void CHTLState::removePhaseData(const std::string& key) {
    auto phaseIt = phaseData_.find(currentPhase_);
    if (phaseIt != phaseData_.end()) {
        auto dataIt = phaseIt->second.find(key);
        if (dataIt != phaseIt->second.end()) {
            phaseIt->second.erase(dataIt);
            
            if (debugMode_) {
                std::cout << "Removed phase data: " << key << " (phase: " << getPhaseName(currentPhase_) << ")" << std::endl;
            }
        }
    }
}

void CHTLState::clearPhaseData() {
    auto phaseIt = phaseData_.find(currentPhase_);
    if (phaseIt != phaseData_.end()) {
        phaseIt->second.clear();
        
        if (debugMode_) {
            std::cout << "Cleared phase data for phase: " << getPhaseName(currentPhase_) << std::endl;
        }
    }
}

void CHTLState::clearAll() {
    processedTemplates_.clear();
    processedCustoms_.clear();
    processedImports_.clear();
    processedNamespaces_.clear();
    processedConfigurations_.clear();
    errors_.clear();
    warnings_.clear();
    variables_.clear();
    scopeVariables_.clear();
    phaseData_.clear();
    scopeStack_.clear();
    scopeStack_.push_back(0);
    namespaceStack_.clear();
    namespaceStack_.push_back("");
    currentScope_ = 0;
    currentNamespace_ = "";
    currentPhase_ = CompilationPhase::INITIALIZATION;
    templateProcessingState_ = TemplateProcessingState::NONE;
    customProcessingState_ = CustomProcessingState::NONE;
    importProcessingState_ = ImportProcessingState::NONE;
    namespaceProcessingState_ = NamespaceProcessingState::NONE;
    configurationProcessingState_ = ConfigurationProcessingState::NONE;
}

void CHTLState::reset() {
    clearAll();
    initializeState();
}

std::string CHTLState::getStateInfo() const {
    std::stringstream info;
    
    info << "CHTL State Information:" << std::endl;
    info << "Current Phase: " << getPhaseName(currentPhase_) << std::endl;
    info << "Current Namespace: " << currentNamespace_ << std::endl;
    info << "Current Scope: " << currentScope_ << std::endl;
    info << "Debug Mode: " << (debugMode_ ? "enabled" : "disabled") << std::endl;
    info << "Processed Templates: " << processedTemplates_.size() << std::endl;
    info << "Processed Customs: " << processedCustoms_.size() << std::endl;
    info << "Processed Imports: " << processedImports_.size() << std::endl;
    info << "Processed Namespaces: " << processedNamespaces_.size() << std::endl;
    info << "Processed Configurations: " << processedConfigurations_.size() << std::endl;
    info << "Variables: " << variables_.size() << std::endl;
    info << "Scope Variables: " << scopeVariables_.size() << std::endl;
    info << "Phase Data: " << phaseData_.size() << std::endl;
    info << "Errors: " << errors_.size() << std::endl;
    info << "Warnings: " << warnings_.size() << std::endl;
    
    return info.str();
}

void CHTLState::printState() const {
    std::cout << getStateInfo() << std::endl;
}

void CHTLState::printProcessedItems() const {
    std::cout << "Processed Items:" << std::endl;
    
    std::cout << "  Templates:" << std::endl;
    for (const auto& templateName : processedTemplates_) {
        std::cout << "    " << templateName << std::endl;
    }
    
    std::cout << "  Customs:" << std::endl;
    for (const auto& customName : processedCustoms_) {
        std::cout << "    " << customName << std::endl;
    }
    
    std::cout << "  Imports:" << std::endl;
    for (const auto& importName : processedImports_) {
        std::cout << "    " << importName << std::endl;
    }
    
    std::cout << "  Namespaces:" << std::endl;
    for (const auto& namespaceName : processedNamespaces_) {
        std::cout << "    " << namespaceName << std::endl;
    }
    
    std::cout << "  Configurations:" << std::endl;
    for (const auto& configName : processedConfigurations_) {
        std::cout << "    " << configName << std::endl;
    }
}

} // namespace CHTL