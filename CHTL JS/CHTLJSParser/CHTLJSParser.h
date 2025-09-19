#ifndef CHTL_JS_PARSER_H
#define CHTL_JS_PARSER_H

#include "../CHTLJSNode/CHTLJSBaseNode.h"
#include <string>
#include <vector>
#include <memory>

namespace CHTL_JS {

class CHTLJSParser {
public:
    explicit CHTLJSParser(const std::string& source);
    std::vector<std::unique_ptr<CHTLJSBaseNode>> parse();

private:
    std::string source;
    size_t current_pos = 0;
};

} // namespace CHTL_JS

#endif // CHTL_JS_PARSER_H
