#ifndef CHTL_LOCAL_STYLE_PARSER_H
#define CHTL_LOCAL_STYLE_PARSER_H

#include <string>
#include <vector>
#include <map>

class LocalStyleParser {
public:
    /**
     * @brief Parses the raw string content of a style block.
     * @param rawContent The string from inside a style {} block.
     * @return A map of CSS property keys to values.
     */
    std::map<std::string, std::string> parse(const std::string& rawContent);
};

#endif // CHTL_LOCAL_STYLE_PARSER_H
