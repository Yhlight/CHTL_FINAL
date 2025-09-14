#ifndef CHTL_GENERATOR_H
#define CHTL_GENERATOR_H

#include "../CHTLNode/BaseNode.h"
#include "../CHTLNode/RootNode.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/TextNode.h"
#include "../CHTLNode/CommentNode.h"
#include "../CHTLNode/OriginNode.h"
#include "../CHTLNode/PropertyReferenceNode.h"
#include "../CHTLNode/PropertyValue.h"
#include "../../CHTLJS/CHTLJSParser/CHTLJSContext.h"
#include <string>
#include <sstream>
#include <memory>
#include <map>
#include <vector>
#include <set>

namespace CHTL {

// Forward declarations
class ElementNode;
class RawScriptNode;
class RawStyleNode;
namespace CHTLJS {
    class CHTLJSParser;
    class CHTLJSGenerator;
}

// Struct to hold info about a property that needs late resolution
struct UnresolvedProperty {
    ElementNode* element; // Pointer to the element node
    std::string property_name;
    std::vector<PropertyValue> value_parts;
};

struct CompilationResult {
    std::string html;
    std::string js;
};

class CHTLGenerator {
public:
    CHTLGenerator();
    ~CHTLGenerator();
    CompilationResult generate(RootNode& root);

private:
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
    void renderRawStyle(const RawStyleNode* node);
    void renderRawScript(const RawScriptNode* node);

    std::string getElementUniqueId(const ElementNode* node);
    void indent();

    // CHTL JS processing pipeline
    std::shared_ptr<CHTLJS::CHTLJSContext> chtljs_context_;
    std::unique_ptr<CHTLJS::CHTLJSParser> chtljs_parser_;
    std::unique_ptr<CHTLJS::CHTLJSGenerator> chtljs_generator_;

    // Data structures for two-pass generation
    std::vector<ElementNode*> all_elements_;
    std::map<std::string, std::map<std::string, Value>> symbol_table_;
    std::vector<UnresolvedProperty> unresolved_properties_;
    std::set<std::string> responsive_variables_;

    std::stringstream output_;
    std::stringstream global_styles_;
    std::stringstream global_scripts_;
    int indentLevel_ = 0;
};

} // namespace CHTL

#endif // CHTL_GENERATOR_H
