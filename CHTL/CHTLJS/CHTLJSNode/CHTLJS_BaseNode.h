#ifndef CHTLJS_BASE_NODE_H
#define CHTLJS_BASE_NODE_H

#include <memory>

namespace CHTL {

class CHTLJS_Visitor; // Forward declaration

class CHTLJS_BaseNode {
public:
    virtual ~CHTLJS_BaseNode() = default;
    virtual void accept(CHTLJS_Visitor& visitor) = 0;
};

} // namespace CHTL

#endif // CHTLJS_BASE_NODE_H
