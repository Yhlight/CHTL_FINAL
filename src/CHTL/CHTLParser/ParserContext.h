#ifndef CHTL_PARSER_CONTEXT_H
#define CHTL_PARSER_CONTEXT_H

#include "../CHTLNode/StyleTemplateNode.h"
#include "../CHTLNode/ElementTemplateNode.h"
#include "../CHTLNode/VarTemplateNode.h"
#include "../CHTLConfig/Configuration.h"
#include "../../CJMOD/API/CJMODManager.h"
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <set>

namespace CHTL {

enum class CMODExportType {
    Element,
    Style,
    Var
};

struct CMODExport {
    CMODExportType type;
    std::string symbol_name;
    std::string source_file; // Path relative to the CMOD root
};

// Holds the shared state between parsers when handling imports.
struct ParserContext {
    Configuration config_;
    std::unordered_map<std::string, std::shared_ptr<StyleTemplateNode>> style_templates_;
    std::unordered_map<std::string, std::shared_ptr<ElementTemplateNode>> element_templates_;
    std::unordered_map<std::string, std::shared_ptr<VarTemplateNode>> var_templates_;
    std::set<std::string> imported_namespaces_;
    std::shared_ptr<CJMOD::CJMODManager> cjmod_manager = std::make_shared<CJMOD::CJMODManager>();

    // Key is the module name (e.g., "MyModule")
    // Value is a list of all things that module exports.
    std::unordered_map<std::string, std::vector<CMODExport>> cmod_exports;
};

} // namespace CHTL

#endif // CHTL_PARSER_CONTEXT_H
