#include "CJMODScanner.h"
#include <vector>
#include <iostream>

namespace CHTL {
namespace CJMOD {

// Helper function to find the start of a balanced expression backwards
size_t findExpressionStart(const std::string& source, size_t from_pos) {
    if (from_pos == 0) return 0;
    int balance = 0;
    size_t pos = from_pos - 1;
    while (true) {
        char c = source[pos];
        if (c == ')') balance++;
        else if (c == '}') balance++;
        else if (c == ']') balance++;
        else if (c == '(') balance--;
        else if (c == '{') balance--;
        else if (c == '[') balance--;

        if (balance == 0 && (isspace(c) || c == ';')) {
            return pos + 1;
        }
        if (pos == 0) {
            return 0;
        }
        pos--;
    }
}

// Helper function to find the end of a balanced expression forwards
size_t findExpressionEnd(const std::string& source, size_t from_pos) {
    int balance = 0;
    size_t pos = from_pos;
    while (pos < source.length()) {
        char c = source[pos];
        if (c == '(') balance++;
        else if (c == '{') balance++;
        else if (c == '[') balance++;
        else if (c == ')') balance--;
        else if (c == '}') balance--;
        else if (c == ']') balance--;

        if (balance == 0 && (isspace(c) || c == ';')) {
            return pos;
        }
        pos++;
    }
    return source.length();
}


Arg CJMODScanner::scan(const Arg& syntax_arg, const std::string& keyword, const std::string& source) {
    size_t keyword_pos = source.find(keyword);
    if (keyword_pos == std::string::npos) {
        return Arg(); // Return empty Arg if keyword not found
    }

    // This is a simplified implementation for a rule like "$ op $"
    if (syntax_arg.size() != 3 || syntax_arg[0].getType() != AtomType::Placeholder || syntax_arg[2].getType() != AtomType::Placeholder) {
        // For now, only support this simple structure
        return Arg();
    }

    // Find the expression on the left of the keyword
    size_t lhs_start = findExpressionStart(source, keyword_pos);
    std::string lhs_val = source.substr(lhs_start, keyword_pos - lhs_start);
    // trim whitespace
    lhs_val.erase(0, lhs_val.find_first_not_of(" \t\n\r"));
    lhs_val.erase(lhs_val.find_last_not_of(" \t\n\r") + 1);


    // Find the expression on the right of the keyword
    size_t rhs_start = keyword_pos + keyword.length();
    size_t rhs_end = findExpressionEnd(source, rhs_start);
    std::string rhs_val = source.substr(rhs_start, rhs_end - rhs_start);
    // trim whitespace
    rhs_val.erase(0, rhs_val.find_first_not_of(" \t\n\r"));
    rhs_val.erase(rhs_val.find_last_not_of(" \t\n\r") + 1);

    if (lhs_val.empty() || rhs_val.empty()) {
        return Arg(); // Failed to find both operands
    }

    // Create a new Arg object with the captured values
    Arg result_arg;
    result_arg.addAtom(syntax_arg[0].getLexeme(), syntax_arg[0].getType());
    result_arg[0].fillValue(lhs_val);

    result_arg.addAtom(syntax_arg[1].getLexeme(), syntax_arg[1].getType());
    result_arg[1].fillValue(keyword);

    result_arg.addAtom(syntax_arg[2].getLexeme(), syntax_arg[2].getType());
    result_arg[2].fillValue(rhs_val);

    return result_arg;
}

} // namespace CJMOD
} // namespace CHTL
