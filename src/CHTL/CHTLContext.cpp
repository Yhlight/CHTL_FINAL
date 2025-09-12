#include "CHTLContext.h"

namespace CHTL {

CHTLContext::CHTLContext() {}

CHTLContext::~CHTLContext() {}

void CHTLContext::setSourcePath(const std::string& path) {
    sourcePath = path;
}

std::string CHTLContext::getSourcePath() const {
    return sourcePath;
}

void CHTLContext::setOutputDir(const std::string& dir) {
    outputDir = dir;
}

std::string CHTLContext::getOutputDir() const {
    return outputDir;
}

void CHTLContext::setOption(const std::string& key, const std::string& value) {
    options[key] = value;
}

std::string CHTLContext::getOption(const std::string& key) const {
    auto it = options.find(key);
    return it != options.end() ? it->second : "";
}

void CHTLContext::addGlobalVariable(const std::string& name, const std::string& value) {
    globalVariables[name] = value;
}

std::string CHTLContext::getGlobalVariable(const std::string& name) const {
    auto it = globalVariables.find(name);
    return it != globalVariables.end() ? it->second : "";
}

void CHTLContext::clear() {
    sourcePath.clear();
    outputDir.clear();
    options.clear();
    globalVariables.clear();
}

} // namespace CHTL