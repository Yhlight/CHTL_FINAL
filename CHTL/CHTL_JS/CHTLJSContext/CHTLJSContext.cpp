#include "CHTLJSContext.h"
#include <sstream>
#include <algorithm>

namespace CHTL_JS {

CHTLJSContext::CHTLJSContext() : debugMode(false), strictMode(false) {
}

CHTLJSContext::CHTLJSContext(const std::string& source, const std::string& output)
    : sourceCode(source), outputPath(output), debugMode(false), strictMode(false) {
}

void CHTLJSContext::setVariable(const std::string& name, const std::string& value) {
    variables[name] = value;
}

std::string CHTLJSContext::getVariable(const std::string& name) const {
    auto it = variables.find(name);
    return it != variables.end() ? it->second : "";
}

bool CHTLJSContext::hasVariable(const std::string& name) const {
    return variables.find(name) != variables.end();
}

void CHTLJSContext::removeVariable(const std::string& name) {
    variables.erase(name);
}

void CHTLJSContext::setConfiguration(const std::string& key, const std::string& value) {
    configurations[key] = value;
}

std::string CHTLJSContext::getConfiguration(const std::string& key) const {
    auto it = configurations.find(key);
    return it != configurations.end() ? it->second : "";
}

bool CHTLJSContext::hasConfiguration(const std::string& key) const {
    return configurations.find(key) != configurations.end();
}

void CHTLJSContext::removeConfiguration(const std::string& key) {
    configurations.erase(key);
}

void CHTLJSContext::addError(const std::string& error) {
    errors.push_back(error);
}

void CHTLJSContext::addWarning(const std::string& warning) {
    warnings.push_back(warning);
}

void CHTLJSContext::clearErrors() {
    errors.clear();
}

void CHTLJSContext::clearWarnings() {
    warnings.clear();
}

void CHTLJSContext::reset() {
    variables.clear();
    configurations.clear();
    errors.clear();
    warnings.clear();
    debugMode = false;
    strictMode = false;
}

void CHTLJSContext::clear() {
    sourceCode.clear();
    outputPath.clear();
    reset();
}

bool CHTLJSContext::isValid() const {
    return !sourceCode.empty() && !hasErrors();
}

std::string CHTLJSContext::toString() const {
    std::ostringstream oss;
    oss << "CHTLJSContext{";
    oss << "sourceLength=" << sourceCode.length() << ", ";
    oss << "outputPath=" << outputPath << ", ";
    oss << "variables=" << variables.size() << ", ";
    oss << "configurations=" << configurations.size() << ", ";
    oss << "errors=" << errors.size() << ", ";
    oss << "warnings=" << warnings.size();
    oss << "}";
    return oss.str();
}

std::string CHTLJSContext::toDebugString() const {
    std::ostringstream oss;
    oss << "CHTLJSContext Debug Info:\n";
    oss << "  Source Code Length: " << sourceCode.length() << "\n";
    oss << "  Output Path: " << outputPath << "\n";
    oss << "  Variables: " << variables.size() << "\n";
    for (const auto& var : variables) {
        oss << "    " << var.first << " = " << var.second << "\n";
    }
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

std::string CHTLJSContext::toJSON() const {
    std::ostringstream oss;
    oss << "{\n";
    oss << "  \"sourceCodeLength\": " << sourceCode.length() << ",\n";
    oss << "  \"outputPath\": \"" << outputPath << "\",\n";
    oss << "  \"variables\": {\n";
    bool first = true;
    for (const auto& var : variables) {
        if (!first) oss << ",\n";
        oss << "    \"" << var.first << "\": \"" << var.second << "\"";
        first = false;
    }
    oss << "\n  },\n";
    oss << "  \"configurations\": {\n";
    first = true;
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

std::string CHTLJSContext::toXML() const {
    std::ostringstream oss;
    oss << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    oss << "<CHTLJSContext>\n";
    oss << "  <sourceCodeLength>" << sourceCode.length() << "</sourceCodeLength>\n";
    oss << "  <outputPath>" << outputPath << "</outputPath>\n";
    oss << "  <variables>\n";
    for (const auto& var : variables) {
        oss << "    <variable name=\"" << var.first << "\">" << var.second << "</variable>\n";
    }
    oss << "  </variables>\n";
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
    oss << "</CHTLJSContext>";
    return oss.str();
}

std::string CHTLJSContext::toYAML() const {
    std::ostringstream oss;
    oss << "CHTLJSContext:\n";
    oss << "  sourceCodeLength: " << sourceCode.length() << "\n";
    oss << "  outputPath: " << outputPath << "\n";
    oss << "  variables:\n";
    for (const auto& var : variables) {
        oss << "    " << var.first << ": " << var.second << "\n";
    }
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

CHTLJSContext CHTLJSContext::clone() const {
    CHTLJSContext cloned;
    cloned.sourceCode = sourceCode;
    cloned.outputPath = outputPath;
    cloned.variables = variables;
    cloned.configurations = configurations;
    cloned.errors = errors;
    cloned.warnings = warnings;
    cloned.debugMode = debugMode;
    cloned.strictMode = strictMode;
    return cloned;
}

bool CHTLJSContext::operator==(const CHTLJSContext& other) const {
    return sourceCode == other.sourceCode &&
           outputPath == other.outputPath &&
           variables == other.variables &&
           configurations == other.configurations &&
           errors == other.errors &&
           warnings == other.warnings &&
           debugMode == other.debugMode &&
           strictMode == other.strictMode;
}

bool CHTLJSContext::operator!=(const CHTLJSContext& other) const {
    return !(*this == other);
}

} // namespace CHTL_JS