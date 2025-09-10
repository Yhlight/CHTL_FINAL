#include "CHTLState.h"
#include <sstream>

namespace CHTL {

CHTLState::CHTLState() : currentState(CHTLStateType::INITIAL), debugMode(false) {
}

CHTLState::CHTLState(CHTLStateType initialState) : currentState(initialState), debugMode(false) {
}

void CHTLState::setStateData(const std::string& key, const std::string& value) {
    stateData[key] = value;
}

std::string CHTLState::getStateData(const std::string& key) const {
    auto it = stateData.find(key);
    return it != stateData.end() ? it->second : "";
}

bool CHTLState::hasStateData(const std::string& key) const {
    return stateData.find(key) != stateData.end();
}

void CHTLState::removeStateData(const std::string& key) {
    stateData.erase(key);
}

void CHTLState::addError(const std::string& error) {
    errors.push_back(error);
}

void CHTLState::addWarning(const std::string& warning) {
    warnings.push_back(warning);
}

void CHTLState::clearErrors() {
    errors.clear();
}

void CHTLState::clearWarnings() {
    warnings.clear();
}

void CHTLState::reset() {
    currentState = CHTLStateType::INITIAL;
    stateData.clear();
    errors.clear();
    warnings.clear();
    debugMode = false;
}

void CHTLState::clear() {
    reset();
}

bool CHTLState::isValid() const {
    return currentState != CHTLStateType::ERROR && !hasErrors();
}

bool CHTLState::canTransitionTo(CHTLStateType targetState) const {
    switch (currentState) {
        case CHTLStateType::INITIAL:
            return targetState == CHTLStateType::PARSING || targetState == CHTLStateType::ERROR;
        case CHTLStateType::PARSING:
            return targetState == CHTLStateType::GENERATING || targetState == CHTLStateType::ERROR;
        case CHTLStateType::GENERATING:
            return targetState == CHTLStateType::COMPLETED || targetState == CHTLStateType::ERROR;
        case CHTLStateType::COMPLETED:
            return targetState == CHTLStateType::INITIAL || targetState == CHTLStateType::ERROR;
        case CHTLStateType::ERROR:
            return targetState == CHTLStateType::INITIAL;
        default:
            return false;
    }
}

bool CHTLState::transitionTo(CHTLStateType targetState) {
    if (canTransitionTo(targetState)) {
        currentState = targetState;
        return true;
    }
    return false;
}

std::string CHTLState::toString() const {
    std::ostringstream oss;
    oss << "CHTLState{";
    oss << "currentState=" << static_cast<int>(currentState) << ", ";
    oss << "stateData=" << stateData.size() << ", ";
    oss << "errors=" << errors.size() << ", ";
    oss << "warnings=" << warnings.size() << ", ";
    oss << "debugMode=" << (debugMode ? "true" : "false");
    oss << "}";
    return oss.str();
}

std::string CHTLState::toDebugString() const {
    std::ostringstream oss;
    oss << "CHTLState Debug Info:\n";
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

std::string CHTLState::getStateName() const {
    switch (currentState) {
        case CHTLStateType::INITIAL:
            return "INITIAL";
        case CHTLStateType::PARSING:
            return "PARSING";
        case CHTLStateType::GENERATING:
            return "GENERATING";
        case CHTLStateType::COMPLETED:
            return "COMPLETED";
        case CHTLStateType::ERROR:
            return "ERROR";
        default:
            return "UNKNOWN";
    }
}

std::string CHTLState::toJSON() const {
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

std::string CHTLState::toXML() const {
    std::ostringstream oss;
    oss << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    oss << "<CHTLState>\n";
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
    oss << "</CHTLState>";
    return oss.str();
}

std::string CHTLState::toYAML() const {
    std::ostringstream oss;
    oss << "CHTLState:\n";
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

CHTLState CHTLState::clone() const {
    CHTLState cloned;
    cloned.currentState = currentState;
    cloned.stateData = stateData;
    cloned.errors = errors;
    cloned.warnings = warnings;
    cloned.debugMode = debugMode;
    return cloned;
}

bool CHTLState::operator==(const CHTLState& other) const {
    return currentState == other.currentState &&
           stateData == other.stateData &&
           errors == other.errors &&
           warnings == other.warnings &&
           debugMode == other.debugMode;
}

bool CHTLState::operator!=(const CHTLState& other) const {
    return !(*this == other);
}

} // namespace CHTL