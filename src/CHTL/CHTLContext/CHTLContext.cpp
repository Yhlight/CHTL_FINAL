#include "CHTL/CHTLContext.h"
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

CHTLContext::CHTLContext() 
    : currentNamespace_(""), debugMode_(false) {
    initializeContext();
}

CHTLContext::~CHTLContext() = default;

void CHTLContext::initializeContext() {
    // Initialize built-in symbols
    initializeBuiltinSymbols();
    
    // Initialize default configuration
    initializeDefaultConfiguration();
    
    // Initialize namespace stack
    namespaceStack_.push_back("");
}

void CHTLContext::initializeBuiltinSymbols() {
    // Initialize built-in HTML elements
    builtinElements_ = {
        "html", "head", "body", "title", "meta", "link", "style", "script",
        "div", "span", "p", "h1", "h2", "h3", "h4", "h5", "h6",
        "a", "img", "ul", "ol", "li", "table", "tr", "td", "th",
        "form", "input", "button", "textarea", "select", "option",
        "nav", "header", "footer", "main", "section", "article", "aside",
        "br", "hr", "strong", "em", "b", "i", "u", "s", "code", "pre"
    };
    
    // Initialize built-in CSS properties
    builtinCSSProperties_ = {
        "color", "background-color", "background", "font-size", "font-family",
        "font-weight", "font-style", "text-align", "text-decoration",
        "width", "height", "margin", "padding", "border", "border-radius",
        "display", "position", "top", "left", "right", "bottom",
        "z-index", "opacity", "visibility", "overflow", "float", "clear"
    };
    
    // Initialize built-in HTML attributes
    builtinAttributes_ = {
        "id", "class", "style", "title", "lang", "dir",
        "href", "src", "alt", "width", "height", "type", "value",
        "name", "placeholder", "required", "disabled", "readonly",
        "checked", "selected", "multiple", "autofocus", "autocomplete"
    };
}

void CHTLContext::initializeDefaultConfiguration() {
    // Set default configuration values
    configuration_["DEBUG_MODE"] = "false";
    configuration_["DISABLE_STYLE_AUTO_ADD_CLASS"] = "false";
    configuration_["DISABLE_STYLE_AUTO_ADD_ID"] = "false";
    configuration_["DISABLE_SCRIPT_AUTO_ADD_CLASS"] = "true";
    configuration_["DISABLE_SCRIPT_AUTO_ADD_ID"] = "true";
    configuration_["DISABLE_DEFAULT_NAMESPACE"] = "false";
    configuration_["DISABLE_CUSTOM_ORIGIN_TYPE"] = "false";
    configuration_["INDEX_INITIAL_COUNT"] = "0";
}

void CHTLContext::setDebugMode(bool enabled) {
    debugMode_ = enabled;
    configuration_["DEBUG_MODE"] = enabled ? "true" : "false";
}

bool CHTLContext::isDebugMode() const {
    return debugMode_;
}

void CHTLContext::setCurrentNamespace(const std::string& namespaceName) {
    currentNamespace_ = namespaceName;
}

std::string CHTLContext::getCurrentNamespace() const {
    return currentNamespace_;
}

void CHTLContext::pushNamespace(const std::string& namespaceName) {
    namespaceStack_.push_back(namespaceName);
    currentNamespace_ = namespaceName;
}

void CHTLContext::popNamespace() {
    if (namespaceStack_.size() > 1) {
        namespaceStack_.pop_back();
        currentNamespace_ = namespaceStack_.back();
    }
}

void CHTLContext::addSymbol(const std::string& name, const std::string& type, std::shared_ptr<BaseNode> node) {
    SymbolInfo symbol;
    symbol.name = name;
    symbol.type = type;
    symbol.node = node;
    symbol.namespace_ = currentNamespace_;
    symbol.defined = true;
    
    symbols_[name] = symbol;
    
    if (debugMode_) {
        std::cout << "Added symbol: " << name << " (type: " << type << ", namespace: " << currentNamespace_ << ")" << std::endl;
    }
}

void CHTLContext::removeSymbol(const std::string& name) {
    auto it = symbols_.find(name);
    if (it != symbols_.end()) {
        symbols_.erase(it);
        
        if (debugMode_) {
            std::cout << "Removed symbol: " << name << std::endl;
        }
    }
}

std::shared_ptr<BaseNode> CHTLContext::getSymbol(const std::string& name) const {
    auto it = symbols_.find(name);
    if (it != symbols_.end()) {
        return it->second.node;
    }
    return nullptr;
}

