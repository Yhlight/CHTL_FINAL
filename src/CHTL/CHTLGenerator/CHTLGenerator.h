#ifndef CHTL_GENERATOR_H
#define CHTL_GENERATOR_H

#include "../CHTLNode/BaseNode.h"
#include "../CHTLNode/RootNode.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/TextNode.h"
#include "../CHTLNode/CommentNode.h"
#include "../CHTLNode/StyleBlockNode.h"
#include "../CHTLNode/ScriptBlockNode.h"
#include "../CHTLNode/OriginNode.h"
#include "../CHTLNode/PropertyReferenceNode.h"
#include "../CHTLNode/PropertyValue.h"
#include <string>
#include <sstream>
#include <memory>
#include <map>
#include <vector>
#include "../../CompilerDispatcher/CompilerDispatcher.h"

namespace CHTL {

// Forward declaration
class ElementNode;

// Struct to hold info about a property that needs late resolution
struct UnresolvedProperty {
    ElementNode* element; // Pointer to the element node
    std::string property_name;
    std::vector<PropertyValue> value_parts;
};

class CHTLGenerator {
public:
    explicit CHTLGenerator(std::shared_ptr<CompilerDispatcher> dispatcher);
    std::string generate(RootNode& root);

private:
    std::shared_ptr<CompilerDispatcher> dispatcher_;

    std::string resolvePlaceholders(std::string content, bool is_script);

    // Two-pass methods
    void firstPass(Node* node);
    void firstPassVisitElement(ElementNode* node);

    void secondPass();

    // Final rendering methods
    void render(const Node* node);
    void renderElement(const ElementNode* node);
    void renderText(const TextNode* node);
    void renderComment(const CommentNode* node);
    void renderOrigin(const OriginNode* node);
    void renderScriptBlock(const ScriptBlockNode* node);

    std::string getElementUniqueId(const ElementNode* node);
    void indent();

    // Data structures for two-pass generation
    std::vector<ElementNode*> all_elements_;
    std::map<std::string, std::map<std::string, Value>> symbol_table_;
    std::vector<UnresolvedProperty> unresolved_properties_;

    std::stringstream output_;
    std::stringstream global_styles_;
    int indentLevel_ = 0;
};

} // namespace CHTL

#endif // CHTL_GENERATOR_H
