#ifndef CHTLJS_PARSER_H
#define CHTLJS_PARSER_H

#include "../CHTLJSNode/CHTLJS_BaseNode.h"
#include <string>
#include <vector>
#include <memory>

class CHTLJSParser {
public:
    explicit CHTLJSParser(const std::string& source);
    std::vector<std::unique_ptr<CHTLJS_BaseNode>> parse();

private:
    const std::string& source;
};

#endif // CHTLJS_PARSER_H
