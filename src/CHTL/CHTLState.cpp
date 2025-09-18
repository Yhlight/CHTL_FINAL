#include "CHTLState.h"

namespace CHTL {

CHTLState::CHTLState() : currentState_(CompilerState::INITIAL) {
    // 初始化默认配置
    configurations_["DEBUG_MODE"] = "false";
    configurations_["INDEX_INITIAL_COUNT"] = "0";
    configurations_["DISABLE_STYLE_AUTO_ADD_CLASS"] = "false";
    configurations_["DISABLE_STYLE_AUTO_ADD_ID"] = "false";
    configurations_["DISABLE_DEFAULT_NAMESPACE"] = "false";
    configurations_["DISABLE_CUSTOM_ORIGIN_TYPE"] = "false";
    configurations_["DISABLE_SCRIPT_AUTO_ADD_CLASS"] = "true";
    configurations_["DISABLE_SCRIPT_AUTO_ADD_ID"] = "true";
}

void CHTLState::setState(CompilerState state) {
    currentState_ = state;
}

void CHTLState::setInputFile(const std::string& filename) {
    inputFile_ = filename;
}

void CHTLState::setOutputFile(const std::string& filename) {
    outputFile_ = filename;
}

void CHTLState::setConfiguration(const std::string& key, const std::string& value) {
    configurations_[key] = value;
}

std::string CHTLState::getConfiguration(const std::string& key) const {
    auto it = configurations_.find(key);
    return (it != configurations_.end()) ? it->second : "";
}

void CHTLState::addError(const std::string& error) {
    errors_.push_back(error);
}

void CHTLState::addWarning(const std::string& warning) {
    warnings_.push_back(warning);
}

void CHTLState::clear() {
    currentState_ = CompilerState::INITIAL;
    inputFile_.clear();
    outputFile_.clear();
    configurations_.clear();
    errors_.clear();
    warnings_.clear();
}

} // namespace CHTL