#ifndef CHTLJS_PARSER_H
#define CHTLJS_PARSER_H

#include "../CHTLJSLexer/CHTLJSLexer.h"
#include "../CHTLJSNode/CHTLJS_BaseNode.h"
#include <vector>
#include <memory>

namespace CHTL {

class CHTLJSParser {
public:
    explicit CHTLJSParser(const std::vector<CHTLJSToken>& tokens);
    std::unique_ptr<CHTLJS_BaseNode> parse();
private:
    const std::vector<CHTLJSToken>& tokens;
    int current = 0;
};

} // namespace CHTL

#endif // CHTLJS_PARSER_H
