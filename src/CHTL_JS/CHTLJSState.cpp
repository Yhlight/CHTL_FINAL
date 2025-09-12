#include "CHTLJSState.h"

namespace CHTL {

CHTLJSState::CHTLJSState() : currentState("initial") {}

CHTLJSState::~CHTLJSState() {}

void CHTLJSState::setState(const std::string& state) {
    currentState = state;
}

std::string CHTLJSState::getCurrentState() const {
    return currentState;
}

void CHTLJSState::pushState(const std::string& state) {
    stateStack.push(currentState);
    currentState = state;
}

std::string CHTLJSState::popState() {
    if (stateStack.empty()) {
        return "";
    }
    
    std::string previousState = currentState;
    currentState = stateStack.top();
    stateStack.pop();
    
    return previousState;
}

bool CHTLJSState::isInState(const std::string& state) const {
    return currentState == state;
}

void CHTLJSState::setVariable(const std::string& name, const std::string& value) {
    variables[name] = value;
}

std::string CHTLJSState::getVariable(const std::string& name) const {
    auto it = variables.find(name);
    return it != variables.end() ? it->second : "";
}

bool CHTLJSState::hasVariable(const std::string& name) const {
    return variables.find(name) != variables.end();
}

void CHTLJSState::clear() {
    currentState = "initial";
    while (!stateStack.empty()) {
        stateStack.pop();
    }
    variables.clear();
}

} // namespace CHTL