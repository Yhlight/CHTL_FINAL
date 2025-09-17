#ifndef CHTL_GENERATOR_H
#define CHTL_GENERATOR_H

#include "../CHTLNode/BaseNode.h"
#include "../CHTLNode/RootNode.h"
#include "../CHTLParser/ParserContext.h"
#include "../CHTLNode/TextNode.h"
#include "../CHTLNode/CommentNode.h"
#include "../CHTLNode/OriginNode.h"
#include "../CHTLNode/ScriptBlockNode.h"
#include <string>
#include <sstream>
#include <memory>
#include <vector>

namespace CHTL {

// The result of a single render pass.
struct RenderResult {
    std::string html;
    std::string css;
    std::string js;
};

class CHTLGenerator {
public:
    explicit CHTLGenerator(std::shared_ptr<ParserContext> context);

    // Pass 1: Analyze an AST and populate the shared context.
    void analyze(RootNode& root);

    // Pass 2: Render the final output from a fully analyzed context.
    RenderResult render(const std::vector<std::unique_ptr<RootNode>>& asts);

private:
    std::shared_ptr<ParserContext> context_;

    // Expression Evaluation
    Value resolvePropertyValue(const std::vector<PropertyValue>& parts);
    Value evaluateExpression(std::vector<PropertyValue>::const_iterator& it, const std::vector<PropertyValue>::const_iterator& end, int min_precedence);

    // Analysis methods (previously firstPass)
    void analyzeNode(Node* node);
    void analyzeElement(ElementNode* node);

    // Rendering methods
    void renderNode(const Node* node);
    void renderElement(const ElementNode* node);
    void renderText(const TextNode* node);
    void renderComment(const CommentNode* node);
    void renderOrigin(const OriginNode* node);
    void renderScriptBlock(const ScriptBlockNode* node);

    std::string getElementUniqueId(const ElementNode* node);
    void indent();

    // Local state for a single render pass
    std::stringstream output_;
    std::stringstream global_styles_;
    std::stringstream global_scripts_;
    int indentLevel_ = 0;
};

} // namespace CHTL

#endif // CHTL_GENERATOR_H
