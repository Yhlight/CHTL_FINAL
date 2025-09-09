#pragma once

#include "../CHTLNode/BaseNode.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/StyleNode.h"
#include "../CHTLNode/TemplateNode.h"
#include "../CHTLNode/RawBlockNode.h"
#include "../CHTLContext/Context.h"
#include <string>
#include <sstream>

namespace CHTL {

struct GenerationResult {
    std::string html;
    std::string global_css;
    std::string global_js;
};

class Generator {
public:
    Generator(Context& context);
    GenerationResult generate(Program* program);

private:
    void visit(Node* node, std::stringstream& out);
    void visit(Program* node, std::stringstream& out);
    void visit(ElementNode* node, std::stringstream& out);
    void visit(TextNode* node, std::stringstream& out);
    void visit(CommentNode* node, std::stringstream& out);
    void visit(AttributeNode* node, std::stringstream& out);
    void visit(Identifier* node, std::stringstream& out);
    void visit(StringLiteral* node, std::stringstream& out);
    void visit(StyleNode* node, std::stringstream& inline_out);
    void visit(StyleRuleNode* node); // Writes to global css
    void visit(StylePropertyNode* node, std::stringstream& out);
    void visit(TemplateInstantiationNode* node, std::stringstream& out);
    void visit(RawBlockNode* node, std::stringstream& out);


    Context& m_context;
    std::stringstream m_main_out;
    std::stringstream m_global_css;
};

} // namespace CHTL
