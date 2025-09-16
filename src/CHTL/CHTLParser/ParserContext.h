#ifndef CHTL_PARSER_CONTEXT_H
#define CHTL_PARSER_CONTEXT_H

#include "../CHTLNode/StyleTemplateNode.h"
#include "../CHTLNode/ElementTemplateNode.h"
#include "../CHTLNode/VarTemplateNode.h"
#include "../CHTLConfig/Configuration.h"
#include "../../CJMOD/CJMODManager.h"
#include <string>
#include <memory>
#include <unordered_map>
#include <set>

namespace CHTL {

// Holds the shared state between parsers when handling imports.
struct ParserContext {
    Configuration config_;
    std::unordered_map<std::string, std::shared_ptr<StyleTemplateNode>> style_templates_;
    std::unordered_map<std::string, std::shared_ptr<ElementTemplateNode>> element_templates_;
    std::unordered_map<std::string, std::shared_ptr<VarTemplateNode>> var_templates_;
    std::set<std::string> imported_namespaces_;
    std::shared_ptr<CJMOD::CJMODManager> cjmod_manager = std::make_shared<CJMOD::CJMODManager>();
};

} // namespace CHTL

#endif // CHTL_PARSER_CONTEXT_H
