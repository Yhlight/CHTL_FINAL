#ifndef CHTL_NAMESPACE_H
#define CHTL_NAMESPACE_H

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <set>
#include "CHTLNode/CHTLNode.h"

namespace CHTL {

// Namespace types
enum class NamespaceType {
    Global,
    Local,
    Nested
};

// Namespace definition
struct NamespaceDefinition {
    std::string name;
    std::string parent_name;
    NamespaceType type;
    std::map<std::string, std::string> symbols;
    std::vector<std::string> child_namespaces;
    std::set<std::string> imported_symbols;
    bool is_default;
};

// Namespace conflict
struct NamespaceConflict {
    std::string symbol_name;
    std::string namespace1;
    std::string namespace2;
    std::string conflict_type;
    std::string resolution;
};

// Namespace manager for handling namespaces
class CHTLNamespaceManager {
public:
    CHTLNamespaceManager();
    ~CHTLNamespaceManager();
    
    // Namespace management
    bool createNamespace(const std::string& name, const std::string& parent = "", bool is_default = false);
    bool deleteNamespace(const std::string& name);
    bool renameNamespace(const std::string& old_name, const std::string& new_name);
    
    // Symbol management
    bool addSymbol(const std::string& namespace_name, const std::string& symbol_name, const std::string& symbol_value);
    bool removeSymbol(const std::string& namespace_name, const std::string& symbol_name);
    bool updateSymbol(const std::string& namespace_name, const std::string& symbol_name, const std::string& symbol_value);
    
    // Symbol resolution
    std::string resolveSymbol(const std::string& symbol_name, const std::string& current_namespace = "");
    std::string resolveSymbolWithNamespace(const std::string& full_symbol_name);
    std::vector<std::string> findSymbols(const std::string& pattern, const std::string& namespace_name = "");
    
    // Namespace operations
    bool mergeNamespaces(const std::string& source_namespace, const std::string& target_namespace);
    bool splitNamespace(const std::string& namespace_name, const std::vector<std::string>& new_namespaces);
    bool moveSymbol(const std::string& symbol_name, const std::string& from_namespace, const std::string& to_namespace);
    
    // Conflict detection and resolution
    std::vector<NamespaceConflict> detectConflicts();
    bool resolveConflict(const NamespaceConflict& conflict, const std::string& resolution);
    bool hasConflicts() const;
    
    // Namespace queries
    bool namespaceExists(const std::string& name) const;
    std::shared_ptr<NamespaceDefinition> getNamespace(const std::string& name) const;
    std::vector<std::string> getNamespaceNames() const;
    std::vector<std::string> getChildNamespaces(const std::string& parent_name) const;
    std::vector<std::string> getSymbols(const std::string& namespace_name) const;
    
    // Import/Export
    bool importSymbol(const std::string& symbol_name, const std::string& from_namespace, const std::string& to_namespace);
    bool exportSymbol(const std::string& symbol_name, const std::string& from_namespace, const std::string& to_namespace);
    bool importAllSymbols(const std::string& from_namespace, const std::string& to_namespace);
    
    // Validation
    bool validateNamespace(const std::string& name) const;
    bool validateSymbol(const std::string& symbol_name) const;
    bool validateNamespaceName(const std::string& name) const;
    
    // Utility methods
    std::string getFullNamespacePath(const std::string& namespace_name) const;
    std::string getDefaultNamespace() const;
    void setDefaultNamespace(const std::string& name);
    void clearAllNamespaces();
    
    // Namespace processing
    std::string processNamespaceDeclaration(const std::string& declaration);
    std::string processSymbolReference(const std::string& reference, const std::string& current_namespace = "");
    std::string processNamespaceImport(const std::string& import_statement);
    
private:
    std::map<std::string, std::shared_ptr<NamespaceDefinition>> namespaces;
    std::string default_namespace;
    std::vector<NamespaceConflict> conflicts;
    
    // Helper methods
    std::string generateNamespaceKey(const std::string& name, const std::string& parent = "") const;
    std::string parseNamespacePath(const std::string& path);
    std::vector<std::string> splitNamespacePath(const std::string& path);
    bool isNamespacePath(const std::string& name) const;
    std::string resolveNamespacePath(const std::string& path);
    bool checkSymbolConflict(const std::string& symbol_name, const std::string& namespace_name);
    void updateConflicts();
    std::string generateUniqueSymbolName(const std::string& base_name, const std::string& namespace_name);
    bool isValidNamespaceName(const std::string& name) const;
    bool isValidSymbolName(const std::string& name) const;
};

} // namespace CHTL

#endif // CHTL_NAMESPACE_H