#ifndef CHTL_GENERATOR_H
#define CHTL_GENERATOR_H

#include "../Node/BaseNode.h"
#include "../Node/RootNode.h"
#include "../Node/ElementNode.h"
#include "../Node/AttributeNode.h"
#include "../Node/TextNode.h"
#include "../Node/StyleNode.h"
#include <string>
#include <memory>
#include <sstream>

namespace CHTL {

class Generator {
public:
    explicit Generator(std::shared_ptr<BaseNode> root);
    void generate();
    std::string getHtml() const;
    std::string getCss() const; // For later, when we have global styles

private:
    void visit(const std::shared_ptr<BaseNode>& node);
    void visitRootNode(const std::shared_ptr<RootNode>& node);
    void visitElementNode(const std::shared_ptr<ElementNode>& node);
    void visitTextNode(const std::shared_ptr<TextNode>& node);
    void visitStyleNode(const std::shared_ptr<StyleNode>& node);


    std::shared_ptr<BaseNode> m_root;
    std::stringstream m_html_stream;
    std::stringstream m_css_stream;
    int m_indent_level = 0;
};

} // namespace CHTL

#endif // CHTL_GENERATOR_H
