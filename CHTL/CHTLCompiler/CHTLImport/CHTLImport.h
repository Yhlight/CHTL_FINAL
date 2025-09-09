#ifndef CHTL_IMPORT_H
#define CHTL_IMPORT_H

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <fstream>
#include "CHTLNode/CHTLNode.h"

namespace CHTL {

// Import types
enum class ImportType {
    HTML,
    CSS,
    JavaScript,
    CHTL,
    CHTLJS
};

// Import modes
enum class ImportMode {
    Precise,    // Exact import
    Type,       // Type-based import
    Wildcard    // Wildcard import
};

// Import definition
struct ImportDefinition {
    ImportType type;
    ImportMode mode;
    std::string path;
    std::string alias;
    std::string target_name;  // For precise imports
    std::string target_type;  // For type imports
    std::vector<std::string> wildcard_patterns;  // For wildcard imports
    std::map<std::string, std::string> parameters;
};

// Import result
struct ImportResult {
    bool success;
    std::string content;
    std::string error_message;
    ImportType type;
    std::string source_path;
};

// Import node for AST (using existing ImportNode from CHTLNode.h)
// The ImportNode class is already defined in CHTLNode.h

// Import manager for handling imports
class CHTLImportManager {
public:
    CHTLImportManager();
    ~CHTLImportManager();
    
    // Import processing
    ImportResult processImport(const ImportDefinition& definition);
    ImportResult processHTMLImport(const std::string& path, const std::string& alias = "");
    ImportResult processCSSImport(const std::string& path, const std::string& alias = "");
    ImportResult processJavaScriptImport(const std::string& path, const std::string& alias = "");
    ImportResult processCHTLImport(const std::string& path, const std::string& alias = "", ImportMode mode = ImportMode::Precise);
    ImportResult processCHTLJSImport(const std::string& path, const std::string& alias = "", ImportMode mode = ImportMode::Precise);
    
    // File processing
    std::string readFile(const std::string& path);
    bool fileExists(const std::string& path);
    std::string resolvePath(const std::string& path);
    
    // Import resolution
    std::string resolveImportPath(const std::string& path, ImportType type);
    std::vector<std::string> findMatchingFiles(const std::string& pattern, ImportType type);
    
    // Validation
    bool validateImport(const ImportDefinition& definition);
    bool validatePath(const std::string& path);
    
    // Utility methods
    void addSearchPath(const std::string& path);
    void removeSearchPath(const std::string& path);
    std::vector<std::string> getSearchPaths() const;
    void clearSearchPaths();
    
    // Content processing
    std::string processHTMLContent(const std::string& content, const std::string& alias = "");
    std::string processCSSContent(const std::string& content, const std::string& alias = "");
    std::string processJavaScriptContent(const std::string& content, const std::string& alias = "");
    std::string processCHTLContent(const std::string& content, const std::string& alias = "", ImportMode mode = ImportMode::Precise);
    std::string processCHTLJSContent(const std::string& content, const std::string& alias = "", ImportMode mode = ImportMode::Precise);
    
private:
    std::vector<std::string> search_paths;
    std::map<std::string, std::string> import_cache;
    
    // Helper methods
    std::string getFileExtension(const std::string& path);
    ImportType getImportTypeFromExtension(const std::string& extension);
    std::string generateImportKey(const ImportDefinition& definition);
    bool isWildcardPattern(const std::string& pattern);
    std::vector<std::string> expandWildcardPattern(const std::string& pattern, ImportType type);
    std::string sanitizeContent(const std::string& content, ImportType type);
    std::string applyAlias(const std::string& content, const std::string& alias, ImportType type);
};

} // namespace CHTL

#endif // CHTL_IMPORT_H