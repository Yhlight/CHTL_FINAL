#ifndef CHTL_CONTEXT_H
#define CHTL_CONTEXT_H

#include <string>
#include <sstream>
#include <unordered_map>
#include <memory>
#include "CHTLNode/OriginNode.h"

// A context object that holds state for a single compilation.
class CHTLContext {
public:
    // Definitions
    std::unordered_map<std::string, std::unique_ptr<OriginNode>> namedOriginBlocks;

    // Generation State
    std::stringstream globalCss;
    std::stringstream globalJs;
};

#endif // CHTL_CONTEXT_H
