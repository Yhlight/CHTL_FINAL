#pragma once

#include "../CHTLJSNode/Node.h"
#include <string>
#include <vector>

namespace CHTL_JS {

class CHTLJSGenerator {
public:
    CHTLJSGenerator(NodeList nodes);

    std::string Generate();

private:
    NodeList m_nodes;
};

} // namespace CHTL_JS
