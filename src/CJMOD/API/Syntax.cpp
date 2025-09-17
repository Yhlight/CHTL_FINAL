#include "Syntax.h"
#include <sstream>
#include <vector>
#include <iostream>

namespace CHTL {
namespace CJMOD {

Arg Syntax::analyze(const std::string& syntax_rule) {
    Arg arg_obj;
    std::stringstream ss(syntax_rule);
    std::string token;

    while (ss >> token) {
        AtomType type = AtomType::Literal; // Default to literal

        // This is a simplified analysis. A real implementation would be more robust.
        if (token == "$") {
            type = AtomType::Placeholder;
        } else if (token == "$?") {
            type = AtomType::Optional;
        } else if (token == "$!") {
            type = AtomType::Required;
        } else if (token == "$_") {
            type = AtomType::Unordered;
        } else if (token == "...") {
            type = AtomType::Variadic;
        }

        arg_obj.addAtom(token, type);
    }

    return arg_obj;
}

// --- Placeholder Implementations ---

bool Syntax::isObject(const std::string& code) {
    // A simple heuristic: does it start with '{' and end with '}'?
    if (code.length() < 2) return false;
    size_t first = code.find_first_not_of(" \t\n\r");
    size_t last = code.find_last_not_of(" \t\n\r");
    if (first == std::string::npos || last == std::string::npos) return false;
    return code[first] == '{' && code[last] == '}';
}

bool Syntax::isFunction(const std::string& code) {
    // A simple heuristic: does it contain "function" or "=>"?
    return code.find("function") != std::string::npos || code.find("=>") != std::string::npos;
}

bool Syntax::isArray(const std::string& code) {
    // A simple heuristic: does it start with '[' and end with ']'?
    if (code.length() < 2) return false;
    size_t first = code.find_first_not_of(" \t\n\r");
    size_t last = code.find_last_not_of(" \t\n\r");
    if (first == std::string::npos || last == std::string::npos) return false;
    return code[first] == '[' && code[last] == ']';
}

bool Syntax::isCHTLJSFunction(const std::string& code) {
    // This would require more complex parsing. For now, we can use a heuristic.
    // e.g., looks for a pattern like "Identifier { ... };"
    return false; // Not implemented yet
}

} // namespace CJMOD
} // namespace CHTL
