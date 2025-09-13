#ifndef CHTL_PARSER_CONTEXT_H
#define CHTL_PARSER_CONTEXT_H

#include <string>
#include <unordered_map>
#include <memory>
#include <unordered_set>
#include "../CHTLNode/StyleTemplateNode.h"
#include "../CHTLNode/ElementTemplateNode.h"
#include "../CHTLNode/VarTemplateNode.h"
#include "CHTL/CHTLConfig/Configuration.h"

namespace CHTL {

struct ModuleInfo {
    std::string name;
    std::string version;
    std::string author;
    // ... other fields from CHTL.md
};

struct ExportMap {
    std::unordered_set<std::string> element_templates;
    std::unordered_set<std::string> style_templates;
    std::unordered_set<std::string> var_templates;
    // ... other exportable types
};

class ParserContext {
public:
    Configuration config_;

    // In-memory storage for templates defined in the current compilation.
    // The key is the fully qualified name (e.g., MyModule::MyTemplate)
    std::unordered_map<std::string, std::shared_ptr<StyleTemplateNode>> style_templates_;
    std::unordered_map<std::string, std::shared_ptr<ElementTemplateNode>> element_templates_;
    std::unordered_map<std::string, std::shared_ptr<VarTemplateNode>> var_templates_;

    // Module information for the current file being parsed
    ModuleInfo module_info_;
    ExportMap export_map_;
};

} // namespace CHTL

#endif // CHTL_PARSER_CONTEXT_H
