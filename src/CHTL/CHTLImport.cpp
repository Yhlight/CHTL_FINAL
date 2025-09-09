#include "CHTL/CHTLImport.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <regex>

namespace CHTL {

// ImportItem 实现
ImportItem::ImportItem(ImportType type, const std::string& name, const std::string& path, 
                       const std::string& alias, ImportMode mode)
    : m_type(type), m_name(name), m_path(path), m_alias(alias), m_mode(mode), m_resolved(false) {
}

std::string ImportItem::resolvePath(const std::string& basePath) const {
    if (m_path.empty()) {
        return "";
    }
    
    // 处理相对路径
    if (m_path[0] != '/' && m_path[0] != '\\' && 
        (m_path.length() < 2 || m_path[1] != ':')) {
        return basePath + "/" + m_path;
    }
    
    return m_path;
}

// Namespace 实现
Namespace::Namespace(const std::string& name, const std::string& parent)
    : m_name(name), m_parent(parent) {
}

std::string Namespace::getFullName() const {
    if (m_parent.empty()) {
        return m_name;
    }
    return m_parent + "." + m_name;
}

void Namespace::addChild(std::shared_ptr<Namespace> child) {
    if (child) {
        m_children.push_back(child);
    }
}

std::shared_ptr<Namespace> Namespace::getChild(const std::string& name) const {
    for (const auto& child : m_children) {
        if (child->getName() == name) {
            return child;
        }
    }
    return nullptr;
}

const std::vector<std::shared_ptr<Namespace>>& Namespace::getChildren() const {
    return m_children;
}

void Namespace::addTemplate(const std::string& name, std::shared_ptr<BaseNode> template_) {
    m_templates[name] = template_;
}

void Namespace::addCustom(const std::string& name, std::shared_ptr<BaseNode> custom) {
    m_customs[name] = custom;
}

void Namespace::addOrigin(const std::string& name, std::shared_ptr<BaseNode> origin) {
    m_origins[name] = origin;
}

std::shared_ptr<BaseNode> Namespace::getTemplate(const std::string& name) const {
    auto it = m_templates.find(name);
    return (it != m_templates.end()) ? it->second : nullptr;
}

std::shared_ptr<BaseNode> Namespace::getCustom(const std::string& name) const {
    auto it = m_customs.find(name);
    return (it != m_customs.end()) ? it->second : nullptr;
}

std::shared_ptr<BaseNode> Namespace::getOrigin(const std::string& name) const {
    auto it = m_origins.find(name);
    return (it != m_origins.end()) ? it->second : nullptr;
}

bool Namespace::hasConflict(const std::string& name) const {
    return m_templates.find(name) != m_templates.end() ||
           m_customs.find(name) != m_customs.end() ||
           m_origins.find(name) != m_origins.end();
}

void Namespace::resolveConflicts() {
    // 解决命名空间内的冲突
    // 这里可以实现具体的冲突解决策略
}

// ImportManager 实现
ImportManager::ImportManager(std::shared_ptr<CHTLContext> context)
    : m_context(context) {
}

void ImportManager::addImport(std::shared_ptr<ImportItem> importItem) {
    if (importItem) {
        m_imports.push_back(importItem);
        m_importsByType[importItem->getType()].push_back(importItem);
    }
}

void ImportManager::addImport(const std::string& type, const std::string& name, 
                             const std::string& path, const std::string& alias) {
    ImportType importType = parseImportType(type);
    auto importItem = std::make_shared<ImportItem>(importType, name, path, alias);
    addImport(importItem);
}

void ImportManager::resolveImports(const std::string& basePath) {
    for (auto& importItem : m_imports) {
        if (!importItem->isResolved()) {
            std::string resolvedPath = importItem->resolvePath(basePath);
            if (fileExists(resolvedPath)) {
                importItem->setResolved(true);
            } else {
                m_context->addWarning("Import file not found: " + resolvedPath);
            }
        }
    }
}

std::vector<std::shared_ptr<ImportItem>> ImportManager::getImports(ImportType type) const {
    auto it = m_importsByType.find(type);
    return (it != m_importsByType.end()) ? it->second : std::vector<std::shared_ptr<ImportItem>>();
}

std::vector<std::shared_ptr<ImportItem>> ImportManager::getAllImports() const {
    return m_imports;
}

std::string ImportManager::loadFile(const std::string& path) const {
    std::ifstream file(path);
    if (!file.is_open()) {
        m_context->addError("Cannot open file: " + path);
        return "";
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

bool ImportManager::fileExists(const std::string& path) const {
    return std::filesystem::exists(path);
}

std::string ImportManager::resolvePath(const std::string& path, const std::string& basePath) const {
    if (path.empty()) {
        return "";
    }
    
    // 处理相对路径
    if (path[0] != '/' && path[0] != '\\' && 
        (path.length() < 2 || path[1] != ':')) {
        return basePath + "/" + path;
    }
    
    return path;
}

std::vector<std::string> ImportManager::findFiles(const std::string& pattern, const std::string& basePath) const {
    std::vector<std::string> files;
    
    if (isWildcardPattern(pattern)) {
        files = expandWildcard(pattern, basePath);
    } else {
        std::string resolvedPath = resolvePath(pattern, basePath);
        if (fileExists(resolvedPath)) {
            files.push_back(resolvedPath);
        }
    }
    
    return files;
}

void ImportManager::clear() {
    m_imports.clear();
    m_importsByType.clear();
}

void ImportManager::removeImport(const std::string& name) {
    m_imports.erase(
        std::remove_if(m_imports.begin(), m_imports.end(),
            [&name](const std::shared_ptr<ImportItem>& item) {
                return item->getName() == name;
            }),
        m_imports.end()
    );
    
    // 同时从类型映射中移除
    for (auto& [type, imports] : m_importsByType) {
        imports.erase(
            std::remove_if(imports.begin(), imports.end(),
                [&name](const std::shared_ptr<ImportItem>& item) {
                    return item->getName() == name;
                }),
            imports.end()
        );
    }
}

ImportType ImportManager::parseImportType(const std::string& typeString) const {
    if (typeString == "@Html") {
        return ImportType::HTML;
    } else if (typeString == "@Style") {
        return ImportType::CSS;
    } else if (typeString == "@JavaScript") {
        return ImportType::JAVASCRIPT;
    } else if (typeString == "@Chtl") {
        return ImportType::CHTL;
    } else if (typeString == "@Cjmod") {
        return ImportType::CJMOD;
    } else {
        return ImportType::UNKNOWN;
    }
}

std::string ImportManager::getFileExtension(const std::string& path) const {
    size_t dotPos = path.find_last_of('.');
    if (dotPos != std::string::npos) {
        return path.substr(dotPos + 1);
    }
    return "";
}

bool ImportManager::isWildcardPattern(const std::string& pattern) const {
    return pattern.find('*') != std::string::npos || pattern.find('?') != std::string::npos;
}

std::vector<std::string> ImportManager::expandWildcard(const std::string& pattern, const std::string& basePath) const {
    std::vector<std::string> files;
    
    try {
        for (const auto& entry : std::filesystem::directory_iterator(basePath)) {
            if (entry.is_regular_file()) {
                std::string filename = entry.path().filename().string();
                
                // 简单的通配符匹配
                if (pattern == "*" || pattern == "*.*") {
                    files.push_back(entry.path().string());
                } else if (pattern.ends_with(".*")) {
                    std::string prefix = pattern.substr(0, pattern.length() - 2);
                    if (filename.starts_with(prefix)) {
                        files.push_back(entry.path().string());
                    }
                } else if (pattern.ends_with(".chtl")) {
                    if (filename.ends_with(".chtl")) {
                        files.push_back(entry.path().string());
                    }
                } else if (pattern.ends_with(".cmod")) {
                    if (filename.ends_with(".cmod")) {
                        files.push_back(entry.path().string());
                    }
                } else if (pattern.ends_with(".cjmod")) {
                    if (filename.ends_with(".cjmod")) {
                        files.push_back(entry.path().string());
                    }
                }
            }
        }
    } catch (const std::filesystem::filesystem_error& e) {
        m_context->addWarning("Error reading directory: " + std::string(e.what()));
    }
    
    return files;
}

// NamespaceManager 实现
NamespaceManager::NamespaceManager(std::shared_ptr<CHTLContext> context)
    : m_context(context) {
}

void NamespaceManager::createNamespace(const std::string& name, const std::string& parent) {
    auto namespace_ = std::make_shared<Namespace>(name, parent);
    m_namespaces[name] = namespace_;
}

std::shared_ptr<Namespace> NamespaceManager::getNamespace(const std::string& name) const {
    auto it = m_namespaces.find(name);
    return (it != m_namespaces.end()) ? it->second : nullptr;
}

std::shared_ptr<Namespace> NamespaceManager::getNamespace(const std::string& name, const std::string& parent) const {
    std::string fullName = parent.empty() ? name : parent + "." + name;
    return getNamespace(fullName);
}

void NamespaceManager::registerTemplate(const std::string& namespaceName, const std::string& name, 
                                       std::shared_ptr<BaseNode> template_) {
    auto namespace_ = getNamespace(namespaceName);
    if (namespace_) {
        namespace_->addTemplate(name, template_);
    } else {
        m_context->addWarning("Namespace not found: " + namespaceName);
    }
}

void NamespaceManager::registerCustom(const std::string& namespaceName, const std::string& name, 
                                     std::shared_ptr<BaseNode> custom) {
    auto namespace_ = getNamespace(namespaceName);
    if (namespace_) {
        namespace_->addCustom(name, custom);
    } else {
        m_context->addWarning("Namespace not found: " + namespaceName);
    }
}

void NamespaceManager::registerOrigin(const std::string& namespaceName, const std::string& name, 
                                     std::shared_ptr<BaseNode> origin) {
    auto namespace_ = getNamespace(namespaceName);
    if (namespace_) {
        namespace_->addOrigin(name, origin);
    } else {
        m_context->addWarning("Namespace not found: " + namespaceName);
    }
}

std::shared_ptr<BaseNode> NamespaceManager::findTemplate(const std::string& name, 
                                                        const std::string& namespaceName) const {
    if (namespaceName.empty()) {
        // 在所有命名空间中查找
        for (const auto& [nsName, ns] : m_namespaces) {
            auto template_ = ns->getTemplate(name);
            if (template_) {
                return template_;
            }
        }
    } else {
        auto namespace_ = getNamespace(namespaceName);
        if (namespace_) {
            return namespace_->getTemplate(name);
        }
    }
    
    return nullptr;
}

std::shared_ptr<BaseNode> NamespaceManager::findCustom(const std::string& name, 
                                                      const std::string& namespaceName) const {
    if (namespaceName.empty()) {
        // 在所有命名空间中查找
        for (const auto& [nsName, ns] : m_namespaces) {
            auto custom = ns->getCustom(name);
            if (custom) {
                return custom;
            }
        }
    } else {
        auto namespace_ = getNamespace(namespaceName);
        if (namespace_) {
            return namespace_->getCustom(name);
        }
    }
    
    return nullptr;
}

std::shared_ptr<BaseNode> NamespaceManager::findOrigin(const std::string& name, 
                                                      const std::string& namespaceName) const {
    if (namespaceName.empty()) {
        // 在所有命名空间中查找
        for (const auto& [nsName, ns] : m_namespaces) {
            auto origin = ns->getOrigin(name);
            if (origin) {
                return origin;
            }
        }
    } else {
        auto namespace_ = getNamespace(namespaceName);
        if (namespace_) {
            return namespace_->getOrigin(name);
        }
    }
    
    return nullptr;
}

void NamespaceManager::resolveConflicts() {
    // 解决命名空间间的冲突
    for (const auto& [name, namespace_] : m_namespaces) {
        namespace_->resolveConflicts();
    }
}

bool NamespaceManager::hasConflict(const std::string& name, const std::string& namespaceName) const {
    if (namespaceName.empty()) {
        // 检查所有命名空间
        for (const auto& [nsName, ns] : m_namespaces) {
            if (ns->hasConflict(name)) {
                return true;
            }
        }
    } else {
        auto namespace_ = getNamespace(namespaceName);
        if (namespace_) {
            return namespace_->hasConflict(name);
        }
    }
    
    return false;
}

void NamespaceManager::clear() {
    m_namespaces.clear();
}

void NamespaceManager::removeNamespace(const std::string& name) {
    m_namespaces.erase(name);
}

std::string NamespaceManager::parseNamespaceName(const std::string& fullName) const {
    size_t dotPos = fullName.find_last_of('.');
    if (dotPos != std::string::npos) {
        return fullName.substr(dotPos + 1);
    }
    return fullName;
}

std::string NamespaceManager::parseParentName(const std::string& fullName) const {
    size_t dotPos = fullName.find_last_of('.');
    if (dotPos != std::string::npos) {
        return fullName.substr(0, dotPos);
    }
    return "";
}

void NamespaceManager::mergeNamespaces(std::shared_ptr<Namespace> target, std::shared_ptr<Namespace> source) {
    // 合并命名空间内容
    // 这里可以实现具体的合并逻辑
}

// ImportParser 实现
ImportParser::ImportParser(std::shared_ptr<CHTLContext> context)
    : m_context(context) {
}

std::shared_ptr<ImportItem> ImportParser::parseImport(std::shared_ptr<ImportNode> node) {
    if (!node) {
        return nullptr;
    }
    
    ImportType type = parseImportType(node->getImportType());
    std::string name = node->getAttribute("name");
    std::string path = node->getPath();
    std::string alias = node->getAlias();
    
    return std::make_shared<ImportItem>(type, name, path, alias);
}

std::vector<std::shared_ptr<ImportItem>> ImportParser::parseImports(std::shared_ptr<BaseNode> root) {
    std::vector<std::shared_ptr<ImportItem>> imports;
    
    if (!root) {
        return imports;
    }
    
    // 递归查找导入节点
    std::function<void(std::shared_ptr<BaseNode>)> findImports = 
        [&](std::shared_ptr<BaseNode> node) {
            if (node->getType() == NodeType::IMPORT) {
                auto importNode = std::static_pointer_cast<ImportNode>(node);
                auto importItem = parseImport(importNode);
                if (importItem) {
                    imports.push_back(importItem);
                }
            }
            
            for (const auto& child : node->getChildren()) {
                findImports(child);
            }
        };
    
    findImports(root);
    return imports;
}

std::shared_ptr<Namespace> ImportParser::parseNamespace(std::shared_ptr<NamespaceNode> node) {
    if (!node) {
        return nullptr;
    }
    
    std::string name = node->getName();
    auto namespace_ = std::make_shared<Namespace>(name);
    
    // 解析命名空间内容
    for (const auto& child : node->getChildren()) {
        // 这里需要根据具体的节点类型来注册到命名空间
        // 暂时跳过具体实现
    }
    
    return namespace_;
}

std::vector<std::shared_ptr<Namespace>> ImportParser::parseNamespaces(std::shared_ptr<BaseNode> root) {
    std::vector<std::shared_ptr<Namespace>> namespaces;
    
    if (!root) {
        return namespaces;
    }
    
    // 递归查找命名空间节点
    std::function<void(std::shared_ptr<BaseNode>)> findNamespaces = 
        [&](std::shared_ptr<BaseNode> node) {
            if (node->getType() == NodeType::NAMESPACE) {
                auto namespaceNode = std::static_pointer_cast<NamespaceNode>(node);
                auto namespace_ = parseNamespace(namespaceNode);
                if (namespace_) {
                    namespaces.push_back(namespace_);
                }
            }
            
            for (const auto& child : node->getChildren()) {
                findNamespaces(child);
            }
        };
    
    findNamespaces(root);
    return namespaces;
}

ImportType ImportParser::parseImportType(const std::string& typeString) const {
    if (typeString == "@Html") {
        return ImportType::HTML;
    } else if (typeString == "@Style") {
        return ImportType::CSS;
    } else if (typeString == "@JavaScript") {
        return ImportType::JAVASCRIPT;
    } else if (typeString == "@Chtl") {
        return ImportType::CHTL;
    } else if (typeString == "@Cjmod") {
        return ImportType::CJMOD;
    } else {
        return ImportType::UNKNOWN;
    }
}

ImportMode ImportParser::parseImportMode(const std::string& modeString) const {
    if (modeString == "exact") {
        return ImportMode::EXACT;
    } else if (modeString == "type") {
        return ImportMode::TYPE;
    } else if (modeString == "wildcard") {
        return ImportMode::WILDCARD;
    } else {
        return ImportMode::EXACT;
    }
}

std::string ImportParser::parseImportPath(const std::string& pathString) const {
    return pathString;
}

std::string ImportParser::parseImportAlias(const std::string& aliasString) const {
    return aliasString;
}

// ImportProcessor 实现
ImportProcessor::ImportProcessor(std::shared_ptr<CHTLContext> context)
    : m_context(context) {
    m_importManager = std::make_unique<ImportManager>(context);
    m_namespaceManager = std::make_unique<NamespaceManager>(context);
    m_parser = std::make_unique<ImportParser>(context);
}

void ImportProcessor::processImports(std::shared_ptr<BaseNode> root, const std::string& basePath) {
    if (!root) {
        return;
    }
    
    // 解析导入
    auto imports = m_parser->parseImports(root);
    for (const auto& import : imports) {
        m_importManager->addImport(import);
    }
    
    // 解析命名空间
    auto namespaces = m_parser->parseNamespaces(root);
    for (const auto& namespace_ : namespaces) {
        m_namespaceManager->createNamespace(namespace_->getName());
    }
    
    // 解析导入
    m_importManager->resolveImports(basePath);
    
    // 处理各种类型的导入
    for (const auto& import : imports) {
        switch (import->getType()) {
            case ImportType::HTML:
                processHtmlImport(import);
                break;
            case ImportType::CSS:
                processCssImport(import);
                break;
            case ImportType::JAVASCRIPT:
                processJavaScriptImport(import);
                break;
            case ImportType::CHTL:
                processChtlImport(import);
                break;
            case ImportType::CJMOD:
                processCjmodImport(import);
                break;
            default:
                break;
        }
    }
}

void ImportProcessor::mergeImportedContent(std::shared_ptr<BaseNode> root) {
    // 合并导入的内容到根节点
    // 这里需要根据具体的导入类型来合并内容
}

void ImportProcessor::processNamespaces(std::shared_ptr<BaseNode> root) {
    // 处理命名空间
    m_namespaceManager->resolveConflicts();
}

void ImportProcessor::resolveConflicts() {
    // 解决冲突
    m_namespaceManager->resolveConflicts();
}

void ImportProcessor::processHtmlImport(std::shared_ptr<ImportItem> importItem) {
    if (importItem->isResolved()) {
        std::string content = m_importManager->loadFile(importItem->resolvePath(""));
        // 处理 HTML 导入
    }
}

void ImportProcessor::processCssImport(std::shared_ptr<ImportItem> importItem) {
    if (importItem->isResolved()) {
        std::string content = m_importManager->loadFile(importItem->resolvePath(""));
        // 处理 CSS 导入
    }
}

void ImportProcessor::processJavaScriptImport(std::shared_ptr<ImportItem> importItem) {
    if (importItem->isResolved()) {
        std::string content = m_importManager->loadFile(importItem->resolvePath(""));
        // 处理 JavaScript 导入
    }
}

void ImportProcessor::processChtlImport(std::shared_ptr<ImportItem> importItem) {
    if (importItem->isResolved()) {
        std::string content = m_importManager->loadFile(importItem->resolvePath(""));
        // 处理 CHTL 导入
    }
}

void ImportProcessor::processCjmodImport(std::shared_ptr<ImportItem> importItem) {
    if (importItem->isResolved()) {
        std::string content = m_importManager->loadFile(importItem->resolvePath(""));
        // 处理 CJMOD 导入
    }
}

void ImportProcessor::mergeHtmlContent(const std::string& content, std::shared_ptr<BaseNode> root) {
    // 合并 HTML 内容
}

void ImportProcessor::mergeCssContent(const std::string& content, std::shared_ptr<BaseNode> root) {
    // 合并 CSS 内容
}

void ImportProcessor::mergeJavaScriptContent(const std::string& content, std::shared_ptr<BaseNode> root) {
    // 合并 JavaScript 内容
}

void ImportProcessor::mergeChtlContent(const std::string& content, std::shared_ptr<BaseNode> root) {
    // 合并 CHTL 内容
}

} // namespace CHTL