#ifndef CHTLCONTEXT_H
#define CHTLCONTEXT_H

#include <string>
#include <vector>
#include <map>
#include <memory>

namespace CHTL {

// Forward declarations
class CHTLNode;
class TemplateInfo;
class CustomInfo;
class ImportInfo;

// CHTL configuration
struct CHTLConfig {
    int index_initial_count = 0;
    bool debug_mode = false;
    bool disable_style_auto_add_class = false;
    bool disable_style_auto_add_id = false;
    bool disable_default_namespace = false;
    bool disable_custom_origin_type = false;
    bool disable_script_auto_add_class = true;
    bool disable_script_auto_add_id = true;
};

// Template types
enum class TemplateType {
    Style,
    Element,
    Var
};

// Custom types
enum class CustomType {
    Style,
    Element,
    Var
};

// Import types
enum class ImportType {
    Html,
    Style,
    JavaScript,
    Chtl,
    CJmod
};

// Template information
class TemplateInfo {
public:
    std::string name;
    TemplateType template_type;
    std::string content;
    std::string namespace_name;
    
    TemplateInfo(const std::string& n, TemplateType t, const std::string& c, const std::string& ns = "")
        : name(n), template_type(t), content(c), namespace_name(ns) {}
};

// Custom information
class CustomInfo {
public:
    std::string name;
    CustomType custom_type;
    std::string content;
    std::string namespace_name;
    
    CustomInfo(const std::string& n, CustomType t, const std::string& c, const std::string& ns = "")
        : name(n), custom_type(t), content(c), namespace_name(ns) {}
};

// Import information
class ImportInfo {
public:
    std::string file_path;
    ImportType import_type;
    std::vector<std::string> imported_items;
    
    ImportInfo(const std::string& path, ImportType type, const std::vector<std::string>& items)
        : file_path(path), import_type(type), imported_items(items) {}
};

// CHTL Compiler Context
class CHTLContext {
private:
    CHTLConfig config;
    std::vector<std::string> namespace_stack;
    std::map<std::string, std::shared_ptr<TemplateInfo>> template_registry;
    std::map<std::string, std::shared_ptr<CustomInfo>> custom_registry;
    std::map<std::string, std::shared_ptr<ImportInfo>> import_cache;
    std::string current_file;

public:
    CHTLContext();
    CHTLContext(const CHTLConfig& cfg);
    
    // Configuration
    void setConfig(const CHTLConfig& cfg);
    const CHTLConfig& getConfig() const;
    
    // File management
    void setCurrentFile(const std::string& file_path);
    const std::string& getCurrentFile() const;
    
    // Namespace management
    void pushNamespace(const std::string& namespace_name);
    void popNamespace();
    const std::string& getCurrentNamespace() const;
    bool hasCurrentNamespace() const;
    
    // Template management
    void registerTemplate(std::shared_ptr<TemplateInfo> info);
    std::shared_ptr<TemplateInfo> getTemplate(const std::string& name) const;
    
    // Custom management
    void registerCustom(std::shared_ptr<CustomInfo> info);
    std::shared_ptr<CustomInfo> getCustom(const std::string& name) const;
    
    // Import management
    void cacheImport(const std::string& path, std::shared_ptr<ImportInfo> info);
    std::shared_ptr<ImportInfo> getCachedImport(const std::string& path) const;
    
    // Utility methods
    std::string getFullName(const std::string& name) const;
    void clear();
};

} // namespace CHTL

#endif // CHTLCONTEXT_H