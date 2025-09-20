#ifndef CHTLJS_GENERATOR_H
#define CHTLJS_GENERATOR_H

#include "../CHTLJSNode/CHTLJS_BaseNode.h"
#include <string>

namespace CHTL {

// The CHTLJS_Visitor interface will be implemented by the generator.
class CHTLJS_Visitor {
public:
    virtual ~CHTLJS_Visitor() = default;
    // visit methods for each CHTLJS node type will be added here
};


class CHTLJSGenerator : public CHTLJS_Visitor {
public:
    std::string generate(CHTLJS_BaseNode* root);
private:
    std::string output;
};

} // namespace CHTL

#endif // CHTLJS_GENERATOR_H
