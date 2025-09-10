#ifndef CHTL_GENERATOR_H
#define CHTL_GENERATOR_H

#include "CHTLNode/BaseNode.h"
#include "CHTLNode/ElementNode.h"
#include "CHTLNode/TextNode.h"
#include <string>
#include <sstream>

namespace CHTL {

class Generator {
public:
    Generator() = default;

    std::string generate(const NodeList& ast);

private:
    void generateNode(const NodePtr& node);
    void generateElement(const ElementNode& node);
    void generateText(const TextNode& node);

    std::string getIndent();

    std::stringstream output_stream_;
    int indent_level_ = 0;
};

} // namespace CHTL

#endif // CHTL_GENERATOR_H
