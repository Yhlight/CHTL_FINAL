#ifndef CHTLMODULESYSTEM_H
#define CHTLMODULESYSTEM_H

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <set>

namespace CHTL {

// Module types
enum class ModuleType {
    CMOD,      // CHTL Module
    CJMOD,     // CHTL JS Module
    Mixed      // Mixed CMOD + CJMOD
};

// Module info structure
struct ModuleInfo {
    std::string name;
    std::string version;
    std::string description;
    std::string author;
    std::string license;
    std::vector<std::string> dependencies;
    std::vector<std::string> exports;
    std::map<std::string, std::string> metadata;
    
    ModuleInfo() = default;
    ModuleInfo(const std::string& n, const std::string& v = "1.0.0")
        : name(n), version(v) {}
};

// CMOD (CHTL Module) structure
struct CMODModule {
    ModuleInfo info;
    std::string content;
    std::vector<std::string> submodules;
    std::map<std::string, std::string> exports;
    std::map<std::string, std::string> imports;
    std::string path;
    
    CMODModule() = default;
    CMODModule(const std::string& n, const std::string& c = "")
        : content(c) {
        info.name = n;
    }
};

// CJMOD (CHTL JS Module) structure
struct CJMODModule {
    ModuleInfo info;
    std::string content;
    std::map<std::string, std::string> exports;
    std::map<std::string, std::string> imports;
    std::string path;
    
    CJMODModule() = default;
    CJMODModule(const std::string& n, const std::string& c = "")
        : content(c) {
        info.name = n;
    }
};

// Mixed module structure
struct MixedModule {
    ModuleInfo info;
    CMODModule cmod_part;
    CJMODModule cjmod_part;
    std::string path;
    
    MixedModule() = default;
    MixedModule(const std::string& n)
        : cmod_part(n), cjmod_part(n) {
        info.name = n;
    }
};

// Module search path
struct ModuleSearchPath {
    std::string path;
    bool recursive;
    int priority;
    
    ModuleSearchPath() : recursive(false), priority(0) {}
    ModuleSearchPath(const std::string& p, bool r = false, int pr = 0)
        : path(p), recursive(r), priority(pr) {}
};

// Module dependency
struct ModuleDependency {
    std::string name;
    std::string version;
    std::string type;  // "exact", "range", "wildcard"
    bool optional;
    
    ModuleDependency() : optional(false) {}
    ModuleDependency(const std::string& n, const std::string& v = "", const std::string& t = "exact")
        : name(n), version(v), type(t), optional(false) {}
};

// CMOD Manager
class CHTLCMODManager {
public:
    CHTLCMODManager();
    ~CHTLCMODManager();
    
    // Module management
    bool loadModule(const std::string& name, const std::string& path);
    bool unloadModule(const std::string& name);
    bool reloadModule(const std::string& name);
    
    // Module queries
    bool hasModule(const std::string& name) const;
    CMODModule getModule(const std::string& name) const;
    std::vector<std::string> getModuleNames() const;
    std::vector<std::string> getModulePaths() const;
    
    // Module operations
    std::string getModuleContent(const std::string& name) const;
    std::string getModuleExport(const std::string& name, const std::string& export_name) const;
    std::vector<std::string> getModuleExports(const std::string& name) const;
    std::vector<std::string> getModuleImports(const std::string& name) const;
    
    // Search path management
    void addSearchPath(const std::string& path, bool recursive = false, int priority = 0);
    void removeSearchPath(const std::string& path);
    std::vector<ModuleSearchPath> getSearchPaths() const;
    
    // Dependency management
    std::vector<ModuleDependency> getModuleDependencies(const std::string& name) const;
    bool resolveDependencies(const std::string& name);
    std::vector<std::string> getDependencyChain(const std::string& name) const;
    
    // Module validation
    bool validateModule(const std::string& name) const;
    std::vector<std::string> getModuleErrors(const std::string& name) const;
    bool validateAllModules() const;
    