std::shared_ptr<BaseNode> CHTLContext::getSymbol(const std::string& name, const std::string& namespaceName) const {
    auto it = symbols_.find(name);
    if (it != symbols_.end() && it->second.namespace_ == namespaceName) {
        return it->second.node;
    }
    return nullptr;
}

bool CHTLContext::hasSymbol(const std::string& name) const {
    return symbols_.find(name) != symbols_.end();
}

bool CHTLContext::hasSymbol(const std::string& name, const std::string& namespaceName) const {
    auto it = symbols_.find(name);
    return it != symbols_.end() && it->second.namespace_ == namespaceName;
}

std::vector<std::string> CHTLContext::getSymbolNames() const {
    std::vector<std::string> names;
    for (const auto& pair : symbols_) {
        names.push_back(pair.first);
    }
    return names;
}

std::vector<std::string> CHTLContext::getSymbolNames(const std::string& type) const {
    std::vector<std::string> names;
    for (const auto& pair : symbols_) {
        if (pair.second.type == type) {
            names.push_back(pair.first);
        }
    }
    return names;
}

std::vector<std::string> CHTLContext::getSymbolNames(const std::string& type, const std::string& namespaceName) const {
    std::vector<std::string> names;
    for (const auto& pair : symbols_) {
        if (pair.second.type == type && pair.second.namespace_ == namespaceName) {
            names.push_back(pair.first);
        }
    }
    return names;
}

void CHTLContext::addTemplate(const std::string& name, std::shared_ptr<TemplateNode> templateNode) {
    addSymbol(name, "Template", templateNode);
}

std::shared_ptr<TemplateNode> CHTLContext::getTemplate(const std::string& name) const {
    auto node = getSymbol(name);
    if (node) {
        return std::dynamic_pointer_cast<TemplateNode>(node);
    }
    return nullptr;
}

void CHTLContext::addCustom(const std::string& name, std::shared_ptr<CustomNode> customNode) {
    addSymbol(name, "Custom", customNode);
}

std::shared_ptr<CustomNode> CHTLContext::getCustom(const std::string& name) const {
    auto node = getSymbol(name);
    if (node) {
        return std::dynamic_pointer_cast<CustomNode>(node);
    }
    return nullptr;
}

void CHTLContext::addImport(const std::string& name, std::shared_ptr<ImportNode> importNode) {
    addSymbol(name, "Import", importNode);
}

std::shared_ptr<ImportNode> CHTLContext::getImport(const std::string& name) const {
    auto node = getSymbol(name);
    if (node) {
        return std::dynamic_pointer_cast<ImportNode>(node);
    }
    return nullptr;
}

void CHTLContext::addNamespace(const std::string& name, std::shared_ptr<NamespaceNode> namespaceNode) {
    addSymbol(name, "Namespace", namespaceNode);
}

std::shared_ptr<NamespaceNode> CHTLContext::getNamespace(const std::string& name) const {
    auto node = getSymbol(name);
    if (node) {
        return std::dynamic_pointer_cast<NamespaceNode>(node);
    }
    return nullptr;
}

void CHTLContext::addConfiguration(const std::string& name, std::shared_ptr<ConfigurationNode> configNode) {
    addSymbol(name, "Configuration", configNode);
}

std::shared_ptr<ConfigurationNode> CHTLContext::getConfigurationNode(const std::string& name) const {
    auto node = getSymbol(name);
    if (node) {
        return std::dynamic_pointer_cast<ConfigurationNode>(node);
    }
    return nullptr;
}

void CHTLContext::setConfiguration(const std::string& key, const std::string& value) {
    configuration_[key] = value;
    
    if (debugMode_) {
        std::cout << "Set configuration: " << key << " = " << value << std::endl;
    }
}

std::string CHTLContext::getConfiguration(const std::string& key) const {
    auto it = configuration_.find(key);
    if (it != configuration_.end()) {
        return it->second;
    }
    return "";
}

bool CHTLContext::hasConfiguration(const std::string& key) const {
    return configuration_.find(key) != configuration_.end();
}

void CHTLContext::setConfiguration(const std::map<std::string, std::string>& config) {
    for (const auto& pair : config) {
        configuration_[pair.first] = pair.second;
    }
}

std::map<std::string, std::string> CHTLContext::getConfiguration() const {
    return configuration_;
}

void CHTLContext::addVariable(const std::string& name, const std::string& value) {
    variables_[name] = value;
    
    if (debugMode_) {
        std::cout << "Added variable: " << name << " = " << value << std::endl;
    }
}

