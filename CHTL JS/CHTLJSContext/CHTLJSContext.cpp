#include "CHTLJSContext.h"
#include <algorithm>

namespace CHTLJS {

CHTLJSContext::CHTLJSContext() {
    reset();
}

CHTLJSContext::~CHTLJSContext() {
}

void CHTLJSContext::reset() {
    sourceCode.clear();
    variables.clear();
    functions.clear();
    virtualObjects.clear();
    configuration.clear();
    errors.clear();
    warnings.clear();
}

void CHTLJSContext::setSourceCode(const std::string& code) {
    sourceCode = code;
}

std::string CHTLJSContext::getSourceCode() const {
    return sourceCode;
}

void CHTLJSContext::addVariable(const std::string& name, const std::string& value) {
    variables[name] = value;
}

std::string CHTLJSContext::getVariable(const std::string& name) const {
    auto it = variables.find(name);
    return (it != variables.end()) ? it->second : "";
}

bool CHTLJSContext::hasVariable(const std::string& name) const {
    return variables.find(name) != variables.end();
}

void CHTLJSContext::addFunction(const std::string& name, const std::string& definition) {
    functions[name] = definition;
}

std::string CHTLJSContext::getFunction(const std::string& name) const {
    auto it = functions.find(name);
    return (it != functions.end()) ? it->second : "";
}

bool CHTLJSContext::hasFunction(const std::string& name) const {
    return functions.find(name) != functions.end();
}

void CHTLJSContext::addVirtualObject(const std::string& name, const std::string& definition) {
    virtualObjects[name] = definition;
}

std::string CHTLJSContext::getVirtualObject(const std::string& name) const {
    auto it = virtualObjects.find(name);
    return (it != virtualObjects.end()) ? it->second : "";
}

bool CHTLJSContext::hasVirtualObject(const std::string& name) const {
    return virtualObjects.find(name) != virtualObjects.end();
}

void CHTLJSContext::setConfiguration(const std::string& key, const std::string& value) {
    configuration[key] = value;
}

std::string CHTLJSContext::getConfiguration(const std::string& key) const {
    auto it = configuration.find(key);
    return (it != configuration.end()) ? it->second : "";
}

void CHTLJSContext::addError(const std::string& error) {
    errors.push_back(error);
}

std::vector<std::string> CHTLJSContext::getErrors() const {
    return errors;
}

bool CHTLJSContext::hasErrors() const {
    return !errors.empty();
}

void CHTLJSContext::addWarning(const std::string& warning) {
    warnings.push_back(warning);
}

std::vector<std::string> CHTLJSContext::getWarnings() const {
    return warnings;
}

} // namespace CHTLJS