    // Module utilities
    std::string findModulePath(const std::string& name) const;
    std::string generateModuleId(const std::string& name) const;
    void clearCache();
    
private:
    std::map<std::string, CMODModule> modules;
    std::vector<ModuleSearchPath> search_paths;
    
    // Helper methods
    bool parseModuleInfo(const std::string& content, ModuleInfo& info) const;
    bool parseModuleExports(const std::string& content, std::map<std::string, std::string>& exports) const;
    bool parseModuleImports(const std::string& content, std::map<std::string, std::string>& imports) const;
    std::string readModuleFile(const std::string& path) const;
    bool isModuleFile(const std::string& path) const;
    std::vector<std::string> findModuleFiles(const std::string& directory) const;
    bool hasCircularDependency(const std::string& name, const std::set<std::string>& visited) const;
};

// CJMOD Manager
class CHTLCJMODManager {
public:
    CHTLCJMODManager();
    ~CHTLCJMODManager();
    
    // Module management
    bool loadModule(const std::string& name, const std::string& path);
    bool unloadModule(const std::string& name);
    bool reloadModule(const std::string& name);
    
    // Module queries
    bool hasModule(const std::string& name) const;
    CJMODModule getModule(const std::string& name) const;
    std::vector<std::string> getModuleNames() const;
    std::vector<std::string> getModulePaths() const;
    
    // Module operations
    std::string getModuleContent(const std::string& name) const;
    std::string getModuleExport(const std::string& name, const std::string& export_name) const;
    std::vector<std::string> getModuleExports(const std::string& name) const;
    std::vector<std::string> getModuleImports(const std::string& name) const;
    
    // Search path management
    void addSearchPath(const std::string& path, bool recursive = false, int priority = 0);
    void removeSearchPath(const std::string& path);
    std::vector<ModuleSearchPath> getSearchPaths() const;
    
    // Dependency management
    std::vector<ModuleDependency> getModuleDependencies(const std::string& name) const;
    bool resolveDependencies(const std::string& name);
    std::vector<std::string> getDependencyChain(const std::string& name) const;
    
    // Module validation
    bool validateModule(const std::string& name) const;
    std::vector<std::string> getModuleErrors(const std::string& name) const;
    bool validateAllModules() const;
    
    // Module utilities
    std::string findModulePath(const std::string& name) const;
    std::string generateModuleId(const std::string& name) const;
    void clearCache();
    
private:
    std::map<std::string, CJMODModule> modules;
    std::vector<ModuleSearchPath> search_paths;
    
    // Helper methods
    bool parseModuleInfo(const std::string& content, ModuleInfo& info) const;
    bool parseModuleExports(const std::string& content, std::map<std::string, std::string>& exports) const;
    bool parseModuleImports(const std::string& content, std::map<std::string, std::string>& imports) const;
    std::string readModuleFile(const std::string& path) const;
    bool isModuleFile(const std::string& path) const;
    std::vector<std::string> findModuleFiles(const std::string& directory) const;
    bool hasCircularDependency(const std::string& name, const std::set<std::string>& visited) const;
};

// Mixed Module Manager
class CHTLMixedModuleManager {
public:
    CHTLMixedModuleManager();
    ~CHTLMixedModuleManager();
    
    // Module management
    bool loadModule(const std::string& name, const std::string& path);
    bool unloadModule(const std::string& name);
    bool reloadModule(const std::string& name);
    
    // Module queries
    bool hasModule(const std::string& name) const;
    MixedModule getModule(const std::string& name) const;
    std::vector<std::string> getModuleNames() const;
    std::vector<std::string> getModulePaths() const;
    
    // Module operations
    std::string getModuleContent(const std::string& name) const;
    std::string getCMODContent(const std::string& name) const;
    std::string getCJMODContent(const std::string& name) const;
    std::string getModuleExport(const std::string& name, const std::string& export_name) const;
    std::vector<std::string> getModuleExports(const std::string& name) const;
    std::vector<std::string> getModuleImports(const std::string& name) const;
    