std::string CHTLContext::getVariable(const std::string& name) const {
    auto it = variables_.find(name);
    if (it != variables_.end()) {
        return it->second;
    }
    return "";
}

bool CHTLContext::hasVariable(const std::string& name) const {
    return variables_.find(name) != variables_.end();
}

void CHTLContext::removeVariable(const std::string& name) {
    auto it = variables_.find(name);
    if (it != variables_.end()) {
        variables_.erase(it);
        
        if (debugMode_) {
            std::cout << "Removed variable: " << name << std::endl;
        }
    }
}

void CHTLContext::setVariables(const std::map<std::string, std::string>& vars) {
    variables_ = vars;
}

std::map<std::string, std::string> CHTLContext::getVariables() const {
    return variables_;
}

bool CHTLContext::isValidElement(const std::string& elementName) const {
    return builtinElements_.find(elementName) != builtinElements_.end();
}

bool CHTLContext::isValidAttribute(const std::string& attributeName) const {
    return builtinAttributes_.find(attributeName) != builtinAttributes_.end();
}

bool CHTLContext::isValidCSSProperty(const std::string& propertyName) const {
    return builtinCSSProperties_.find(propertyName) != builtinCSSProperties_.end();
}

void CHTLContext::addCustomElement(const std::string& elementName) {
    customElements_.insert(elementName);
}

void CHTLContext::addCustomAttribute(const std::string& attributeName) {
    customAttributes_.insert(attributeName);
}

void CHTLContext::addCustomCSSProperty(const std::string& propertyName) {
    customCSSProperties_.insert(propertyName);
}

bool CHTLContext::isCustomElement(const std::string& elementName) const {
    return customElements_.find(elementName) != customElements_.end();
}

bool CHTLContext::isCustomAttribute(const std::string& attributeName) const {
    return customAttributes_.find(attributeName) != customAttributes_.end();
}

bool CHTLContext::isCustomCSSProperty(const std::string& propertyName) const {
    return customCSSProperties_.find(propertyName) != customCSSProperties_.end();
}

void CHTLContext::addError(const std::string& error) {
    errors_.push_back(error);
    
    if (debugMode_) {
        std::cerr << "Error: " << error << std::endl;
    }
}

void CHTLContext::addWarning(const std::string& warning) {
    warnings_.push_back(warning);
    
    if (debugMode_) {
        std::cout << "Warning: " << warning << std::endl;
    }
}

std::vector<std::string> CHTLContext::getErrors() const {
    return errors_;
}

std::vector<std::string> CHTLContext::getWarnings() const {
    return warnings_;
}

void CHTLContext::clearErrors() {
    errors_.clear();
}

void CHTLContext::clearWarnings() {
    warnings_.clear();
}

void CHTLContext::clearAll() {
    symbols_.clear();
    configuration_.clear();
    variables_.clear();
    customElements_.clear();
    customAttributes_.clear();
    customCSSProperties_.clear();
    errors_.clear();
    warnings_.clear();
    namespaceStack_.clear();
    namespaceStack_.push_back("");
    currentNamespace_ = "";
}

void CHTLContext::reset() {
    clearAll();
    initializeContext();
}

std::string CHTLContext::getSymbolInfo() const {
    std::stringstream info;
    
    info << "CHTL Context Information:" << std::endl;
    info << "Current Namespace: " << currentNamespace_ << std::endl;
    info << "Debug Mode: " << (debugMode_ ? "enabled" : "disabled") << std::endl;
    info << "Symbols: " << symbols_.size() << std::endl;
    info << "Configuration: " << configuration_.size() << std::endl;
    info << "Variables: " << variables_.size() << std::endl;
    info << "Errors: " << errors_.size() << std::endl;
    info << "Warnings: " << warnings_.size() << std::endl;
    
    return info.str();
}

void CHTLContext::printSymbols() const {
    std::cout << "Symbols in context:" << std::endl;
    for (const auto& pair : symbols_) {
        std::cout << "  " << pair.first << " (type: " << pair.second.type 
                  << ", namespace: " << pair.second.namespace_ << ")" << std::endl;
    }
}

void CHTLContext::printConfiguration() const {
    std::cout << "Configuration:" << std::endl;
    for (const auto& pair : configuration_) {
        std::cout << "  " << pair.first << " = " << pair.second << std::endl;
    }
}

void CHTLContext::printVariables() const {
    std::cout << "Variables:" << std::endl;
    for (const auto& pair : variables_) {
        std::cout << "  " << pair.first << " = " << pair.second << std::endl;
    }
}

} // namespace CHTL