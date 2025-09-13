#ifndef CHTL_JS_GENERATOR_H
#define CHTL_JS_GENERATOR_H

#include "CHTLJSNode.h"
#include <string>
#include <vector>
#include <memory>

namespace CHTL {
namespace CHTLJS {

class CHTLJSGenerator {
public:
    CHTLJSGenerator() = default;
    std::string generate(const std::vector<std::unique_ptr<JSNode>>& ast);
};

} // namespace CHTLJS
} // namespace CHTL

#endif // CHTL_JS_GENERATOR_H
