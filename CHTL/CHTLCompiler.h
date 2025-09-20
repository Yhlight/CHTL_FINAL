#ifndef CHTL_COMPILER_H
#define CHTL_COMPILER_H

#include "CHTLLoader/CHTLLoader.h"
#include "CHTLNode/BaseNode.h"
#include "CHTLNode/TemplateDefinitionNode.h"
#include <string>
#include <vector>
#include <memory>
#include <map>
#include <set>

namespace CHTL {

class CHTLCompiler {
public:
    CHTLCompiler();

    // Compiles the CHTL project starting from an entry file.
    // Returns the root of the final, combined AST.
    std::unique_ptr<BaseNode> compile(const std::string& entry_path);

    const std::map<std::string, TemplateDefinitionNode>& getTemplateDefinitions() const { return template_definitions; }

private:
    CHTLLoader loader;
    std::map<std::string, TemplateDefinitionNode> template_definitions;
    std::set<std::string> parsed_files;
};

} // namespace CHTL

#endif // CHTL_COMPILER_H
