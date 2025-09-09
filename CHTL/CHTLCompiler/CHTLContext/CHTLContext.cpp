#include "CHTLContext.h"
#include <stdexcept>

namespace CHTL {

CHTLContext::CHTLContext() : config() {}

CHTLContext::CHTLContext(const CHTLConfig& cfg) : config(cfg) {}

void CHTLContext::setConfig(const CHTLConfig& cfg) {
    config = cfg;
}

const CHTLConfig& CHTLContext::getConfig() const {
    return config;
}

void CHTLContext::setCurrentFile(const std::string& file_path) {
    current_file = file_path;
}

const std::string& CHTLContext::getCurrentFile() const {
    return current_file;
}

void CHTLContext::pushNamespace(const std::string& namespace_name) {
    namespace_stack.push_back(namespace_name);
}

void CHTLContext::popNamespace() {
    if (!namespace_stack.empty()) {
        namespace_stack.pop_back();
    }
}

const std::string& CHTLContext::getCurrentNamespace() const {
    static const std::string empty;
    if (namespace_stack.empty()) {
        return empty;
    }
    return namespace_stack.back();
}

bool CHTLContext::hasCurrentNamespace() const {
    return !namespace_stack.empty();
}

void CHTLContext::registerTemplate(std::shared_ptr<TemplateInfo> info) {
    std::string key = getFullName(info->name);
    template_registry[key] = info;
}

std::shared_ptr<TemplateInfo> CHTLContext::getTemplate(const std::string& name) const {
    // First try with current namespace
    if (hasCurrentNamespace()) {
        std::string key = getCurrentNamespace() + "::" + name;
        auto it = template_registry.find(key);
        if (it != template_registry.end()) {
            return it->second;
        }
    }
    
    // Then try without namespace
    auto it = template_registry.find(name);
    if (it != template_registry.end()) {
        return it->second;
    }
    
    return nullptr;
}

void CHTLContext::registerCustom(std::shared_ptr<CustomInfo> info) {
    std::string key = getFullName(info->name);
    custom_registry[key] = info;
}

std::shared_ptr<CustomInfo> CHTLContext::getCustom(const std::string& name) const {
    // First try with current namespace
    if (hasCurrentNamespace()) {
        std::string key = getCurrentNamespace() + "::" + name;
        auto it = custom_registry.find(key);
        if (it != custom_registry.end()) {
            return it->second;
        }
    }
    
    // Then try without namespace
    auto it = custom_registry.find(name);
    if (it != custom_registry.end()) {
        return it->second;
    }
    
    return nullptr;
}

void CHTLContext::cacheImport(const std::string& path, std::shared_ptr<ImportInfo> info) {
    import_cache[path] = info;
}

std::shared_ptr<ImportInfo> CHTLContext::getCachedImport(const std::string& path) const {
    auto it = import_cache.find(path);
    if (it != import_cache.end()) {
        return it->second;
    }
    return nullptr;
}

std::string CHTLContext::getFullName(const std::string& name) const {
    if (hasCurrentNamespace()) {
        return getCurrentNamespace() + "::" + name;
    }
    return name;
}

void CHTLContext::clear() {
    namespace_stack.clear();
    template_registry.clear();
    custom_registry.clear();
    import_cache.clear();
    current_file.clear();
}

} // namespace CHTL