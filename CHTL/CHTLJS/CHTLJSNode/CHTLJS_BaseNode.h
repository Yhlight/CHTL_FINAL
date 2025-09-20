#ifndef CHTLJS_BASE_NODE_H
#define CHTLJS_BASE_NODE_H

#include "CHTLJS_Visitor.h"
#include <memory>

namespace CHTLJS {

class CHTLJS_BaseNode {
public:
    virtual ~CHTLJS_BaseNode() = default;
    virtual void accept(CHTLJS_Visitor& visitor) = 0;
    virtual std::unique_ptr<CHTLJS_BaseNode> clone() const = 0;
};

} // namespace CHTLJS

#endif // CHTLJS_BASE_NODE_H
