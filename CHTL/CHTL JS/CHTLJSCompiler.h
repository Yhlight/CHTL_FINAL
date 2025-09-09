#pragma once

#include <string>

namespace CHTL {
namespace CHTL_JS {

class CHTLJSCompiler {
public:
    CHTLJSCompiler();
    std::string compile(const std::string& source);
};

} // namespace CHTL_JS
} // namespace CHTL
