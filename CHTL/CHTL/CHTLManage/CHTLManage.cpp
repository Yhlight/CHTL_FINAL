#include "CHTLManage.h"
#include <sstream>

namespace CHTL {

CHTLManage::CHTLManage() : debugMode(false), strictMode(false) {
}

void CHTLManage::setConfiguration(const std::string& key, const std::string& value) {
    configurations[key] = value;
}

std::string CHTLManage::getConfiguration(const std::string& key) const {
    auto it = configurations.find(key);
    return it != configurations.end() ? it->second : "";
}

bool CHTLManage::hasConfiguration(const std::string& key) const {
    return configurations.find(key) != configurations.end();
}

void CHTLManage::removeConfiguration(const std::string& key) {
    configurations.erase(key);
}

void CHTLManage::addError(const std::string& error) {
    errors.push_back(error);
}

void CHTLManage::addWarning(const std::string& warning) {
    warnings.push_back(warning);
}

void CHTLManage::clearErrors() {
    errors.clear();
}

void CHTLManage::clearWarnings() {
    warnings.clear();
}

void CHTLManage::reset() {
    configurations.clear();
    errors.clear();
    warnings.clear();
    debugMode = false;
    strictMode = false;
}

void CHTLManage::clear() {
    reset();
}

bool CHTLManage::isValid() const {
    return !hasErrors();
}

std::string CHTLManage::toString() const {
    std::ostringstream oss;
    oss << "CHTLManage{";
    oss << "configurations=" << configurations.size() << ", ";
    oss << "errors=" << errors.size() << ", ";
    oss << "warnings=" << warnings.size() << ", ";
    oss << "debugMode=" << (debugMode ? "true" : "false") << ", ";
    oss << "strictMode=" << (strictMode ? "true" : "false");
    oss << "}";
    return oss.str();
}

std::string CHTLManage::toDebugString() const {
    std::ostringstream oss;
    oss << "CHTLManage Debug Info:\n";
    oss << "  Configurations: " << configurations.size() << "\n";
    for (const auto& config : configurations) {
        oss << "    " << config.first << " = " << config.second << "\n";
    }
    oss << "  Errors: " << errors.size() << "\n";
    for (const auto& error : errors) {
        oss << "    " << error << "\n";
    }
    oss << "  Warnings: " << warnings.size() << "\n";
    for (const auto& warning : warnings) {
        oss << "    " << warning << "\n";
    }
    oss << "  Debug Mode: " << (debugMode ? "true" : "false") << "\n";
    oss << "  Strict Mode: " << (strictMode ? "true" : "false") << "\n";
    return oss.str();
}

std::string CHTLManage::toJSON() const {
    std::ostringstream oss;
    oss << "{\n";
    oss << "  \"configurations\": {\n";
    bool first = true;
    for (const auto& config : configurations) {
        if (!first) oss << ",\n";
        oss << "    \"" << config.first << "\": \"" << config.second << "\"";
        first = false;
    }
    oss << "\n  },\n";
    oss << "  \"errors\": [\n";
    first = true;
    for (const auto& error : errors) {
        if (!first) oss << ",\n";
        oss << "    \"" << error << "\"";
        first = false;
    }
    oss << "\n  ],\n";
    oss << "  \"warnings\": [\n";
    first = true;
    for (const auto& warning : warnings) {
        if (!first) oss << ",\n";
        oss << "    \"" << warning << "\"";
        first = false;
    }
    oss << "\n  ],\n";
    oss << "  \"debugMode\": " << (debugMode ? "true" : "false") << ",\n";
    oss << "  \"strictMode\": " << (strictMode ? "true" : "false") << "\n";
    oss << "}";
    return oss.str();
}

std::string CHTLManage::toXML() const {
    std::ostringstream oss;
    oss << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    oss << "<CHTLManage>\n";
    oss << "  <configurations>\n";
    for (const auto& config : configurations) {
        oss << "    <configuration key=\"" << config.first << "\">" << config.second << "</configuration>\n";
    }
    oss << "  </configurations>\n";
    oss << "  <errors>\n";
    for (const auto& error : errors) {
        oss << "    <error>" << error << "</error>\n";
    }
    oss << "  </errors>\n";
    oss << "  <warnings>\n";
    for (const auto& warning : warnings) {
        oss << "    <warning>" << warning << "</warning>\n";
    }
    oss << "  </warnings>\n";
    oss << "  <debugMode>" << (debugMode ? "true" : "false") << "</debugMode>\n";
    oss << "  <strictMode>" << (strictMode ? "true" : "false") << "</strictMode>\n";
    oss << "</CHTLManage>";
    return oss.str();
}

std::string CHTLManage::toYAML() const {
    std::ostringstream oss;
    oss << "CHTLManage:\n";
    oss << "  configurations:\n";
    for (const auto& config : configurations) {
        oss << "    " << config.first << ": " << config.second << "\n";
    }
    oss << "  errors:\n";
    for (const auto& error : errors) {
        oss << "    - " << error << "\n";
    }
    oss << "  warnings:\n";
    for (const auto& warning : warnings) {
        oss << "    - " << warning << "\n";
    }
    oss << "  debugMode: " << (debugMode ? "true" : "false") << "\n";
    oss << "  strictMode: " << (strictMode ? "true" : "false") << "\n";
    return oss.str();
}

CHTLManage CHTLManage::clone() const {
    CHTLManage cloned;
    cloned.configurations = configurations;
    cloned.errors = errors;
    cloned.warnings = warnings;
    cloned.debugMode = debugMode;
    cloned.strictMode = strictMode;
    return cloned;
}

bool CHTLManage::operator==(const CHTLManage& other) const {
    return configurations == other.configurations &&
           errors == other.errors &&
           warnings == other.warnings &&
           debugMode == other.debugMode &&
           strictMode == other.strictMode;
}

bool CHTLManage::operator!=(const CHTLManage& other) const {
    return !(*this == other);
}

} // namespace CHTL