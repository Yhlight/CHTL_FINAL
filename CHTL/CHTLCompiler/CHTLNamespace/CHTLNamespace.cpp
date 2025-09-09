#include "CHTLNamespace.h"
#include <algorithm>
#include <sstream>
#include <regex>

namespace CHTL {

CHTLNamespaceManager::CHTLNamespaceManager() {
    // Initialize with global namespace
    auto global_ns = std::make_shared<NamespaceDefinition>();
    global_ns->name = "global";
    global_ns->parent_name = "";
    global_ns->type = NamespaceType::Global;
    global_ns->is_default = true;
    namespaces["global"] = global_ns;
    default_namespace = "global";
}

CHTLNamespaceManager::~CHTLNamespaceManager() {
    // Cleanup
}

bool CHTLNamespaceManager::createNamespace(const std::string& name, const std::string& parent, bool is_default) {
    if (!validateNamespaceName(name)) {
        return false;
    }
    
    std::string key = generateNamespaceKey(name, parent);
    if (namespaces.find(key) != namespaces.end()) {
        return false; // Namespace already exists
    }
    
    auto ns = std::make_shared<NamespaceDefinition>();
    ns->name = name;
    ns->parent_name = parent;
    ns->type = (parent.empty()) ? NamespaceType::Global : NamespaceType::Nested;
    ns->is_default = is_default;
    
    namespaces[key] = ns;
    
    // Add to parent's child list
    if (!parent.empty()) {
        std::string parent_key = generateNamespaceKey(parent, "");
        auto parent_it = namespaces.find(parent_key);
        if (parent_it != namespaces.end()) {
            parent_it->second->child_namespaces.push_back(name);
        }
    }
    
    if (is_default) {
        default_namespace = name;
    }
    
    updateConflicts();
    return true;
}

bool CHTLNamespaceManager::deleteNamespace(const std::string& name) {
    std::string key = generateNamespaceKey(name, "");
    auto it = namespaces.find(key);
    if (it == namespaces.end()) {
        return false;
    }
    
    // Check if namespace has children
    if (!it->second->child_namespaces.empty()) {
        return false; // Cannot delete namespace with children
    }
    
    // Remove from parent's child list
    if (!it->second->parent_name.empty()) {
        std::string parent_key = generateNamespaceKey(it->second->parent_name, "");
        auto parent_it = namespaces.find(parent_key);
        if (parent_it != namespaces.end()) {
            auto& children = parent_it->second->child_namespaces;
            children.erase(std::remove(children.begin(), children.end(), name), children.end());
        }
    }
    
    namespaces.erase(it);
    updateConflicts();
    return true;
}

bool CHTLNamespaceManager::renameNamespace(const std::string& old_name, const std::string& new_name) {
    if (!validateNamespaceName(new_name)) {
        return false;
    }
    
    std::string old_key = generateNamespaceKey(old_name, "");
    auto it = namespaces.find(old_key);
    if (it == namespaces.end()) {
        return false;
    }
    
    std::string new_key = generateNamespaceKey(new_name, it->second->parent_name);
    if (namespaces.find(new_key) != namespaces.end()) {
        return false; // New name already exists
    }
    
    // Update namespace
    it->second->name = new_name;
    namespaces[new_key] = it->second;
    namespaces.erase(old_key);
    
    // Update parent's child list
    if (!it->second->parent_name.empty()) {
        std::string parent_key = generateNamespaceKey(it->second->parent_name, "");
        auto parent_it = namespaces.find(parent_key);
        if (parent_it != namespaces.end()) {
            auto& children = parent_it->second->child_namespaces;
            std::replace(children.begin(), children.end(), old_name, new_name);
        }
    }
    
    updateConflicts();
    return true;
}

bool CHTLNamespaceManager::addSymbol(const std::string& namespace_name, const std::string& symbol_name, const std::string& symbol_value) {
    if (!validateSymbol(symbol_name)) {
        return false;
    }
    
    std::string key = generateNamespaceKey(namespace_name, "");
    auto it = namespaces.find(key);
    if (it == namespaces.end()) {
        return false;
    }
    
    // Check for conflicts
    if (checkSymbolConflict(symbol_name, namespace_name)) {
        return false;
    }
    
    it->second->symbols[symbol_name] = symbol_value;
    updateConflicts();
    return true;
}

bool CHTLNamespaceManager::removeSymbol(const std::string& namespace_name, const std::string& symbol_name) {
    std::string key = generateNamespaceKey(namespace_name, "");
    auto it = namespaces.find(key);
    if (it == namespaces.end()) {
        return false;
    }
    
    auto symbol_it = it->second->symbols.find(symbol_name);
    if (symbol_it == it->second->symbols.end()) {
        return false;
    }
    
    it->second->symbols.erase(symbol_it);
    updateConflicts();
    return true;
}

bool CHTLNamespaceManager::updateSymbol(const std::string& namespace_name, const std::string& symbol_name, const std::string& symbol_value) {
    std::string key = generateNamespaceKey(namespace_name, "");
    auto it = namespaces.find(key);
    if (it == namespaces.end()) {
        return false;
    }
    
    auto symbol_it = it->second->symbols.find(symbol_name);
    if (symbol_it == it->second->symbols.end()) {
        return false;
    }
    
    symbol_it->second = symbol_value;
    updateConflicts();
    return true;
}

std::string CHTLNamespaceManager::resolveSymbol(const std::string& symbol_name, const std::string& current_namespace) {
    // First try current namespace
    if (!current_namespace.empty()) {
        std::string key = generateNamespaceKey(current_namespace, "");
        auto it = namespaces.find(key);
        if (it != namespaces.end()) {
            auto symbol_it = it->second->symbols.find(symbol_name);
            if (symbol_it != it->second->symbols.end()) {
                return symbol_it->second;
            }
        }
    }
    
    // Try default namespace
    if (current_namespace != default_namespace) {
        std::string key = generateNamespaceKey(default_namespace, "");
        auto it = namespaces.find(key);
        if (it != namespaces.end()) {
            auto symbol_it = it->second->symbols.find(symbol_name);
            if (symbol_it != it->second->symbols.end()) {
                return symbol_it->second;
            }
        }
    }
    
    // Try global namespace
    if (current_namespace != "global" && default_namespace != "global") {
        std::string key = generateNamespaceKey("global", "");
        auto it = namespaces.find(key);
        if (it != namespaces.end()) {
            auto symbol_it = it->second->symbols.find(symbol_name);
            if (symbol_it != it->second->symbols.end()) {
                return symbol_it->second;
            }
        }
    }
    
    return "";
}

std::string CHTLNamespaceManager::resolveSymbolWithNamespace(const std::string& full_symbol_name) {
    // Parse namespace::symbol format
    size_t pos = full_symbol_name.find("::");
    if (pos == std::string::npos) {
        return resolveSymbol(full_symbol_name);
    }
    
    std::string namespace_name = full_symbol_name.substr(0, pos);
    std::string symbol_name = full_symbol_name.substr(pos + 2);
    
    std::string key = generateNamespaceKey(namespace_name, "");
    auto it = namespaces.find(key);
    if (it == namespaces.end()) {
        return "";
    }
    
    auto symbol_it = it->second->symbols.find(symbol_name);
    if (symbol_it == it->second->symbols.end()) {
        return "";
    }
    
    return symbol_it->second;
}

std::vector<std::string> CHTLNamespaceManager::findSymbols(const std::string& pattern, const std::string& namespace_name) {
    std::vector<std::string> matches;
    
    if (namespace_name.empty()) {
        // Search all namespaces
        for (const auto& pair : namespaces) {
            for (const auto& symbol_pair : pair.second->symbols) {
                if (symbol_pair.first.find(pattern) != std::string::npos) {
                    matches.push_back(symbol_pair.first);
                }
            }
        }
    } else {
        std::string key = generateNamespaceKey(namespace_name, "");
        auto it = namespaces.find(key);
        if (it != namespaces.end()) {
            for (const auto& symbol_pair : it->second->symbols) {
                if (symbol_pair.first.find(pattern) != std::string::npos) {
                    matches.push_back(symbol_pair.first);
                }
            }
        }
    }
    
    return matches;
}

bool CHTLNamespaceManager::mergeNamespaces(const std::string& source_namespace, const std::string& target_namespace) {
    std::string source_key = generateNamespaceKey(source_namespace, "");
    std::string target_key = generateNamespaceKey(target_namespace, "");
    
    auto source_it = namespaces.find(source_key);
    auto target_it = namespaces.find(target_key);
    
    if (source_it == namespaces.end() || target_it == namespaces.end()) {
        return false;
    }
    
    // Merge symbols
    for (const auto& symbol_pair : source_it->second->symbols) {
        if (target_it->second->symbols.find(symbol_pair.first) == target_it->second->symbols.end()) {
            target_it->second->symbols[symbol_pair.first] = symbol_pair.second;
        }
    }
    
    // Merge child namespaces
    for (const auto& child : source_it->second->child_namespaces) {
        if (std::find(target_it->second->child_namespaces.begin(), target_it->second->child_namespaces.end(), child) == target_it->second->child_namespaces.end()) {
            target_it->second->child_namespaces.push_back(child);
        }
    }
    
    updateConflicts();
    return true;
}

bool CHTLNamespaceManager::splitNamespace(const std::string& namespace_name, const std::vector<std::string>& new_namespaces) {
    std::string key = generateNamespaceKey(namespace_name, "");
    auto it = namespaces.find(key);
    if (it == namespaces.end()) {
        return false;
    }
    
    // Create new namespaces
    for (const auto& new_name : new_namespaces) {
        if (!createNamespace(new_name, it->second->parent_name)) {
            return false;
        }
    }
    
    // Distribute symbols (simplified - just move all to first new namespace)
    if (!new_namespaces.empty()) {
        std::string first_key = generateNamespaceKey(new_namespaces[0], "");
        auto first_it = namespaces.find(first_key);
        if (first_it != namespaces.end()) {
            first_it->second->symbols = it->second->symbols;
        }
    }
    
    updateConflicts();
    return true;
}

bool CHTLNamespaceManager::moveSymbol(const std::string& symbol_name, const std::string& from_namespace, const std::string& to_namespace) {
    std::string from_key = generateNamespaceKey(from_namespace, "");
    std::string to_key = generateNamespaceKey(to_namespace, "");
    
    auto from_it = namespaces.find(from_key);
    auto to_it = namespaces.find(to_key);
    
    if (from_it == namespaces.end() || to_it == namespaces.end()) {
        return false;
    }
    
    auto symbol_it = from_it->second->symbols.find(symbol_name);
    if (symbol_it == from_it->second->symbols.end()) {
        return false;
    }
    
    // Check for conflicts in target namespace
    if (checkSymbolConflict(symbol_name, to_namespace)) {
        return false;
    }
    
    // Move symbol
    to_it->second->symbols[symbol_name] = symbol_it->second;
    from_it->second->symbols.erase(symbol_it);
    
    updateConflicts();
    return true;
}

std::vector<NamespaceConflict> CHTLNamespaceManager::detectConflicts() {
    return conflicts;
}

bool CHTLNamespaceManager::resolveConflict(const NamespaceConflict& conflict, const std::string& resolution) {
    // Simple conflict resolution - just update the resolution
    for (auto& c : conflicts) {
        if (c.symbol_name == conflict.symbol_name && 
            c.namespace1 == conflict.namespace1 && 
            c.namespace2 == conflict.namespace2) {
            c.resolution = resolution;
            return true;
        }
    }
    return false;
}

bool CHTLNamespaceManager::hasConflicts() const {
    return !conflicts.empty();
}

bool CHTLNamespaceManager::namespaceExists(const std::string& name) const {
    std::string key = generateNamespaceKey(name, "");
    return namespaces.find(key) != namespaces.end();
}

std::shared_ptr<NamespaceDefinition> CHTLNamespaceManager::getNamespace(const std::string& name) const {
    std::string key = generateNamespaceKey(name, "");
    auto it = namespaces.find(key);
    return (it != namespaces.end()) ? it->second : nullptr;
}

std::vector<std::string> CHTLNamespaceManager::getNamespaceNames() const {
    std::vector<std::string> names;
    for (const auto& pair : namespaces) {
        names.push_back(pair.second->name);
    }
    return names;
}

std::vector<std::string> CHTLNamespaceManager::getChildNamespaces(const std::string& parent_name) const {
    std::string key = generateNamespaceKey(parent_name, "");
    auto it = namespaces.find(key);
    return (it != namespaces.end()) ? it->second->child_namespaces : std::vector<std::string>();
}

std::vector<std::string> CHTLNamespaceManager::getSymbols(const std::string& namespace_name) const {
    std::string key = generateNamespaceKey(namespace_name, "");
    auto it = namespaces.find(key);
    if (it == namespaces.end()) {
        return {};
    }
    
    std::vector<std::string> symbols;
    for (const auto& pair : it->second->symbols) {
        symbols.push_back(pair.first);
    }
    return symbols;
}

bool CHTLNamespaceManager::importSymbol(const std::string& symbol_name, const std::string& from_namespace, const std::string& to_namespace) {
    std::string from_key = generateNamespaceKey(from_namespace, "");
    std::string to_key = generateNamespaceKey(to_namespace, "");
    
    auto from_it = namespaces.find(from_key);
    auto to_it = namespaces.find(to_key);
    
    if (from_it == namespaces.end() || to_it == namespaces.end()) {
        return false;
    }
    
    auto symbol_it = from_it->second->symbols.find(symbol_name);
    if (symbol_it == from_it->second->symbols.end()) {
        return false;
    }
    
    // Check for conflicts
    if (checkSymbolConflict(symbol_name, to_namespace)) {
        return false;
    }
    
    // Import symbol
    to_it->second->symbols[symbol_name] = symbol_it->second;
    to_it->second->imported_symbols.insert(symbol_name);
    
    updateConflicts();
    return true;
}

bool CHTLNamespaceManager::exportSymbol(const std::string& symbol_name, const std::string& from_namespace, const std::string& to_namespace) {
    return importSymbol(symbol_name, from_namespace, to_namespace);
}

bool CHTLNamespaceManager::importAllSymbols(const std::string& from_namespace, const std::string& to_namespace) {
    std::string from_key = generateNamespaceKey(from_namespace, "");
    std::string to_key = generateNamespaceKey(to_namespace, "");
    
    auto from_it = namespaces.find(from_key);
    auto to_it = namespaces.find(to_key);
    
    if (from_it == namespaces.end() || to_it == namespaces.end()) {
        return false;
    }
    
    bool success = true;
    for (const auto& symbol_pair : from_it->second->symbols) {
        if (!importSymbol(symbol_pair.first, from_namespace, to_namespace)) {
            success = false;
        }
    }
    
    return success;
}

bool CHTLNamespaceManager::validateNamespace(const std::string& name) const {
    return namespaceExists(name);
}

bool CHTLNamespaceManager::validateSymbol(const std::string& symbol_name) const {
    return isValidSymbolName(symbol_name);
}

bool CHTLNamespaceManager::validateNamespaceName(const std::string& name) const {
    return isValidNamespaceName(name);
}

std::string CHTLNamespaceManager::getFullNamespacePath(const std::string& namespace_name) const {
    std::string key = generateNamespaceKey(namespace_name, "");
    auto it = namespaces.find(key);
    if (it == namespaces.end()) {
        return "";
    }
    
    std::string path = namespace_name;
    std::string current_parent = it->second->parent_name;
    
    while (!current_parent.empty()) {
        path = current_parent + "::" + path;
        std::string parent_key = generateNamespaceKey(current_parent, "");
        auto parent_it = namespaces.find(parent_key);
        if (parent_it == namespaces.end()) {
            break;
        }
        current_parent = parent_it->second->parent_name;
    }
    
    return path;
}

std::string CHTLNamespaceManager::getDefaultNamespace() const {
    return default_namespace;
}

void CHTLNamespaceManager::setDefaultNamespace(const std::string& name) {
    if (namespaceExists(name)) {
        default_namespace = name;
    }
}

void CHTLNamespaceManager::clearAllNamespaces() {
    namespaces.clear();
    conflicts.clear();
    
    // Recreate global namespace
    auto global_ns = std::make_shared<NamespaceDefinition>();
    global_ns->name = "global";
    global_ns->parent_name = "";
    global_ns->type = NamespaceType::Global;
    global_ns->is_default = true;
    namespaces["global"] = global_ns;
    default_namespace = "global";
}

std::string CHTLNamespaceManager::processNamespaceDeclaration(const std::string& declaration) {
    // Simple namespace declaration processing
    // This would parse namespace declarations and create namespaces
    return declaration;
}

std::string CHTLNamespaceManager::processSymbolReference(const std::string& reference, const std::string& current_namespace) {
    return resolveSymbol(reference, current_namespace);
}

std::string CHTLNamespaceManager::processNamespaceImport(const std::string& import_statement) {
    // Simple namespace import processing
    // This would parse import statements and import symbols
    return import_statement;
}

std::string CHTLNamespaceManager::generateNamespaceKey(const std::string& name, const std::string& parent) const {
    return parent.empty() ? name : parent + "::" + name;
}

std::string CHTLNamespaceManager::parseNamespacePath(const std::string& path) {
    return path;
}

std::vector<std::string> CHTLNamespaceManager::splitNamespacePath(const std::string& path) {
    std::vector<std::string> parts;
    std::stringstream ss(path);
    std::string part;
    
    while (std::getline(ss, part, ':')) {
        if (!part.empty()) {
            parts.push_back(part);
        }
    }
    
    return parts;
}

bool CHTLNamespaceManager::isNamespacePath(const std::string& name) const {
    return name.find("::") != std::string::npos;
}

std::string CHTLNamespaceManager::resolveNamespacePath(const std::string& path) {
    return path;
}

bool CHTLNamespaceManager::checkSymbolConflict(const std::string& symbol_name, const std::string& namespace_name) {
    std::string key = generateNamespaceKey(namespace_name, "");
    auto it = namespaces.find(key);
    if (it == namespaces.end()) {
        return false;
    }
    
    return it->second->symbols.find(symbol_name) != it->second->symbols.end();
}

void CHTLNamespaceManager::updateConflicts() {
    conflicts.clear();
    
    // Simple conflict detection
    // This would implement more sophisticated conflict detection
}

std::string CHTLNamespaceManager::generateUniqueSymbolName(const std::string& base_name, const std::string& namespace_name) {
    std::string key = generateNamespaceKey(namespace_name, "");
    auto it = namespaces.find(key);
    if (it == namespaces.end()) {
        return base_name;
    }
    
    std::string unique_name = base_name;
    int counter = 1;
    
    while (it->second->symbols.find(unique_name) != it->second->symbols.end()) {
        unique_name = base_name + "_" + std::to_string(counter);
        counter++;
    }
    
    return unique_name;
}

bool CHTLNamespaceManager::isValidNamespaceName(const std::string& name) const {
    if (name.empty()) {
        return false;
    }
    
    // Simple validation - no special characters except ::
    return std::regex_match(name, std::regex("^[a-zA-Z_][a-zA-Z0-9_:]*$"));
}

bool CHTLNamespaceManager::isValidSymbolName(const std::string& name) const {
    if (name.empty()) {
        return false;
    }
    
    // Simple validation - alphanumeric and underscore
    return std::regex_match(name, std::regex("^[a-zA-Z_][a-zA-Z0-9_]*$"));
}

} // namespace CHTL