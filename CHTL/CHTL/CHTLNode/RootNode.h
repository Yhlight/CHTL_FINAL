#pragma once

#include "BaseNode.h"

namespace CHTL {

class RootNode : public BaseNode {
public:
    RootNode();
    
    std::string toHTML() const override;
    std::string toCSS() const override;
    std::string toJS() const override;
    
    std::shared_ptr<BaseNode> clone() const override;
};

} // namespace CHTL