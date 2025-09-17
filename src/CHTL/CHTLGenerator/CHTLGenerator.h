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
#include "../CHTLNode/NamespaceNode.h"
#include "../CHTLNode/ImportNode.h"
#include "../CHTLNode/ConfigurationNode.h"
#include <string>
#include <sstream>
#include <memory>
#include <map>
#include <vector>
#include <set>
#include "../CHTLParser/ParserContext.h"

namespace CHTL {

// Forward declaration
class ElementNode;

// Struct to hold info about a property that needs late resolution
struct UnresolvedProperty {
    ElementNode* element; // Pointer to the element node
    std::string property_name;
    std::vector<PropertyValue> value_parts;
};

struct CompilationResult {
    std::string html;
    std::string js;
    std::map<std::string, std::string> placeholder_map;
};

class CHTLGenerator {
public:
    explicit CHTLGenerator(std::shared_ptr<ParserContext> context);
    CompilationResult generate(RootNode& root, bool use_default_struct = false);

private:
    std::shared_ptr<ParserContext> context_;
    // Expression Evaluation
    Value resolvePropertyValue(const std::vector<PropertyValue>& parts);
    Value evaluateExpression(std::vector<PropertyValue>::const_iterator& it, const std::vector<PropertyValue>::const_iterator& end, int min_precedence);

    // Two-pass methods
    void firstPass(Node* node);
    void firstPassVisitElement(ElementNode* node);

    void secondPass();
    std::string generateReactivityScript();

    // Final rendering methods
    void render(const Node* node);
    void renderElement(const ElementNode* node);
    void renderText(const TextNode* node);
    void renderComment(const CommentNode* node);
    void renderOrigin(const OriginNode* node);
    void renderScriptBlock(const ScriptBlockNode* node);
    void renderNamespace(const NamespaceNode* node);

    std::string getElementUniqueId(const ElementNode* node);
    void indent();

    // Data structures for two-pass generation
    std::vector<ElementNode*> all_elements_;
    std::map<std::string, std::map<std::string, Value>> symbol_table_;
    std::vector<UnresolvedProperty> unresolved_properties_;
    std::set<std::string> responsive_variables_;

    std::stringstream output_;
    std::stringstream global_styles_;
    std::stringstream global_scripts_;
    std::map<std::string, std::string> placeholder_map_;
    int indentLevel_ = 0;
};

} // namespace CHTL

#endif // CHTL_GENERATOR_H
