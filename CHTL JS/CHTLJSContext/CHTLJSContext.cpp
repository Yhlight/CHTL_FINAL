#include "CHTLJSContext.h"
#include <iostream>

namespace CHTLJS {

CHTLJSContext::CHTLJSContext() 
    : debug_mode_(false), output_path_(""), current_namespace_("") {
}

CHTLJSContext::~CHTLJSContext() {
}

void CHTLJSContext::setDebugMode(bool debug) {
    debug_mode_ = debug;
}

bool CHTLJSContext::isDebugMode() const {
    return debug_mode_;
}

void CHTLJSContext::setOutputPath(const std::string& path) {
    output_path_ = path;
}

const std::string& CHTLJSContext::getOutputPath() const {
    return output_path_;
}

void CHTLJSContext::addModule(const std::string& name, const std::string& content) {
    modules_[name] = content;
}

std::string CHTLJSContext::getModule(const std::string& name) const {
    auto it = modules_.find(name);
    if (it != modules_.end()) {
        return it->second;
    }
    return "";
}

bool CHTLJSContext::hasModule(const std::string& name) const {
    return modules_.find(name) != modules_.end();
}

void CHTLJSContext::setVariable(const std::string& name, const std::string& value) {
    variables_[name] = value;
}

std::string CHTLJSContext::getVariable(const std::string& name) const {
    auto it = variables_.find(name);
    if (it != variables_.end()) {
        return it->second;
    }
    return "";
}

bool CHTLJSContext::hasVariable(const std::string& name) const {
    return variables_.find(name) != variables_.end();
}

void CHTLJSContext::setCurrentNamespace(const std::string& namespace_name) {
    current_namespace_ = namespace_name;
}

const std::string& CHTLJSContext::getCurrentNamespace() const {
    return current_namespace_;
}

void CHTLJSContext::addError(const std::string& error) {
    errors_.push_back(error);
    if (debug_mode_) {
        std::cerr << "[ERROR] " << error << std::endl;
    }
}

const std::vector<std::string>& CHTLJSContext::getErrors() const {
    return errors_;
}

bool CHTLJSContext::hasErrors() const {
    return !errors_.empty();
}

void CHTLJSContext::clearErrors() {
    errors_.clear();
}

void CHTLJSContext::addWarning(const std::string& warning) {
    warnings_.push_back(warning);
    if (debug_mode_) {
        std::cout << "[WARNING] " << warning << std::endl;
    }
}

const std::vector<std::string>& CHTLJSContext::getWarnings() const {
    return warnings_;
}

bool CHTLJSContext::hasWarnings() const {
    return !warnings_.empty();
}

void CHTLJSContext::clearWarnings() {
    warnings_.clear();
}

} // namespace CHTLJS