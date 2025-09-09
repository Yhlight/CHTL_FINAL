#pragma once

#include "../CHTL/CHTLNode/BaseNode.h"
#include "../CHTL/CHTLNode/ModuleNode.h"
#include "../CHTL/CHTLContext/Context.h"
#include <string>
#include <unordered_set>

namespace CHTL {

class Loader {
public:
    Loader();

    void loadImports(Program* program, Context& context);

private:
    void processImport(ImportNode* importNode, Context& context);

    std::unordered_set<std::string> m_loaded_files;
};

} // namespace CHTL
