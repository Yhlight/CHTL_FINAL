#include "CHTLJSState.h"
#include <sstream>

namespace CHTL_JS {

CHTLJSState::CHTLJSState() : currentState(CHTLJSStateType::INITIAL), debugMode(false) {
}

CHTLJSState::CHTLJSState(CHTLJSStateType initialState) : currentState(initialState), debugMode(false) {
}

void CHTLJSState::setStateData(const std::string& key, const std::string& value) {
    stateData[key] = value;
}

std::string CHTLJSState::getStateData(const std::string& key) const {
    auto it = stateData.find(key);
    return it != stateData.end() ? it->second : "";
}

bool CHTLJSState::hasStateData(const std::string& key) const {
    return stateData.find(key) != stateData.end();
}

void CHTLJSState::removeStateData(const std::string& key) {
    stateData.erase(key);
}

void CHTLJSState::addError(const std::string& error) {
    errors.push_back(error);
}

void CHTLJSState::addWarning(const std::string& warning) {
    warnings.push_back(warning);
}

void CHTLJSState::clearErrors() {
    errors.clear();
}

void CHTLJSState::clearWarnings() {
    warnings.clear();
}

void CHTLJSState::reset() {
    currentState = CHTLJSStateType::INITIAL;
    stateData.clear();
    errors.clear();
    warnings.clear();
    debugMode = false;
}

void CHTLJSState::clear() {
    reset();
}

bool CHTLJSState::isValid() const {
    return currentState != CHTLJSStateType::ERROR && !hasErrors();
}

bool CHTLJSState::canTransitionTo(CHTLJSStateType targetState) const {
    switch (currentState) {
        case CHTLJSStateType::INITIAL:
            return targetState == CHTLJSStateType::PARSING || targetState == CHTLJSStateType::ERROR;
        case CHTLJSStateType::PARSING:
            return targetState == CHTLJSStateType::GENERATING || targetState == CHTLJSStateType::ERROR;
        case CHTLJSStateType::GENERATING:
            return targetState == CHTLJSStateType::COMPLETED || targetState == CHTLJSStateType::ERROR;
        case CHTLJSStateType::COMPLETED:
            return targetState == CHTLJSStateType::INITIAL || targetState == CHTLJSStateType::ERROR;
        case CHTLJSStateType::ERROR:
            return targetState == CHTLJSStateType::INITIAL;
        default:
            return false;
    }
}

bool CHTLJSState::transitionTo(CHTLJSStateType targetState) {
    if (canTransitionTo(targetState)) {
        currentState = targetState;
        return true;
    }
    return false;
}

std::string CHTLJSState::toString() const {
    std::ostringstream oss;
    oss << "CHTLJSState{";
    oss << "currentState=" << static_cast<int>(currentState) << ", ";
    oss << "stateData=" << stateData.size() << ", ";
    oss << "errors=" << errors.size() << ", ";
    oss << "warnings=" << warnings.size() << ", ";
    oss << "debugMode=" << (debugMode ? "true" : "false");
    oss << "}";
    return oss.str();
}

std::string CHTLJSState::toDebugString() const {
    std::ostringstream oss;
    oss << "CHTLJSState Debug Info:\n";
    oss << "  Current State: " << getStateName() << "\n";
    oss << "  State Data: " << stateData.size() << "\n";
    for (const auto& data : stateData) {
        oss << "    " << data.first << " = " << data.second << "\n";
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
    return oss.str();
}

std::string CHTLJSState::getStateName() const {
    switch (currentState) {
        case CHTLJSStateType::INITIAL:
            return "INITIAL";
        case CHTLJSStateType::PARSING:
            return "PARSING";
        case CHTLJSStateType::GENERATING:
            return "GENERATING";
        case CHTLJSStateType::COMPLETED:
            return "COMPLETED";
        case CHTLJSStateType::ERROR:
            return "ERROR";
        default:
            return "UNKNOWN";
    }
}

std::string CHTLJSState::toJSON() const {
    std::ostringstream oss;
    oss << "{\n";
    oss << "  \"currentState\": \"" << getStateName() << "\",\n";
    oss << "  \"stateData\": {\n";
    bool first = true;
    for (const auto& data : stateData) {
        if (!first) oss << ",\n";
        oss << "    \"" << data.first << "\": \"" << data.second << "\"";
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
    oss << "  \"debugMode\": " << (debugMode ? "true" : "false") << "\n";
    oss << "}";
    return oss.str();
}

std::string CHTLJSState::toXML() const {
    std::ostringstream oss;
    oss << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    oss << "<CHTLJSState>\n";
    oss << "  <currentState>" << getStateName() << "</currentState>\n";
    oss << "  <stateData>\n";
    for (const auto& data : stateData) {
        oss << "    <data key=\"" << data.first << "\">" << data.second << "</data>\n";
    }
    oss << "  </stateData>\n";
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
    oss << "</CHTLJSState>";
    return oss.str();
}

std::string CHTLJSState::toYAML() const {
    std::ostringstream oss;
    oss << "CHTLJSState:\n";
    oss << "  currentState: " << getStateName() << "\n";
    oss << "  stateData:\n";
    for (const auto& data : stateData) {
        oss << "    " << data.first << ": " << data.second << "\n";
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
    return oss.str();
}

CHTLJSState CHTLJSState::clone() const {
    CHTLJSState cloned;
    cloned.currentState = currentState;
    cloned.stateData = stateData;
    cloned.errors = errors;
    cloned.warnings = warnings;
    cloned.debugMode = debugMode;
    return cloned;
}

bool CHTLJSState::operator==(const CHTLJSState& other) const {
    return currentState == other.currentState &&
           stateData == other.stateData &&
           errors == other.errors &&
           warnings == other.warnings &&
           debugMode == other.debugMode;
}

bool CHTLJSState::operator!=(const CHTLJSState& other) const {
    return !(*this == other);
}

} // namespace CHTL_JS