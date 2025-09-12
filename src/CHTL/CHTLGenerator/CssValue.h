#ifndef CHTL_CSS_VALUE_H
#define CHTL_CSS_VALUE_H

#include <string>
#include <stdexcept>
#include <algorithm>
#include "../CHTLLexer/Token.h"

namespace CHTL {

struct CssValue {
    double value;
    std::string unit;

    bool operator==(const CssValue& other) const {
        return value == other.value && unit == other.unit;
    }
};

// Helper function to parse a string lexeme into a CssValue
inline CssValue parseCssValue(const std::string& lexeme) {
    if (lexeme.empty()) {
        throw std::invalid_argument("Cannot parse empty string into CssValue.");
    }

    // Find the first character that is not part of the number
    auto it = std::find_if(lexeme.begin(), lexeme.end(), [](char c) {
        return !std::isdigit(c) && c != '.' && c != '-';
    });

    std::string num_part(lexeme.begin(), it);
    std::string unit_part(it, lexeme.end());

    if (num_part.empty()) {
         // This could happen for lexemes like ".box" which are not numeric values.
         // For now, we'll treat it as a non-numeric value with the lexeme as the "unit".
        return {0.0, unit_part};
    }

    try {
        return {std::stod(num_part), unit_part};
    } catch (const std::invalid_argument& e) {
        // If stod fails, it's likely not a numeric value we can calculate.
        // Treat the whole lexeme as a literal string (unit).
        return {0.0, lexeme};
    }
}


} // namespace CHTL

#endif // CHTL_CSS_VALUE_H
