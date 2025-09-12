#pragma once
#include "../CHTLNode/Node.h"
#include <string>
#include <sstream>
namespace CHTL {
class Generator {
public:
    std::string generate(RootNode* root);
private:
    void generate_body_content(Node* node, std::stringstream& stream);
    void visit_element(ElementNode* node, std::stringstream& stream);
    void visit_text(TextNode* node, std::stringstream& stream);
    std::string generate_global_styles(RootNode* root);
    void indent(std::stringstream& stream);
    int m_indent_level = 0;
};
}
