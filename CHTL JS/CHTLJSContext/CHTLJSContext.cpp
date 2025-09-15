#include "CHTLJSContext.h"
#include <iostream>

namespace CHTLJS {

CHTLJSContext::CHTLJSContext() {
    // 初始化默认配置
    setCompileOption("minify", false);
    setCompileOption("source_maps", true);
    setCompileOption("strict_mode", true);
}

CHTLJSContext::~CHTLJSContext() {
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

void CHTLJSContext::removeVariable(const std::string& name) {
    variables_.erase(name);
}

void CHTLJSContext::setReactiveVariable(const std::string& name, const std::string& value) {
    reactive_variables_[name] = value;
}

std::string CHTLJSContext::getReactiveVariable(const std::string& name) const {
    auto it = reactive_variables_.find(name);
    if (it != reactive_variables_.end()) {
        return it->second;
    }
    return "";
}

bool CHTLJSContext::hasReactiveVariable(const std::string& name) const {
    return reactive_variables_.find(name) != reactive_variables_.end();
}

void CHTLJSContext::registerVirtualObject(const std::string& name, std::function<std::string()> generator) {
    virtual_objects_[name] = generator;
}

std::string CHTLJSContext::generateVirtualObject(const std::string& name) const {
    auto it = virtual_objects_.find(name);
    if (it != virtual_objects_.end()) {
        return it->second();
    }
    return "";
}

bool CHTLJSContext::hasVirtualObject(const std::string& name) const {
    return virtual_objects_.find(name) != virtual_objects_.end();
}

void CHTLJSContext::addEventListener(const std::string& selector, const std::string& event, const std::string& handler) {
    std::string key = selector + ":" + event;
    event_listeners_[key].push_back(handler);
}

std::vector<std::string> CHTLJSContext::getEventListeners(const std::string& selector) const {
    std::vector<std::string> listeners;
    for (const auto& pair : event_listeners_) {
        if (pair.first.find(selector + ":") == 0) {
            listeners.insert(listeners.end(), pair.second.begin(), pair.second.end());
        }
    }
    return listeners;
}

void CHTLJSContext::addAnimation(const std::string& name, const std::string& animation) {
    animations_[name] = animation;
}

std::string CHTLJSContext::getAnimation(const std::string& name) const {
    auto it = animations_.find(name);
    if (it != animations_.end()) {
        return it->second;
    }
    return "";
}

bool CHTLJSContext::hasAnimation(const std::string& name) const {
    return animations_.find(name) != animations_.end();
}

void CHTLJSContext::addRoute(const std::string& path, const std::string& handler) {
    routes_[path] = handler;
}

std::string CHTLJSContext::getRouteHandler(const std::string& path) const {
    auto it = routes_.find(path);
    if (it != routes_.end()) {
        return it->second;
    }
    return "";
}

bool CHTLJSContext::hasRoute(const std::string& path) const {
    return routes_.find(path) != routes_.end();
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

void CHTLJSContext::setConfiguration(const std::string& key, const std::string& value) {
    configurations_[key] = value;
}

std::string CHTLJSContext::getConfiguration(const std::string& key) const {
    auto it = configurations_.find(key);
    if (it != configurations_.end()) {
        return it->second;
    }
    return "";
}

bool CHTLJSContext::hasConfiguration(const std::string& key) const {
    return configurations_.find(key) != configurations_.end();
}

void CHTLJSContext::setCompileOption(const std::string& option, bool value) {
    compile_options_[option] = value;
}

bool CHTLJSContext::getCompileOption(const std::string& option) const {
    auto it = compile_options_.find(option);
    if (it != compile_options_.end()) {
        return it->second;
    }
    return false;
}

void CHTLJSContext::clear() {
    variables_.clear();
    reactive_variables_.clear();
    virtual_objects_.clear();
    event_listeners_.clear();
    animations_.clear();
    routes_.clear();
    modules_.clear();
    configurations_.clear();
    compile_options_.clear();
}

} // namespace CHTLJS