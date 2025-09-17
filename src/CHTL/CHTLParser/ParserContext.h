#ifndef CHTL_PARSER_CONTEXT_H
#define CHTL_PARSER_CONTEXT_H

#include "../CHTLNode/StyleTemplateNode.h"
#include "../CHTLNode/ElementTemplateNode.h"
#include "../CHTLNode/VarTemplateNode.h"
#include "../CHTLNode/PropertyValue.h" // Now includes UnresolvedProperty and Value definitions
#include "../CHTLConfig/Configuration.h"
#include "../../CJMOD/API/CJMODManager.h"
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <set>

namespace CHTL {

class ElementNode; // Forward declaration is still needed here

enum class CMODExportType {
    Element,
    Style,
    Var
};

struct CMODExport {
    CMODExportType type;
    std::string symbol_name;
    std::string source_file;
};

struct ParserContext {
    Configuration config_;
    std::unordered_map<std::string, std::shared_ptr<StyleTemplateNode>> style_templates_;
    std::unordered_map<std::string, std::shared_ptr<ElementTemplateNode>> element_templates_;
    std::unordered_map<std::string, std::shared_ptr<VarTemplateNode>> var_templates_;
    std::set<std::string> imported_namespaces_;
    std::shared_ptr<CJMOD::CJMODManager> cjmod_manager = std::make_shared<CJMOD::CJMODManager>();
    std::unordered_map<std::string, std::vector<CMODExport>> cmod_exports;

    // A global list of all element nodes created by the parser across all fragments.
    std::vector<ElementNode*> all_elements_;

    // State for the generator, shared across fragments
    std::map<std::string, std::map<std::string, Value>> symbol_table_;
    std::vector<UnresolvedProperty> unresolved_properties_;
    std::set<std::string> responsive_variables_;
};

} // namespace CHTL

#endif // CHTL_PARSER_CONTEXT_H
