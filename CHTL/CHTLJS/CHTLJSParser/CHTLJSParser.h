#ifndef CHTLJS_PARSER_H
#define CHTLJS_PARSER_H

#include "../CHTLJSLexer/CHTLJSLexer.h"
#include "../CHTLJSNode/CHTLJS_BaseNode.h"
#include <vector>
#include <memory>
#include <string>

namespace CHTL_JS {

class CHTLJSParser {
public:
    explicit CHTLJSParser(const std::string& source, const std::vector<Token>& tokens);
    std::unique_ptr<CHTLJS_BaseNode> parse();
private:
    const std::string& source;
    const std::vector<Token>& tokens;
    int current = 0;
};

} // namespace CHTL_JS

#endif // CHTLJS_PARSER_H
