#include "OriginManager.hpp"
#include <sstream>
#include <regex>

namespace CHTL {

OriginManager& OriginManager::getInstance() {
    static OriginManager instance;
    return instance;
}

void OriginManager::addOrigin(const std::string& name, OriginType type, 
                             const std::string& content) {
    addOrigin(name, type, content, {});
}

void OriginManager::addOrigin(const std::string& name, OriginType type, 
                             const std::string& content,
                             const std::map<std::string, std::string>& attributes) {
    if (!validateOriginContent(type, content)) {
        return;
    }
    
    auto originDef = std::make_shared<OriginDefinition>();
    originDef->type = type;
    originDef->name = name;
    originDef->content = processOriginContent(type, content);
    originDef->attributes = attributes;
    origins_[name] = originDef;
}

std::shared_ptr<OriginManager::OriginDefinition> OriginManager::getOrigin(const std::string& name) const {
    auto it = origins_.find(name);
    return (it != origins_.end()) ? it->second : nullptr;
}

bool OriginManager::hasOrigin(const std::string& name) const {
    return origins_.find(name) != origins_.end();
}

std::string OriginManager::getOriginContent(const std::string& name) const {
    auto originDef = getOrigin(name);
    return originDef ? originDef->content : "";
}

std::map<std::string, std::string> OriginManager::getOriginAttributes(const std::string& name) const {
    auto originDef = getOrigin(name);
    return originDef ? originDef->attributes : std::map<std::string, std::string>();
}

std::vector<std::string> OriginManager::getAllOriginNames() const {
    std::vector<std::string> names;
    for (const auto& [name, originDef] : origins_) {
        names.push_back(name);
    }
    return names;
}

std::vector<std::string> OriginManager::getOriginNamesByType(OriginType type) const {
    std::vector<std::string> names;
    for (const auto& [name, originDef] : origins_) {
        if (originDef->type == type) {
            names.push_back(name);
        }
    }
    return names;
}

void OriginManager::clear() {
    origins_.clear();
}

void OriginManager::removeOrigin(const std::string& name) {
    origins_.erase(name);
}

void OriginManager::registerCustomType(const std::string& typeName, OriginType type) {
    customTypes_[typeName] = type;
}

OriginManager::OriginType OriginManager::getCustomType(const std::string& typeName) const {
    auto it = customTypes_.find(typeName);
    return (it != customTypes_.end()) ? it->second : OriginType::CUSTOM;
}

bool OriginManager::isCustomType(const std::string& typeName) const {
    return customTypes_.find(typeName) != customTypes_.end();
}

bool OriginManager::validateOriginContent(OriginType type, const std::string& content) const {
    if (content.empty()) {
        return false;
    }
    
    switch (type) {
        case OriginType::HTML: {
            // 基本的HTML验证
            // 检查是否包含HTML标签
            std::regex htmlTagRegex(R"(<[^>]+>)");
            return std::regex_search(content, htmlTagRegex);
        }
        
        case OriginType::CSS: {
            // 基本的CSS验证
            // 检查是否包含CSS选择器或属性
            std::regex cssRegex(R"([.#]?[\w-]+\s*\{[^}]*\})");
            return std::regex_search(content, cssRegex);
        }
        
        case OriginType::JAVASCRIPT: {
            // 基本的JavaScript验证
            // 检查是否包含JavaScript关键字或语法
            std::regex jsRegex(R"(\b(function|var|let|const|if|for|while|return)\b)");
            return std::regex_search(content, jsRegex);
        }
        
        case OriginType::CUSTOM:
        default:
            // 自定义类型总是有效的
            return true;
    }
}

std::string OriginManager::processOriginContent(OriginType type, const std::string& content) const {
    std::string processed = content;
    
    switch (type) {
        case OriginType::HTML: {
            // HTML内容处理
            // 可以在这里添加HTML清理、格式化等逻辑
            break;
        }
        
        case OriginType::CSS: {
            // CSS内容处理
            // 可以在这里添加CSS清理、格式化等逻辑
            break;
        }
        
        case OriginType::JAVASCRIPT: {
            // JavaScript内容处理
            // 可以在这里添加JavaScript清理、格式化等逻辑
            break;
        }
        
        case OriginType::CUSTOM:
        default:
            // 自定义类型内容处理
            // 可以在这里添加自定义处理逻辑
            break;
    }
    
    return processed;
}

} // namespace CHTL