#ifndef CHTL_PARSER_CONTEXT_H
#define CHTL_PARSER_CONTEXT_H

#include "../CHTLNode/StyleTemplateNode.h"
#include "../CHTLNode/ElementTemplateNode.h"
#include "../CHTLNode/VarTemplateNode.h"
#include "../CHTLConfig/Configuration.h"
#include <string>
#include <memory>
#include <unordered_map>
#include <map>

namespace CHTL {

// Holds the shared state between parsers when handling imports.
struct ParserContext {
    std::string namespace_;
    Configuration config_;
    std::unordered_map<std::string, std::shared_ptr<StyleTemplateNode>> style_templates_;
    std::unordered_map<std::string, std::shared_ptr<ElementTemplateNode>> element_templates_;
    std::unordered_map<std::string, std::shared_ptr<VarTemplateNode>> var_templates_;

    // Map of imported namespaces to their contexts.
    std::map<std::string, std::shared_ptr<ParserContext>> namespaces_;
};

} // namespace CHTL

#endif // CHTL_PARSER_CONTEXT_H
