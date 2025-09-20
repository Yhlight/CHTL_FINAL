#ifndef CHTLJS_BASE_NODE_H
#define CHTLJS_BASE_NODE_H

#include <memory>

namespace CHTL_JS {

class Visitor; // Forward declaration

class CHTLJS_BaseNode {
public:
    virtual ~CHTLJS_BaseNode() = default;
    virtual void accept(Visitor& visitor) = 0;
};

} // namespace CHTL_JS

#endif // CHTLJS_BASE_NODE_H
