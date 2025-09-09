#pragma once

#include <string>

namespace CHTL {

class CodeMerger {
public:
    std::string merge(const std::string& html_body, const std::string& css_content, const std::string& js_content);
};

} // namespace CHTL