    // Search path management
    void addSearchPath(const std::string& path, bool recursive = false, int priority = 0);
    void removeSearchPath(const std::string& path);
    std::vector<ModuleSearchPath> getSearchPaths() const;
    
    // Dependency management
    std::vector<ModuleDependency> getModuleDependencies(const std::string& name) const;
    bool resolveDependencies(const std::string& name);
    std::vector<std::string> getDependencyChain(const std::string& name) const;
    
    // Module validation
    bool validateModule(const std::string& name) const;
    std::vector<std::string> getModuleErrors(const std::string& name) const;
    bool validateAllModules() const;
    
    // Module utilities
    std::string findModulePath(const std::string& name) const;
    std::string generateModuleId(const std::string& name) const;
    void clearCache();
    
private:
    std::map<std::string, MixedModule> modules;
    std::vector<ModuleSearchPath> search_paths;
    
    // Helper methods
    bool parseModuleInfo(const std::string& content, ModuleInfo& info) const;
    bool parseModuleExports(const std::string& content, std::map<std::string, std::string>& exports) const;
    bool parseModuleImports(const std::string& content, std::map<std::string, std::string>& imports) const;
    std::string readModuleFile(const std::string& path) const;
    bool isModuleFile(const std::string& path) const;
    std::vector<std::string> findModuleFiles(const std::string& directory) const;
    bool hasCircularDependency(const std::string& name, const std::set<std::string>& visited) const;
    bool splitMixedModule(const std::string& content, std::string& cmod_part, std::string& cjmod_part) const;
};

// Unified Module Manager
class CHTLModuleManager {
public:
    CHTLModuleManager();
    ~CHTLModuleManager();
    
    // Module management
    bool loadModule(const std::string& name, const std::string& path, ModuleType type = ModuleType::CMOD);
    bool unloadModule(const std::string& name);
    bool reloadModule(const std::string& name);
    
    // Module queries
    bool hasModule(const std::string& name) const;
    ModuleType getModuleType(const std::string& name) const;
    std::vector<std::string> getModuleNames() const;
    std::vector<std::string> getModuleNames(ModuleType type) const;
    
    // Module operations
    std::string getModuleContent(const std::string& name) const;
    std::string getModuleExport(const std::string& name, const std::string& export_name) const;
    std::vector<std::string> getModuleExports(const std::string& name) const;
    std::vector<std::string> getModuleImports(const std::string& name) const;
    
    // Search path management
    void addSearchPath(const std::string& path, bool recursive = false, int priority = 0);
    void removeSearchPath(const std::string& path);
    std::vector<ModuleSearchPath> getSearchPaths() const;
    
    // Dependency management
    std::vector<ModuleDependency> getModuleDependencies(const std::string& name) const;
    bool resolveDependencies(const std::string& name);
    std::vector<std::string> getDependencyChain(const std::string& name) const;
    
    // Module validation
    bool validateModule(const std::string& name) const;
    std::vector<std::string> getModuleErrors(const std::string& name) const;
    bool validateAllModules() const;
    
    // Module utilities
    std::string findModulePath(const std::string& name) const;
    std::string generateModuleId(const std::string& name) const;
    void clearCache();
    
    // Manager access
    CHTLCMODManager& getCMODManager();
    CHTLCJMODManager& getCJMODManager();
    CHTLMixedModuleManager& getMixedModuleManager();
    
private:
    CHTLCMODManager cmod_manager;
    CHTLCJMODManager cjmod_manager;
    CHTLMixedModuleManager mixed_manager;
    
    // Helper methods
    ModuleType detectModuleType(const std::string& content) const;
    std::string getModuleManagerName(const std::string& name) const;
};

} // namespace CHTL

#endif // CHTLMODULESYSTEM_H