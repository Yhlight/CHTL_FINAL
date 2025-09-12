#ifndef CHTL_CONTEXT_H
#define CHTL_CONTEXT_H

#include <string>
#include <vector>
#include <map>
#include <memory>

// Forward declarations
class StyleTemplateNode;
class ElementTemplateNode;

/**
 * @brief Holds compilation-wide data that needs to be passed through the pipeline.
 *
 * This context object can be used to store global information, such as CSS rules
 * that need to be hoisted, or a table of defined templates.
 */
class CHTLContext {
public:
    // A string to accumulate all CSS rules that need to be placed in a global <style> tag.
    std::string globalCssContent;

    // A map to store parsed style templates by name.
    std::map<std::string, std::shared_ptr<StyleTemplateNode>> styleTemplates;
    // A map to store parsed element templates by name.
    std::map<std::string, std::shared_ptr<ElementTemplateNode>> elementTemplates;
};

#endif // CHTL_CONTEXT_H
