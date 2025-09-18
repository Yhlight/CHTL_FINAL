#include "CHTLJSContext.h"

namespace CHTL {

CHTLJSContext::CHTLJSContext() {
}

void CHTLJSContext::registerVirtualObject(const std::string& name, const std::string& content) {
    virtualObjects_[name] = content;
}

std::string CHTLJSContext::getVirtualObject(const std::string& name) const {
    auto it = virtualObjects_.find(name);
    return (it != virtualObjects_.end()) ? it->second : "";
}

bool CHTLJSContext::hasVirtualObject(const std::string& name) const {
    return virtualObjects_.find(name) != virtualObjects_.end();
}

void CHTLJSContext::registerFunction(const std::string& name, const std::string& content) {
    functions_[name] = content;
}

std::string CHTLJSContext::getFunction(const std::string& name) const {
    auto it = functions_.find(name);
    return (it != functions_.end()) ? it->second : "";
}

bool CHTLJSContext::hasFunction(const std::string& name) const {
    return functions_.find(name) != functions_.end();
}

void CHTLJSContext::registerEventDelegate(const std::string& parent, const std::string& target, const std::string& event, const std::string& handler) {
    std::string key = parent + ":" + target + ":" + event;
    eventDelegates_[key].push_back(handler);
}

std::vector<std::string> CHTLJSContext::getEventDelegates(const std::string& parent) const {
    std::vector<std::string> delegates;
    
    for (const auto& pair : eventDelegates_) {
        if (pair.first.find(parent) == 0) {
            delegates.insert(delegates.end(), pair.second.begin(), pair.second.end());
        }
    }
    
    return delegates;
}

void CHTLJSContext::registerRoute(const std::string& url, const std::string& page) {
    routes_[url] = page;
}

std::string CHTLJSContext::getRoute(const std::string& url) const {
    auto it = routes_.find(url);
    return (it != routes_.end()) ? it->second : "";
}

bool CHTLJSContext::hasRoute(const std::string& url) const {
    return routes_.find(url) != routes_.end();
}

void CHTLJSContext::registerAnimation(const std::string& name, const std::string& content) {
    animations_[name] = content;
}

std::string CHTLJSContext::getAnimation(const std::string& name) const {
    auto it = animations_.find(name);
    return (it != animations_.end()) ? it->second : "";
}

bool CHTLJSContext::hasAnimation(const std::string& name) const {
    return animations_.find(name) != animations_.end();
}

void CHTLJSContext::addError(const std::string& error) {
    errors_.push_back(error);
}

void CHTLJSContext::addWarning(const std::string& warning) {
    warnings_.push_back(warning);
}

void CHTLJSContext::clear() {
    virtualObjects_.clear();
    functions_.clear();
    eventDelegates_.clear();
    routes_.clear();
    animations_.clear();
    errors_.clear();
    warnings_.clear();
}

} // namespace CHTL