#ifndef CHTL_LOADER_H
#define CHTL_LOADER_H

#include <string>
#include <vector>
#include <memory>
#include "Config/Configuration.h"
#include "CHTLNode/BaseNode.h"

namespace CHTL {

class CHTLParser; // Forward declaration

/**
 * @brief Represents the content loaded from a file.
 */
struct LoadedContent {
    enum class Type { AST, RAW_TEXT, EMPTY };
    Type type = Type::EMPTY;
    std::unique_ptr<BaseNode> ast_root;
    std::string raw_text;
};

class Loader {
public:
    Loader(std::shared_ptr<Configuration> config, const std::string& current_file_path);

    LoadedContent load(const std::string& import_path, const std::string& import_type);

    CHTLParser& getMostRecentParser();

private:
    std::shared_ptr<Configuration> m_config;
    std::string m_base_path;
    std::unique_ptr<CHTLParser> m_last_parser;

    std::string resolvePath(const std::string& path, const std::string& extension_hint);
};

} // namespace CHTL

#endif // CHTL_LOADER_H
