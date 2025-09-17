#ifndef CHTL_AST_PRINTER_H
#define CHTL_AST_PRINTER_H

#include "../CHTL/Node/BaseNode.h"
#include <string>
#include <memory>

namespace CHTL {

class AstPrinter {
public:
    std::string print(const std::shared_ptr<BaseNode>& node);

private:
    void visit(const std::shared_ptr<BaseNode>& node, int indentLevel);

    std::string m_output;
};

} // namespace CHTL

#endif // CHTL_AST_PRINTER_H
