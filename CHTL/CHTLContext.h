#ifndef CHTL_CONTEXT_H
#define CHTL_CONTEXT_H

#include "CHTLNode/TemplateNode.h"
#include <string>
#include <unordered_map>
#include <memory>
#include <sstream>

// A context object that holds state for a single compilation.
// This includes all defined templates, customizations, and transient state like global CSS.
class CHTLContext {
public:
    // Definitions
    std::unordered_map<std::string, TemplateNode*> elementTemplates;
    std::unordered_map<std::string, TemplateNode*> styleTemplates;
    std::unordered_map<std::string, TemplateNode*> varTemplates;

    // Generation State
    std::stringstream globalCss;
    std::stringstream globalJs;
    std::unordered_map<std::string, std::string> scriptBlocks;

    // TODO: Add maps for Customizations as well.
};

#endif // CHTL_CONTEXT_H
