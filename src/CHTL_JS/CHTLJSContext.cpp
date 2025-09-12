#include "CHTLJSContext.h"

namespace CHTL {

CHTLJSContext::CHTLJSContext() {}

CHTLJSContext::~CHTLJSContext() {}

void CHTLJSContext::setSourcePath(const std::string& path) {
    sourcePath = path;
}

std::string CHTLJSContext::getSourcePath() const {
    return sourcePath;
}

void CHTLJSContext::setOutputDir(const std::string& dir) {
    outputDir = dir;
}

std::string CHTLJSContext::getOutputDir() const {
    return outputDir;
}

void CHTLJSContext::setOption(const std::string& key, const std::string& value) {
    options[key] = value;
}

std::string CHTLJSContext::getOption(const std::string& key) const {
    auto it = options.find(key);
    return it != options.end() ? it->second : "";
}

void CHTLJSContext::addGlobalVariable(const std::string& name, const std::string& value) {
    globalVariables[name] = value;
}

std::string CHTLJSContext::getGlobalVariable(const std::string& name) const {
    auto it = globalVariables.find(name);
    return it != globalVariables.end() ? it->second : "";
}

void CHTLJSContext::addReactiveVariable(const std::string& name, const std::string& value) {
    reactiveVariables[name] = value;
}

std::string CHTLJSContext::getReactiveVariable(const std::string& name) const {
    auto it = reactiveVariables.find(name);
    return it != reactiveVariables.end() ? it->second : "";
}

void CHTLJSContext::clear() {
    sourcePath.clear();
    outputDir.clear();
    options.clear();
    globalVariables.clear();
    reactiveVariables.clear();
}

} // namespace CHTL