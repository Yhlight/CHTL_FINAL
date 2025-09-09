#include "CHTL/CHTLContext.h"
#include <algorithm>
#include <iostream>

namespace CHTL {

CHTLContext::CHTLContext() 
    : debugMode_(false) {
    // 设置默认选项
    options_["debug"] = "false";
    options_["optimize"] = "true";
    options_["warnings"] = "true";
    options_["errors"] = "true";
}

CHTLContext::~CHTLContext() = default;

void CHTLContext::setInputFile(const std::string& filePath) {
    inputFile_ = filePath;
}

void CHTLContext::setOutputFile(const std::string& filePath) {
    outputFile_ = filePath;
}

void CHTLContext::addModulePath(const std::string& path) {
    // 检查路径是否已存在
    if (std::find(modulePaths_.begin(), modulePaths_.end(), path) == modulePaths_.end()) {
        modulePaths_.push_back(path);
    }
}

void CHTLContext::setOption(const std::string& option, const std::string& value) {
    options_[option] = value;
}

std::string CHTLContext::getOption(const std::string& option) const {
    auto it = options_.find(option);
    if (it != options_.end()) {
        return it->second;
    }
    return "";
}

bool CHTLContext::isDebugMode() const {
    return debugMode_;
}

void CHTLContext::setDebugMode(bool debug) {
    debugMode_ = debug;
    setOption("debug", debug ? "true" : "false");
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
        std::cerr << "Warning: " << warning << std::endl;
    }
}

void CHTLContext::clearMessages() {
    errors_.clear();
    warnings_.clear();
}

} // namespace CHTL