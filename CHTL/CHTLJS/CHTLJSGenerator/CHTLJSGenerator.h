#ifndef CHTLJS_GENERATOR_H
#define CHTLJS_GENERATOR_H

#include "../CHTLJSNode/CHTLJS_BaseNode.h"
#include "../CHTLJSNode/Visitor.h"
#include <string>

namespace CHTL_JS {

class CHTLJSGenerator : public Visitor {
public:
    CHTLJSGenerator();
    std::string generate(CHTLJS_BaseNode* root);

    // visit methods will be implemented here

private:
    std::string output;
};

} // namespace CHTL_JS

#endif // CHTLJS_GENERATOR_H
