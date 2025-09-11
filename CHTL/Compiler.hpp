#pragma once

#include "CHTLContext.hpp"
#include "CHTLGenerator/HtmlGenerator.hpp"
#include "CHTLNode/BaseNode.hpp"
#include <string>
#include <vector>
#include <memory>

namespace CHTL {

class Compiler {
public:
    Compiler(const std::string& base_dir);
    std::string compile(const std::string& entry_path);

private:
    std::vector<NodePtr> processFile(const std::string& path);
    void resolveImports(const std::vector<NodePtr>& ast, const std::string& current_dir);

    CHTLContext m_context;
    std::vector<std::string> m_processed_files; // To prevent circular imports
    std::string m_base_dir;
};

} // namespace CHTL
