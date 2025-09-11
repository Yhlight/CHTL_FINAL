#ifndef CHTL_LOCAL_STYLE_PARSER_H
#define CHTL_LOCAL_STYLE_PARSER_H

#include "ParsedStyleBlock.h"
#include <string>

class LocalStyleParser {
public:
    /**
     * @brief Parses the raw string content of a style block into inline styles and global rules.
     * @param rawContent The string from inside a style {} block.
     * @return A ParsedStyleBlock struct containing the parsed data.
     */
    ParsedStyleBlock parse(const std::string& rawContent);
};

#endif // CHTL_LOCAL_STYLE_PARSER_H
