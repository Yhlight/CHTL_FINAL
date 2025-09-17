#ifndef CHTL_CJMOD_SYNTAX_H
#define CHTL_CJMOD_SYNTAX_H

#include "Arg.h"
#include <string>
#include <vector>

namespace CHTL {
namespace CJMOD {

class Syntax {
public:
    /**
     * @brief Analyzes a syntax definition string.
     *
     * Parses a string like "$ ** $" into a structured Arg object
     * that can be used for scanning and generation.
     *
     * @param syntax_rule The rule string to analyze.
     * @return An Arg object representing the parsed rule.
     */
    static Arg analyze(const std::string& syntax_rule);

    /**
     * @brief Checks if a given string is a valid JavaScript object literal.
     */
    static bool isObject(const std::string& code);

    /**
     * @brief Checks if a given string is a valid JavaScript function declaration or expression.
     */
    static bool isFunction(const std::string& code);

    /**
     * @brief Checks if a given string is a valid JavaScript array literal.
     */
    static bool isArray(const std::string& code);

    /**
     * @brief Checks if a given string is a valid CHTL JS function call.
     */
    static bool isCHTLJSFunction(const std::string& code);
};

} // namespace CJMOD
} // namespace CHTL

#endif // CHTL_CJMOD_SYNTAX_H
