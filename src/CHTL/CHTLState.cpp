#include "CHTLState.h"

namespace CHTL {

CHTLState::CHTLState() : currentState("initial") {}

CHTLState::~CHTLState() {}

void CHTLState::setState(const std::string& state) {
    currentState = state;
}

std::string CHTLState::getCurrentState() const {
    return currentState;
}

void CHTLState::pushState(const std::string& state) {
    stateStack.push(currentState);
    currentState = state;
}

std::string CHTLState::popState() {
    if (stateStack.empty()) {
        return "";
    }
    
    std::string previousState = currentState;
    currentState = stateStack.top();
    stateStack.pop();
    
    return previousState;
}

bool CHTLState::isInState(const std::string& state) const {
    return currentState == state;
}

void CHTLState::setVariable(const std::string& name, const std::string& value) {
    variables[name] = value;
}

std::string CHTLState::getVariable(const std::string& name) const {
    auto it = variables.find(name);
    return it != variables.end() ? it->second : "";
}

bool CHTLState::hasVariable(const std::string& name) const {
    return variables.find(name) != variables.end();
}

void CHTLState::clear() {
    currentState = "initial";
    while (!stateStack.empty()) {
        stateStack.pop();
    }
    variables.clear();
}

} // namespace CHTL