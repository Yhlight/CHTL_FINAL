#pragma once

#include <string>
#include <vector>
#include <memory>
#include <set>
#include <functional>
#include <map>

#include "../CHTLContext/CHTLContext.h"
#include "../CHTLNode/DocumentNode.h"

namespace CHTL {

// The CHTLLoader is the main entry point for the compiler.
// It manages reading files, orchestrating the parser, and handling imports.
class CHTLLoader {
public:
    // Type for a function that can read a file. For tests, this can be a lambda
    // that reads from a map. For production, it reads from disk.
    using FileProvider = std::function<std::string(const std::string&)>;

    // Production constructor
    CHTLLoader();
    // Test constructor
    explicit CHTLLoader(FileProvider provider);

    // Loads and parses an entry-point file and all its imports.
    std::unique_ptr<DocumentNode> loadAndParse(const std::string& entry_point_path, CHTLContext& context);

    // Called by the parser when it encounters an import statement.
    void queueImport(const std::string& current_file_path, const std::string& import_path);

private:
    FileProvider m_file_provider;
    std::vector<std::string> m_import_queue;
    std::set<std::string> m_parsed_files; // To prevent circular imports
};

} // namespace CHTL
